// server/src/service/quickmode_service.c
#include <stdio.h>
#include "dao/dao_question.h"
#include "service/quickmode_service.h"
#include "service/commands.h"
#include "service/protocol.h"
#include "service/client_session.h"
#include <inttypes.h>

// Quickmode không lưu vào DB, chỉ lưu trong memory
// Sau khi game kết thúc, chỉ update stats trong users table

int qm_debug_start(int64_t user_id) {
    (void)user_id; // TODO: Implement game logic without DB storage
    
    Question q;
    if (dao_question_get_random("EASY", &q) != 0) {
        fprintf(stderr, "[QM] get_random question failed\n");
        return -1;
    }

    printf("=== QUICKMODE DEBUG ===\n");
    printf("User: %" PRId64 "\n", user_id);
    printf("Question %" PRId64 "\n", q.question_id);
    printf("Q: %s\n", q.content);
    printf("A: %s\n", q.op_a);
    printf("B: %s\n", q.op_b);
    printf("C: %s\n", q.op_c);
    printf("D: %s\n", q.op_d);
    printf("Correct: %s\n", q.correct_op);

    return 0;
}

void quickmode_dispatch(ClientSession *sess, uint16_t cmd, const char *payload, uint32_t payload_len) {
    (void)payload_len; (void)payload;
    // Minimal placeholder: return error for unknown commands
    protocol_send_error(sess, cmd, "QUICKMODE_NOT_IMPLEMENTED");
}
