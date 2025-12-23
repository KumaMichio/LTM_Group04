// server/src/test/test_onevn_interactive.c
// Interactive test - Cho phép user nhập đáp án thực tế

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
static struct termios old_termios;

// Helper: Set terminal to non-canonical mode
static void set_noncanonical(void) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

// Helper: Restore terminal
static void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

// Helper: Check if key pressed (non-blocking)
static int kbhit(void) {
    int ch = getchar();
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
#endif

#include "db.h"
#include "dao/dao_rooms.h"
#include "dao/dao_onevn.h"
#include "dao/dao_question.h"

// Interactive game simulation
static void interactive_game_simulation(int64_t room_id, int easy_count, int medium_count, int hard_count) {
    int total_rounds = easy_count + medium_count + hard_count;
    int current_round = 0;
    int easy_done = 0, medium_done = 0, hard_done = 0;
    int score = 0;
    int consecutive_correct = 0;
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║         🎮 INTERACTIVE MULTIPLAYER TEST                ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("\nBạn sẽ chơi như một player trong game multiplayer.\n");
    printf("Nhấn phím A, B, C, hoặc D để trả lời.\n");
    printf("Nhấn Q để thoát.\n\n");
    
#ifndef _WIN32
    set_noncanonical();
#endif
    
    while (current_round < total_rounds) {
        current_round++;
        
        // Select difficulty
        const char *difficulty = NULL;
        if (easy_done < easy_count) {
            difficulty = "EASY";
            easy_done++;
        } else if (medium_done < medium_count) {
            difficulty = "MEDIUM";
            medium_done++;
        } else if (hard_done < hard_count) {
            difficulty = "HARD";
            hard_done++;
        } else {
            break;
        }
        
        // Get question
        Question q;
        if (dao_question_get_random(difficulty, &q) != 0) {
            printf("❌ Không thể lấy câu hỏi\n");
            break;
        }
        
        printf("\n");
        printf("╔════════════════════════════════════════════════════════╗\n");
        printf("║  ROUND %d/%d - %s                                    ║\n", 
               current_round, total_rounds, difficulty);
        printf("╚════════════════════════════════════════════════════════╝\n");
        printf("\nCâu hỏi: %s\n", q.content);
        printf("A. %s\n", q.op_a);
        printf("B. %s\n", q.op_b);
        printf("C. %s\n", q.op_c);
        printf("D. %s\n", q.op_d);
        printf("\n⏱️  Thời gian: 15 giây\n");
        printf("Đáp án của bạn (A/B/C/D, Q để thoát): ");
        fflush(stdout);
        
        // Timer
        time_t start_time = time(NULL);
        char answer = 0;
        int answered = 0;
        
        // Wait for input or timeout
        while ((time(NULL) - start_time) < 15) {
#ifdef _WIN32
            if (_kbhit()) {
                answer = _getch();
#else
            if (kbhit()) {
                answer = getchar();
#endif
                if (answer == 'q' || answer == 'Q') {
#ifndef _WIN32
                    restore_terminal();
#endif
                    printf("\n\n👋 Bạn đã thoát game.\n");
                    printf("Điểm số cuối: %d\n", score);
                    return;
                }
                answer = toupper(answer);
                if (answer >= 'A' && answer <= 'D') {
                    answered = 1;
                    break;
                }
            }
#ifndef _WIN32
            usleep(100000); // 100ms
#else
            Sleep(100); // 100ms on Windows
#endif
        }
        
#ifndef _WIN32
        restore_terminal();
#endif
        
        if (!answered) {
            printf("\n⏱️  HẾT THỜI GIAN! Bạn bị loại!\n");
            printf("Điểm số cuối: %d\n", score);
            return;
        }
        
        printf("%c\n", answer);
        
        // Check answer
        char correct_op = toupper(q.correct_op[0]);
        time_t elapsed = time(NULL) - start_time;
        double time_left = 15.0 - elapsed;
        if (time_left < 0) time_left = 0;
        
        if (answer == correct_op) {
            // Calculate score
            int base_score = 0;
            if (strcmp(difficulty, "EASY") == 0) base_score = 1000;
            else if (strcmp(difficulty, "MEDIUM") == 0) base_score = 1500;
            else if (strcmp(difficulty, "HARD") == 0) base_score = 2000;
            
            double time_percent = (time_left / 15.0) * 100.0;
            int round_score = (int)(base_score * time_percent / 100.0);
            
            // Bonus
            if (consecutive_correct >= 5) round_score += 500;
            else if (consecutive_correct == 4) round_score += 200;
            else if (consecutive_correct == 3) round_score += 100;
            
            score += round_score;
            consecutive_correct++;
            
            printf("✅ ĐÚNG! +%d điểm (Tổng: %d) - Thời gian còn lại: %.1fs\n",
                   round_score, score, time_left);
            printf("   Chuỗi đúng: %d câu liên tiếp\n", consecutive_correct);
        } else {
            printf("❌ SAI! Đáp án đúng là %c. Bạn bị loại!\n", correct_op);
            printf("Điểm số cuối: %d\n", score);
            return;
        }
        
#ifndef _WIN32
        set_noncanonical();
        usleep(1000000); // 1 second pause
#else
        Sleep(1000); // 1 second on Windows
#endif
    }
    
#ifndef _WIN32
    restore_terminal();
#endif
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║                    🏆 CHIẾN THẮNG! 🏆                  ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("\nBạn đã hoàn thành tất cả %d câu hỏi!\n", total_rounds);
    printf("Điểm số cuối: %d\n", score);
    printf("Chuỗi đúng cuối: %d câu liên tiếp\n", consecutive_correct);
}

int main(void) {
    const char *conninfo = getenv("DB_CONN");
    if (!conninfo) {
        fprintf(stderr, "❌ Set DB_CONN environment variable\n");
        return 1;
    }
    
    if (db_connect(conninfo) != 0) {
        fprintf(stderr, "❌ Database connection failed\n");
        return 1;
    }
    
    printf("=== INTERACTIVE MULTIPLAYER TEST ===\n\n");
    
    // Create test room
    int64_t owner_id = 1;
    int easy_count = 5, medium_count = 5, hard_count = 5;
    
    printf("Tạo phòng với config:\n");
    printf("  Dễ: %d câu\n", easy_count);
    printf("  Vừa: %d câu\n", medium_count);
    printf("  Khó: %d câu\n", hard_count);
    printf("  Tổng: %d câu\n\n", easy_count + medium_count + hard_count);
    
    int64_t room_id = 0;
    if (dao_rooms_create_with_config(owner_id, easy_count, medium_count, hard_count, &room_id) == 0) {
        printf("✅ Phòng đã được tạo: Room ID = %ld\n", room_id);
        
        // Start interactive game
        interactive_game_simulation(room_id, easy_count, medium_count, hard_count);
    } else {
        printf("❌ Không thể tạo phòng\n");
    }
    
    db_disconnect();
    return 0;
}

