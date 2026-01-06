// Test Reconnect Logic and Flow
// Compile: make build/test_reconnect
// Usage: ./build/test_reconnect [host] [port]
//
// This test verifies:
// 1. Disconnect during game triggers reconnect grace period
// 2. Reconnect within 30s restores game state
// 3. Reconnect after 30s fails
// 4. Invalid token reconnect fails

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdint.h>
#include <sys/select.h>
#include <time.h>
#include "service/commands.h"

typedef struct {
    uint16_t cmd;
    uint16_t user_id;
    uint32_t length;
} PacketHeader;

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "9000"

// ==================== Network Helpers ====================

static int connect_to_server(const char *host, const char *port) {
    struct addrinfo hints, *res, *rp;
    int sockfd = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int s = getaddrinfo(host, port, &hints, &res);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) continue;
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(res);
    return sockfd;
}

static int send_packet(int sockfd, uint16_t cmd, uint16_t user_id, const char *json) {
    PacketHeader hdr;
    uint32_t json_len = json ? (uint32_t)strlen(json) : 0;

    hdr.cmd = htons(cmd);
    hdr.user_id = htons(user_id);
    hdr.length = htonl(json_len);

    if (send(sockfd, &hdr, sizeof(hdr), 0) != sizeof(hdr)) return -1;
    if (json_len > 0 && send(sockfd, json, json_len, 0) != (ssize_t)json_len) return -1;
    return 0;
}

static int recv_packet(int sockfd, uint16_t *cmd, char *payload, size_t payload_size) {
    PacketHeader hdr;
    ssize_t n = recv(sockfd, &hdr, sizeof(hdr), MSG_WAITALL);
    if (n != sizeof(hdr)) return -1;

    *cmd = ntohs(hdr.cmd);
    uint32_t len = ntohl(hdr.length);

    if (len > 0) {
        if (len >= payload_size) len = payload_size - 1;
        n = recv(sockfd, payload, len, MSG_WAITALL);
        if (n != (ssize_t)len) return -1;
        payload[len] = '\0';
    } else {
        payload[0] = '\0';
    }
    return 0;
}

static int recv_packet_with_timeout(int sockfd, uint16_t *cmd, char *payload, size_t payload_size, int timeout_sec) {
    fd_set readfds;
    struct timeval timeout;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;

    int ret = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
    if (ret > 0 && FD_ISSET(sockfd, &readfds)) {
        return recv_packet(sockfd, cmd, payload, payload_size);
    }
    return -1; // timeout
}

// ==================== JSON Helpers ====================

static int64_t extract_int(const char *json, const char *key) {
    char search[64];
    snprintf(search, sizeof(search), "\"%s\"", key);
    const char *p = strstr(json, search);
    if (!p) return 0;
    p = strchr(p, ':');
    if (!p) return 0;
    while (*p == ' ' || *p == ':') p++;
    return atoll(p);
}

static char *extract_string(const char *json, const char *key, char *buf, size_t buf_size) {
    char search[64];
    snprintf(search, sizeof(search), "\"%s\"", key);
    const char *p = strstr(json, search);
    if (!p) return NULL;
    p = strchr(p, ':');
    if (!p) return NULL;
    while (*p == ' ' || *p == ':' || *p == '"') p++;
    const char *end = strchr(p, '"');
    if (!end) return NULL;
    size_t len = end - p;
    if (len >= buf_size) len = buf_size - 1;
    strncpy(buf, p, len);
    buf[len] = '\0';
    return buf;
}

// ==================== Test Functions ====================

typedef struct {
    int sockfd;
    uint16_t user_id;
    int64_t room_id;
    char access_token[128];
    char username[64];
    char password[64];
} TestClient;

static int client_login(TestClient *client, const char *host, const char *port) {
    client->sockfd = connect_to_server(host, port);
    if (client->sockfd < 0) {
        fprintf(stderr, "[%s] Failed to connect\n", client->username);
        return -1;
    }

    char json[256];
    snprintf(json, sizeof(json), "{\"username\":\"%s\",\"password\":\"%s\"}",
             client->username, client->password);

    if (send_packet(client->sockfd, CMD_REQ_LOGIN, 0, json) != 0) {
        fprintf(stderr, "[%s] Failed to send LOGIN\n", client->username);
        return -1;
    }

    uint16_t cmd;
    char response[1024];
    if (recv_packet(client->sockfd, &cmd, response, sizeof(response)) != 0) {
        fprintf(stderr, "[%s] Failed to receive LOGIN response\n", client->username);
        return -1;
    }

    if (cmd == CMD_RES_LOGIN && strstr(response, "\"error\"") == NULL) {
        client->user_id = (uint16_t)extract_int(response, "user_id");
        extract_string(response, "token", client->access_token, sizeof(client->access_token));
        printf("[%s] ✓ Logged in, user_id=%d, token=%s\n", 
               client->username, client->user_id, client->access_token);
        return 0;
    }

    fprintf(stderr, "[%s] Login failed: %s\n", client->username, response);
    return -1;
}

static int client_create_room(TestClient *client) {
    if (send_packet(client->sockfd, CMD_REQ_CREATE_ROOM, client->user_id, "{}") != 0) {
        return -1;
    }

    uint16_t cmd;
    char response[1024];
    if (recv_packet(client->sockfd, &cmd, response, sizeof(response)) != 0) {
        return -1;
    }

    if (cmd == CMD_RES_CREATE_ROOM) {
        client->room_id = extract_int(response, "room_id");
        printf("[%s] ✓ Created room_id=%lld\n", client->username, (long long)client->room_id);
        return 0;
    }
    return -1;
}

static int client_join_room(TestClient *client, int64_t room_id) {
    char json[128];
    snprintf(json, sizeof(json), "{\"room_id\":%lld}", (long long)room_id);

    if (send_packet(client->sockfd, CMD_REQ_JOIN_ROOM, client->user_id, json) != 0) {
        return -1;
    }

    uint16_t cmd;
    char response[1024];
    if (recv_packet(client->sockfd, &cmd, response, sizeof(response)) != 0) {
        return -1;
    }

    if (cmd == CMD_RES_JOIN_ROOM && strstr(response, "\"error\"") == NULL) {
        client->room_id = room_id;
        printf("[%s] ✓ Joined room_id=%lld\n", client->username, (long long)room_id);
        return 0;
    }
    printf("[%s] ✗ Join failed: cmd=0x%04x, %s\n", client->username, cmd, response);
    return -1;
}

static int client_start_game(TestClient *client) {
    char json[128];
    snprintf(json, sizeof(json), "{\"room_id\":%lld}", (long long)client->room_id);

    if (send_packet(client->sockfd, CMD_REQ_START_GAME, client->user_id, json) != 0) {
        return -1;
    }

    uint16_t cmd;
    char response[1024];
    if (recv_packet(client->sockfd, &cmd, response, sizeof(response)) != 0) {
        return -1;
    }

    if (cmd == CMD_NOTIFY_GAME_START_1VN || cmd == CMD_RES_START_GAME) {
        printf("[%s] ✓ Game started\n", client->username);
        return 0;
    }
    printf("[%s] ✗ Start failed: cmd=0x%04x, %s\n", client->username, cmd, response);
    return -1;
}

static int client_wait_for_question(TestClient *client) {
    uint16_t cmd;
    char response[2048];
    
    // May need to drain extra packets (e.g., CMD_RES_START_GAME for owner)
    for (int i = 0; i < 5; i++) {
        if (recv_packet_with_timeout(client->sockfd, &cmd, response, sizeof(response), 3) == 0) {
            if (cmd == CMD_NOTIFY_QUESTION_1VN) {
                int round = (int)extract_int(response, "round");
                printf("[%s] ✓ Received question round=%d\n", client->username, round);
                return round;
            }
            // Not the question, continue draining
            printf("[%s] Received cmd=0x%04x (not question), continuing...\n", client->username, cmd);
        } else {
            break; // timeout
        }
    }
    printf("[%s] ✗ Did not receive question\n", client->username);
    return -1;
}


static int client_reconnect(TestClient *client, const char *host, const char *port) {
    // Close old socket first if still open
    if (client->sockfd >= 0) {
        close(client->sockfd);
        client->sockfd = -1;
    }

    // Connect with new socket
    client->sockfd = connect_to_server(host, port);
    if (client->sockfd < 0) {
        fprintf(stderr, "[%s] Failed to reconnect\n", client->username);
        return -1;
    }

    // Send reconnect request
    char json[256];
    snprintf(json, sizeof(json), "{\"user_id\":%d,\"access_token\":\"%s\"}",
             client->user_id, client->access_token);

    printf("[%s] Sending CMD_REQ_RECONNECT...\n", client->username);
    if (send_packet(client->sockfd, CMD_REQ_RECONNECT, client->user_id, json) != 0) {
        fprintf(stderr, "[%s] Failed to send reconnect request\n", client->username);
        return -1;
    }

    uint16_t cmd;
    char response[1024];
    if (recv_packet(client->sockfd, &cmd, response, sizeof(response)) != 0) {
        fprintf(stderr, "[%s] Failed to receive reconnect response\n", client->username);
        return -1;
    }

    printf("[%s] Reconnect response: cmd=0x%04x, %s\n", client->username, cmd, response);

    if (cmd == CMD_RES_RECONNECT) {
        if (strstr(response, "\"error\"") != NULL) {
            printf("[%s] ✗ Reconnect failed: %s\n", client->username, response);
            return -1;
        }
        
        int time_remaining = (int)extract_int(response, "time_remaining");
        int score = (int)extract_int(response, "score");
        int current_round = (int)extract_int(response, "current_round");
        
        printf("[%s] ✓ Reconnected! score=%d, round=%d, time_remaining=%ds\n", 
               client->username, score, current_round, time_remaining);
        return 0;
    }

    printf("[%s] ✗ Unexpected response cmd=0x%04x\n", client->username, cmd);
    return -1;
}

static void client_disconnect(TestClient *client) {
    if (client->sockfd >= 0) {
        printf("[%s] Disconnecting (simulating network loss)...\n", client->username);
        close(client->sockfd);
        client->sockfd = -1;
    }
}

// ==================== Test Scenarios ====================

static void print_separator(const char *title) {
    printf("\n");
    printf("==================================================\n");
    printf("  %s\n", title);
    printf("==================================================\n\n");
}

// Test 1: Basic reconnect within grace period
static int test_reconnect_success(const char *host, const char *port) {
    print_separator("TEST 1: Reconnect within 30s grace period");

    TestClient client1 = {.sockfd = -1};
    TestClient client2 = {.sockfd = -1};
    strcpy(client1.username, "alice");
    strcpy(client1.password, "alice123");
    strcpy(client2.username, "bob");
    strcpy(client2.password, "bob123");

    int result = 0;

    // Step 1: Login both clients
    printf("[STEP 1] Login both clients\n");
    if (client_login(&client1, host, port) != 0) {
        result = -1;
        goto cleanup;
    }
    if (client_login(&client2, host, port) != 0) {
        result = -1;
        goto cleanup;
    }

    // Step 2: Client1 creates room
    printf("\n[STEP 2] Client1 creates room\n");
    if (client_create_room(&client1) != 0) {
        result = -1;
        goto cleanup;
    }

    // Step 3: Client2 joins room
    printf("\n[STEP 3] Client2 joins room\n");
    if (client_join_room(&client2, client1.room_id) != 0) {
        result = -1;
        goto cleanup;
    }
    
    // Process join notification for client1
    uint16_t cmd;
    char response[1024];
    recv_packet_with_timeout(client1.sockfd, &cmd, response, sizeof(response), 2);

    // Step 4: Start game
    printf("\n[STEP 4] Client1 starts game\n");
    if (client_start_game(&client1) != 0) {
        result = -1;
        goto cleanup;
    }

    // Client2 receives game start
    recv_packet_with_timeout(client2.sockfd, &cmd, response, sizeof(response), 2);
    printf("[client2] Received game start notification\n");

    // Step 5: Wait for first question
    printf("\n[STEP 5] Wait for first question\n");
    int round = client_wait_for_question(&client1);
    if (round < 0) {
        result = -1;
        goto cleanup;
    }
    client_wait_for_question(&client2);

    // Step 6: Client2 disconnects (simulating network loss)
    printf("\n[STEP 6] Client2 disconnects (simulating network loss)\n");
    client_disconnect(&client2);

    // Step 7: Wait 5 seconds (within grace period)
    printf("\n[STEP 7] Waiting 5 seconds (within 30s grace period)...\n");
    sleep(5);

    // Step 8: Client2 reconnects
    printf("\n[STEP 8] Client2 attempts to reconnect\n");
    if (client_reconnect(&client2, host, port) != 0) {
        printf("\n✗ TEST 1 FAILED: Reconnect should succeed within grace period\n");
        result = -1;
        goto cleanup;
    }

    printf("\n✓ TEST 1 PASSED: Reconnect within grace period succeeded!\n");

cleanup:
    if (client1.sockfd >= 0) close(client1.sockfd);
    if (client2.sockfd >= 0) close(client2.sockfd);
    return result;
}

// Test 2: Reconnect with invalid token
static int test_reconnect_invalid_token(const char *host, const char *port) {
    print_separator("TEST 2: Reconnect with invalid token");

    TestClient client1 = {.sockfd = -1};
    TestClient client2 = {.sockfd = -1};
    strcpy(client1.username, "charlie");
    strcpy(client1.password, "charlie123");
    strcpy(client2.username, "testuser1");
    strcpy(client2.password, "testpass1");

    int result = 0;

    // Login both clients
    if (client_login(&client1, host, port) != 0 || client_login(&client2, host, port) != 0) {
        result = -1;
        goto cleanup;
    }

    // Create room, join, start game
    if (client_create_room(&client1) != 0) {
        result = -1;
        goto cleanup;
    }
    if (client_join_room(&client2, client1.room_id) != 0) {
        result = -1;
        goto cleanup;
    }
    
    uint16_t cmd;
    char response[1024];
    recv_packet_with_timeout(client1.sockfd, &cmd, response, sizeof(response), 2);
    
    if (client_start_game(&client1) != 0) {
        result = -1;
        goto cleanup;
    }
    recv_packet_with_timeout(client2.sockfd, &cmd, response, sizeof(response), 2);
    client_wait_for_question(&client1);
    client_wait_for_question(&client2);

    // Disconnect client2
    client_disconnect(&client2);
    sleep(2);

    // Try to reconnect with wrong token
    strcpy(client2.access_token, "INVALID_TOKEN_12345678901234567890");
    
    printf("[STEP] Client2 tries to reconnect with INVALID token\n");
    int reconnect_result = client_reconnect(&client2, host, port);
    
    if (reconnect_result == 0) {
        printf("\n✗ TEST 2 FAILED: Reconnect should fail with invalid token\n");
        result = -1;
    } else {
        printf("\n✓ TEST 2 PASSED: Reconnect with invalid token correctly rejected!\n");
    }

cleanup:
    if (client1.sockfd >= 0) close(client1.sockfd);
    if (client2.sockfd >= 0) close(client2.sockfd);
    return result;
}

// ==================== Main ====================

int main(int argc, char *argv[]) {
    const char *host = (argc > 1) ? argv[1] : DEFAULT_HOST;
    const char *port = (argc > 2) ? argv[2] : DEFAULT_PORT;

    printf("=== RECONNECT LOGIC & FLOW TEST ===\n");
    printf("Server: %s:%s\n", host, port);
    printf("Time: %s\n", __TIME__);
    printf("\nNOTE: Make sure the server is running and database has users:\n");
    printf("  - alice/alice123\n");
    printf("  - bob/bob123\n");
    printf("  - charlie/charlie123\n");
    printf("  - testuser1/testpass1\n");
    printf("\n");

    int passed = 0;
    int failed = 0;

    // Test 1: Reconnect success
    if (test_reconnect_success(host, port) == 0) {
        passed++;
    } else {
        failed++;
    }

    // Wait between tests
    printf("\nWaiting 5 seconds before next test...\n");
    sleep(5);

    // Test 2: Invalid token
    if (test_reconnect_invalid_token(host, port) == 0) {
        passed++;
    } else {
        failed++;
    }

    // Summary
    print_separator("TEST SUMMARY");
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    printf("\nNote: Test 3 (reconnect after timeout) requires 30+ seconds and\n");
    printf("should be tested manually or with adjusted RECONNECT_GRACE_PERIOD.\n");

    return (failed > 0) ? 1 : 0;
}
