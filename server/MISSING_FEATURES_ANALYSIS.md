# 📋 PHÂN TÍCH CÁC CHỨC NĂNG CÒN THIẾU

So sánh yêu cầu với implementation hiện tại.

---

## ✅ **1. KHỞI ĐỘNG & XÁC THỰC**

### Yêu cầu:
- Sau login thành công, server trả về:
  - ✅ Thông tin tài khoản
  - ❌ Danh sách bạn bè
  - ❌ Thống kê
  - ❌ Tin nhắn offline

### Hiện tại:
- `auth_login()` chỉ trả về `{"token": "..."}`
- Client phải gọi riêng: `CMD_REQ_LIST_FRIENDS`, `CMD_REQ_GET_PROFILE`, `CMD_REQ_FETCH_OFFLINE`

### ❌ **THIẾU:**
1. **Login response tích hợp**: Cần trả về đầy đủ thông tin trong một response
   ```json
   {
     "token": "...",
     "user": { "user_id": 1, "username": "...", "avatar_img": "..." },
     "friends": [...],
     "stats": { "quickmode_games": 0, "quickmode_wins": 0, ... },
     "offline_messages": [...]
   }
   ```

---

## ✅ **2. GIAO DIỆN CHÍNH**

### Yêu cầu:
- Avatar, Username ✅ (có trong profile)
- Danh sách bạn bè với trạng thái Online/Offline/Đang chơi
- Tìm phòng / Tạo phòng mới (Mode 1vN)
- Đấu nhanh (Mode cơ bản)
- Xem bảng xếp hạng / thống kê

### Hiện tại:
- ✅ Có API riêng cho từng chức năng
- ❌ Friend status (online/offline/in-game) chưa có real-time tracking
- ❌ Tìm phòng (search/list rooms) chưa có API

### ❌ **THIẾU:**
1. **Real-time friend status**: 
   - Cần track online/offline/in-game status
   - Broadcast `CMD_NOTIFY_FRIEND_STATUS` khi status thay đổi
   
2. **Tìm phòng (List/Search Rooms)**:
   - `CMD_REQ_LIST_ROOMS` - danh sách phòng đang chờ
   - `CMD_REQ_SEARCH_ROOMS` - tìm phòng theo tiêu chí

---

## ✅ **3. KẾT BẠN & HỆ THỐNG BẠN BÈ**

### Yêu cầu:
- Tìm bạn qua user_id
- Gửi và nhận lời mời kết bạn
- Server gửi thông báo trạng thái online/offline của bạn bè thời gian thực
- Có thể mời bạn bè trực tiếp vào phòng chơi

### Hiện tại:
- ✅ `CMD_REQ_ADD_FRIEND` - gửi lời mời
- ✅ `CMD_REQ_RESPOND_FRIEND` - chấp nhận/từ chối
- ✅ `CMD_REQ_LIST_FRIENDS` - danh sách bạn bè
- ✅ `CMD_NOTIFY_FRIEND_REQ` - thông báo lời mời đến
- ❌ Tìm bạn qua user_id
- ❌ Real-time status update
- ❌ Mời bạn vào phòng

### ❌ **THIẾU:**
1. **Search User by ID/Username**:
   - `CMD_REQ_SEARCH_USER` (0x0209)
   - `CMD_RES_SEARCH_USER` (0x020A)
   - Trả về: `{ "user_id": 123, "username": "...", "avatar_img": "..." }`

2. **Real-time Friend Status**:
   - Track online/offline/in-game trong `ClientSession`
   - Broadcast `CMD_NOTIFY_FRIEND_STATUS` khi login/logout/join_game

3. **Invite Friend to Room**:
   - `CMD_REQ_INVITE_FRIEND` (0x0406) - đã có command nhưng chưa implement
   - `CMD_NOTIFY_ROOM_INVITE` (0x0407) - đã có command nhưng chưa implement
   - Handler trong `dispatcher.c` case 0x04

---

## ✅ **4. CHATTING**

### Yêu cầu:
- Chat 1-1: Gửi tin nhắn trực tiếp giữa hai người chơi
- Chat phòng: Gửi tin cho toàn bộ thành viên trong cùng phòng

### Hiện tại:
- ✅ `CMD_REQ_SEND_DM` - gửi tin nhắn 1-1
- ✅ `CMD_REQ_SEND_ROOM_CHAT` - gửi tin nhắn phòng
- ✅ `CMD_REQ_FETCH_OFFLINE` - lấy tin nhắn offline
- ✅ `CMD_NOTIFY_DM` - nhận tin nhắn real-time (command có nhưng chưa broadcast)
- ✅ `CMD_NOTIFY_ROOM_CHAT` - nhận tin nhắn phòng (command có nhưng chưa broadcast)

### ⚠️ **THIẾU:**
1. **Real-time message broadcasting**:
   - Khi gửi DM, server cần tìm session của người nhận và gửi `CMD_NOTIFY_DM`
   - Khi gửi room chat, server cần broadcast `CMD_NOTIFY_ROOM_CHAT` cho tất cả members
   - Cần hàm `server_broadcast_to_user()` và `server_broadcast_to_room()`

---

## ❌ **5. CHẾ ĐỘ CHƠI - BASIC MODE (Quickmode)**

### Yêu cầu:
- 15 câu hỏi: 5 dễ, 5 trung bình, 5 khó
- Trả lời sai sẽ phải dừng lại
- Tích hợp 1 quyền trợ giúp: 50:50
- Người chơi trả lời đúng hết sẽ giành chiến thắng
- Luồng: Server gửi câu hỏi → Client hiển thị → Người chơi chọn đáp án trong thời gian giới hạn → Server kiểm tra → Sau 15 câu hoặc khi sai thì kết thúc

### Hiện tại:
- ✅ `dao_question_get_random()` - lấy câu hỏi ngẫu nhiên
- ✅ Commands đã định nghĩa (0x0501-0x0509)
- ❌ `quickmode_dispatch()` chỉ trả về `"QUICKMODE_NOT_IMPLEMENTED"`
- ❌ Không có session tracking
- ❌ Không có timer logic
- ❌ Không có answer validation
- ❌ Không có lifeline 50:50
- ❌ Không có score calculation
- ❌ Không update stats sau game

### ❌ **THIẾU HOÀN TOÀN:**
1. **Game Session Management**:
   - Tạo session khi bắt đầu game
   - Track current round (1-15)
   - Track difficulty progression (EASY 1-5, MEDIUM 6-10, HARD 11-15)
   - Track lifeline usage (50:50 chỉ dùng 1 lần)

2. **Question Flow**:
   - `CMD_REQ_START_QUICKMODE` → Server tạo session → Gửi `CMD_NOTIFY_GAME_START`
   - Server gửi `CMD_NOTIFY_QUESTION` với câu hỏi đầu tiên
   - Client hiển thị và bắt đầu timer (15 giây)
   - Client gửi `CMD_REQ_SUBMIT_ANSWER` → Server validate → Gửi `CMD_NOTIFY_ANSWER_RESULT`
   - Nếu đúng → tiếp tục round tiếp theo
   - Nếu sai → `CMD_NOTIFY_GAME_OVER`

3. **Lifeline 50:50**:
   - `CMD_REQ_USE_LIFELINE` → Server xóa 2 đáp án sai → Gửi `CMD_RES_USE_LIFELINE` với 2 đáp án còn lại
   - Chỉ cho phép dùng 1 lần trong toàn bộ game

4. **Timer Logic**:
   - Server track timeout (15 giây)
   - Nếu timeout → tự động kết thúc round và game

5. **Score Calculation**:
   - Base score = 1000 per correct answer
   - Difficulty multiplier: EASY 1.0x, MEDIUM 1.5x, HARD 2.0x
   - Response time bonus: <2s +500, 2-5s +250, 5-10s +100, >10s 0

6. **Stats Update**:
   - Sau game kết thúc, update `users.quickmode_games`, `users.quickmode_wins`
   - Lưu match result (nhưng theo yêu cầu mới: Quickmode không lưu trữ, chỉ update stats)

---

## ❌ **6. CHẾ ĐỘ CHƠI - 1vN MODE**

### Yêu cầu:
- N người chơi trong một phòng sẽ cùng cạnh tranh với nhau
- Mỗi câu hỏi, tất cả cùng trả lời
- Trả lời nhanh sẽ được nhiều điểm hơn, sai không được điểm
- Sau x câu hỏi (x là số câu hỏi chủ phòng chọn), người nhiều điểm nhất giành chiến thắng

### Hiện tại:
- ✅ `dao_onevn_create_session()` - tạo session
- ✅ `dao_onevn_update_players()` - update players JSONB
- ✅ `dao_onevn_end_session()` - kết thúc session
- ✅ Commands đã định nghĩa (0x0601-0x0606)
- ❌ Không có handler trong dispatcher
- ❌ Không có game logic
- ❌ Không có synchronization giữa players
- ❌ Không có scoring system
- ❌ Không có elimination logic

### ❌ **THIẾU HOÀN TOÀN:**
1. **Game Start**:
   - Owner gọi `CMD_REQ_START_GAME` (0x040C)
   - Server tạo `onevn_sessions` với `status='IN_PROGRESS'`
   - Server broadcast `CMD_NOTIFY_GAME_START_1VN` cho tất cả players trong room
   - Server gửi câu hỏi đầu tiên: `CMD_NOTIFY_QUESTION_1VN`

2. **Simultaneous Answering**:
   - Tất cả players nhận cùng 1 câu hỏi
   - Mỗi player gửi `CMD_REQ_SUBMIT_ANSWER_1VN` với `answer` và `response_time_ms`
   - Server tính điểm: `base_score * speed_multiplier` (trả lời nhanh hơn = điểm cao hơn)
   - Server update `players` JSONB với score mới

3. **Elimination**:
   - Nếu player trả lời sai → `eliminated = true`
   - Server broadcast `CMD_NOTIFY_ELIMINATION` với danh sách players bị loại

4. **Round Progression**:
   - Sau mỗi câu hỏi, server kiểm tra:
     - Nếu còn > 1 player chưa bị loại → tiếp tục round tiếp theo
     - Nếu chỉ còn 1 player → kết thúc game, player đó thắng
     - Nếu đã hết số câu hỏi chủ phòng chọn → kết thúc, player có điểm cao nhất thắng

5. **Game End**:
   - Server update `onevn_sessions`: `status='FINISHED'`, `winner_id`, `ended_at`
   - Server update `players` JSONB với `final_rank`
   - Server broadcast `CMD_NOTIFY_GAME_OVER_1VN` với kết quả
   - Server update stats: `users.onevn_games`, `users.onevn_wins`

6. **Scoring System**:
   - Base score = 1000 per correct answer
   - Speed multiplier: 
     - < 1s: 2.0x
     - 1-2s: 1.5x
     - 2-3s: 1.2x
     - 3-5s: 1.0x
     - > 5s: 0.8x
   - Wrong answer = 0 points

---

## ⚠️ **7. KẾT THÚC TRẬN & THỐNG KÊ**

### Yêu cầu:
- Server gửi thông báo kèm kết quả chi tiết (điểm, hạng)
- Client hiển thị bảng kết quả, và nút "Chơi lại / Quay lại sảnh"
- Hệ thống cập nhật vào CSDL:
  - Tổng số trận ✅
  - Tỉ lệ thắng ✅
  - Điểm cao nhất ❌ (không có trong schema mới)
  - Số câu đúng / trung bình thời gian trả lời ❌

### Hiện tại:
- ✅ `users.quickmode_games`, `users.quickmode_wins`, `users.onevn_games`, `users.onevn_wins`
- ✅ `CMD_NOTIFY_GAME_OVER` và `CMD_NOTIFY_GAME_OVER_1VN` (commands có nhưng chưa implement)
- ❌ Không có điểm cao nhất (highest_score)
- ❌ Không có số câu đúng (total_correct_answers)
- ❌ Không có trung bình thời gian trả lời (avg_response_time_ms)

### ❌ **THIẾU:**
1. **Database Schema**:
   - Cần thêm vào `users` table:
     - `highest_score INT` (điểm cao nhất trong Quickmode)
     - `total_correct_answers INT` (tổng số câu đúng)
     - `avg_response_time_ms INT` (trung bình thời gian trả lời, tính bằng milliseconds)

2. **Stats Update Logic**:
   - Sau mỗi game (Quickmode hoặc 1vN), update:
     - `quickmode_games++` hoặc `onevn_games++`
     - `quickmode_wins++` hoặc `onevn_wins++` (nếu thắng)
     - `highest_score = MAX(highest_score, current_score)`
     - `total_correct_answers += correct_answers_in_game`
     - `avg_response_time_ms = (avg_response_time_ms * total_games + current_avg_time) / (total_games + 1)`

3. **Game Over Response**:
   - `CMD_NOTIFY_GAME_OVER` payload:
     ```json
     {
       "session_id": 123,
       "final_score": 25000,
       "correct_answers": 12,
       "total_rounds": 15,
       "win": false,
       "stats": {
         "quickmode_games": 5,
         "quickmode_wins": 2,
         "highest_score": 25000,
         "total_correct_answers": 60,
         "avg_response_time_ms": 3500
       }
     }
     ```

---

## 📊 **TỔNG KẾT**

### ✅ **ĐÃ CÓ:**
- Authentication (Register/Login)
- Friends system (Add/Respond/List)
- Chat system (DM/Room/Offline)
- Room management (Create/Join/Leave)
- Stats API (Profile/Leaderboard/Match History)
- Database schema cơ bản
- Command definitions đầy đủ

### ❌ **THIẾU HOÀN TOÀN:**
1. **Basic Mode (Quickmode) Game Logic** - 0% complete
2. **1vN Mode Game Logic** - 0% complete
3. **Real-time Broadcasting** (messages, friend status, room updates)
4. **Search User** functionality
5. **Invite Friend to Room** implementation
6. **List/Search Rooms** API
7. **Enhanced Stats** (highest_score, total_correct, avg_response_time)

### ⚠️ **CẦN CẢI THIỆN:**
1. **Login Response** - tích hợp đầy đủ thông tin
2. **Friend Status Tracking** - real-time online/offline/in-game
3. **Message Broadcasting** - real-time delivery
4. **Stats Schema** - thêm các trường còn thiếu

---

## 🎯 **ƯU TIÊN IMPLEMENTATION**

### **Priority 1 (Critical):**
1. Basic Mode game logic (Quickmode)
2. 1vN Mode game logic
3. Stats update sau mỗi game

### **Priority 2 (Important):**
4. Real-time broadcasting (messages, friend status)
5. Search User
6. Invite Friend to Room
7. Enhanced login response

### **Priority 3 (Nice to have):**
8. List/Search Rooms
9. Enhanced stats schema

