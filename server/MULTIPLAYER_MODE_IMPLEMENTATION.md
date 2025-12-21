# 🎮 MULTIPLAYER MODE (1vN) - TÀI LIỆU TRIỂN KHAI

## 📋 TỔNG QUAN

Đã hoàn thiện tính năng multiplayer mode (1vN) cho game "Ai là triệu phú" với đầy đủ logic game, tính điểm, và quản lý phòng.

---

## ✅ CÁC TÍNH NĂNG ĐÃ TRIỂN KHAI

### 1. Database Schema Updates

**File:** `server/migration_add_room_config.sql`

- Thêm các cột vào bảng `room`:
  - `easy_count` (INT) - Số câu hỏi dễ
  - `medium_count` (INT) - Số câu hỏi vừa
  - `hard_count` (INT) - Số câu hỏi khó
  - Constraint: Tổng số câu hỏi ≤ 30

### 2. DAO Layer Updates

**File:** `server/include/dao/dao_rooms.h`, `server/src/dao/dao_rooms.c`

**Các hàm mới:**
- `dao_rooms_create_with_config()` - Tạo phòng với config số câu hỏi
- `dao_rooms_get_config()` - Lấy config của phòng
- `dao_rooms_get_owner()` - Lấy owner_id của phòng

### 3. OneVN Service

**File:** `server/include/service/onevn_service.h`, `server/src/service/onevn_service.c`

**Tính năng chính:**

#### 3.1. Game State Management
- Quản lý trạng thái game trong memory (OneVNGameState)
- Track: round hiện tại, điểm số, trạng thái loại bỏ, chuỗi đúng liên tiếp
- Hỗ trợ tối đa 100 game đồng thời

#### 3.2. Tính Điểm
- **Điểm gốc:**
  - Dễ: 1000 điểm
  - Vừa: 1500 điểm
  - Khó: 2000 điểm

- **Điểm bonus cho chuỗi đúng:**
  - 3 câu liên tiếp: +100 điểm
  - 4 câu liên tiếp: +200 điểm
  - 5+ câu liên tiếp: +500 điểm mỗi câu

- **Tính theo thời gian:**
  ```
  remainingPercent = 100 * (timeLeft / totalTime)
  Điểm câu = điểm gốc * remainingPercent + bonus
  ```

#### 3.3. Logic Game

**Bắt đầu game:**
1. Owner tạo phòng với config (easy_count, medium_count, hard_count)
2. Players join phòng (tối thiểu 2, tối đa 8)
3. Owner bắt đầu game → Server:
   - Tạo 1vN session
   - Khởi tạo điểm = 0, eliminated = false cho tất cả players
   - Cập nhật room status = IN_PROGRESS
   - Broadcast `CMD_NOTIFY_GAME_START_1VN`

**Mỗi round:**
1. Server chọn câu hỏi ngẫu nhiên (theo độ khó còn lại)
2. Server broadcast câu hỏi cho tất cả players (`CMD_NOTIFY_QUESTION_1VN`)
3. Players gửi đáp án với thời gian phản hồi (`CMD_REQ_SUBMIT_ANSWER_1VN`)
4. Server chấm điểm và cập nhật trạng thái

**Kết thúc game:**
- Chỉ còn 1 player chưa bị loại → Thắng ngay
- Hết số câu hỏi → Người điểm cao nhất thắng
- Tất cả đều sai → Không có người thắng
- Server gửi kết quả cuối với leaderboard (`CMD_NOTIFY_GAME_OVER_1VN`)

#### 3.4. Commands Handled

- `CMD_REQ_START_GAME` (0x040C) - Bắt đầu game 1vN
- `CMD_REQ_SUBMIT_ANSWER_1VN` (0x0603) - Gửi đáp án
- `CMD_NOTIFY_GAME_START_1VN` (0x0601) - Thông báo bắt đầu game
- `CMD_NOTIFY_QUESTION_1VN` (0x0602) - Gửi câu hỏi
- `CMD_RES_SUBMIT_ANSWER_1VN` (0x0604) - Kết quả đáp án
- `CMD_NOTIFY_ELIMINATION` (0x0605) - Thông báo loại bỏ
- `CMD_NOTIFY_GAME_OVER_1VN` (0x0606) - Kết thúc game

### 4. Dispatcher Updates

**File:** `server/src/service/dispatcher.c`

- Cập nhật `CMD_REQ_CREATE_ROOM` để nhận config (easy_count, medium_count, hard_count)
- Route `CMD_REQ_START_GAME` đến `onevn_dispatch()`
- Route commands 0x06xx đến `onevn_dispatch()`

### 5. JSON Utils

**File:** `server/include/utils/json.h`, `server/src/utils/json.c`

- Thêm hàm `util_json_get_double()` để parse số thực từ JSON

---

## 📝 CẤU TRÚC DỮ LIỆU

### OneVNGameState
```c
typedef struct {
    int64_t session_id;
    int64_t room_id;
    int current_round;
    int total_rounds;
    int easy_count, medium_count, hard_count;
    int easy_done, medium_done, hard_done;
    char current_difficulty[16];
    Question current_question;
    int64_t *player_ids;
    int player_count;
    int *player_scores;
    int *player_consecutive_correct;
    int *player_eliminated;
    int64_t *player_answered_round;
} OneVNGameState;
```

### Players JSONB Format
```json
[
  {
    "user_id": 123,
    "score": 5000,
    "eliminated": false,
    "final_rank": 1
  },
  ...
]
```

---

## 🔄 FLOW GAME

### 1. Tạo phòng và chuẩn bị
```
Client (Owner) → CMD_REQ_CREATE_ROOM {easy_count, medium_count, hard_count}
Server → CMD_RES_CREATE_ROOM {room_id, config}

Client (Players) → CMD_REQ_JOIN_ROOM {room_id}
Server → CMD_RES_JOIN_ROOM
```

### 2. Bắt đầu game
```
Client (Owner) → CMD_REQ_START_GAME {room_id}
Server:
  - Tạo 1vN session
  - Khởi tạo game state
  - Broadcast CMD_NOTIFY_GAME_START_1VN {session_id, total_rounds}
```

### 3. Mỗi round
```
Server → CMD_NOTIFY_QUESTION_1VN {round, question, options, time_limit}
Client → CMD_REQ_SUBMIT_ANSWER_1VN {session_id, round, answer, time_left}
Server → CMD_RES_SUBMIT_ANSWER_1VN {correct, score, total_score, eliminated}
```

### 4. Kết thúc game
```
Server → CMD_NOTIFY_GAME_OVER_1VN {winner_id, leaderboard}
```

---

## ⚠️ LƯU Ý VÀ HẠN CHẾ

### Đã implement:
- ✅ Logic tính điểm đầy đủ (base + bonus + time)
- ✅ Quản lý game state trong memory
- ✅ Loại bỏ players khi trả lời sai
- ✅ Kiểm tra điều kiện kết thúc game
- ✅ Leaderboard sorting

### Cần hoàn thiện:
- ⚠️ **Broadcast mechanism:** Hiện tại chưa có cơ chế broadcast thực sự đến tất cả players trong room. Cần implement:
  - Hàm `server_broadcast_to_room()` để gửi message đến tất cả sessions trong room
  - Quản lý active sessions per room
  
- ⚠️ **Parse members JSON:** Hiện tại dùng simple string parsing, nên dùng JSON parser chuyên nghiệp hơn

- ⚠️ **Timer management:** Cần implement timer 15 giây cho mỗi câu hỏi và tự động kết thúc round khi hết thời gian

- ⚠️ **Stats update:** Cần gọi `dao_stats` để cập nhật `onevn_games` và `onevn_wins` sau khi game kết thúc

- ⚠️ **Error handling:** Cần xử lý các trường hợp:
  - Player disconnect giữa chừng
  - Room bị xóa khi game đang chạy
  - Database errors

---

## 🚀 CÁCH SỬ DỤNG

### 1. Chạy migration
```sql
\i server/migration_add_room_config.sql
```

### 2. Build server
```bash
cd server
make
```

### 3. Test flow
1. Client A: Tạo phòng với config
   ```json
   {"easy_count": 10, "medium_count": 10, "hard_count": 10}
   ```

2. Client B, C: Join phòng

3. Client A: Bắt đầu game

4. Server: Gửi câu hỏi đầu tiên

5. Clients: Gửi đáp án

6. Server: Tính điểm và tiếp tục hoặc kết thúc game

---

## 📊 VÍ DỤ TÍNH ĐIỂM

**Scenario:**
- Câu hỏi: HARD (2000 điểm gốc)
- Thời gian còn lại: 10 giây / 15 giây = 66.67%
- Chuỗi đúng: 5 câu liên tiếp

**Tính toán:**
```
Base score = 2000 * 0.6667 = 1333 điểm
Bonus = 500 điểm (5 câu liên tiếp)
Tổng = 1333 + 500 = 1833 điểm
```

---

## 🔧 FILES ĐÃ TẠO/SỬA

### Files mới:
- `server/include/service/onevn_service.h`
- `server/src/service/onevn_service.c`
- `server/migration_add_room_config.sql`
- `server/MULTIPLAYER_MODE_IMPLEMENTATION.md`

### Files đã sửa:
- `server/include/dao/dao_rooms.h`
- `server/src/dao/dao_rooms.c`
- `server/include/utils/json.h`
- `server/src/utils/json.c`
- `server/src/service/dispatcher.c`

---

## 📚 TÀI LIỆU THAM KHẢO

- Command definitions: `server/include/service/commands.h`
- Database schema: `server/db.sql`
- Protocol: `server/include/service/protocol.h`

---

**Tạo bởi:** AI Assistant  
**Ngày:** $(date)  
**Version:** 1.0

