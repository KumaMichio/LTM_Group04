# 📋 TÍNH NĂNG ĐÃ TRIỂN KHAI - GAME AI LÀ TRIỆU PHÚ

> **Dự án:** Trò chơi "Ai là triệu phú" trực tuyến  
> **Ngôn ngữ:** C/C++ (Server), C++/Qt6 (Client)  
> **Database:** PostgreSQL  
> **Kiến trúc:** Client-Server với TCP Socket

---

## 📑 MỤC LỤC

1. [Tính năng Client (Qt6)](#1-tính-năng-client-qt6)
2. [Tính năng Server (C)](#2-tính-năng-server-c)
3. [Tính năng Database](#3-tính-năng-database)
4. [Tính năng Network & Protocol](#4-tính-năng-network--protocol)
5. [Tính năng Game](#5-tính-năng-game)
6. [Tính năng Bảo mật](#6-tính-năng-bảo-mật)

---

## 1. TÍNH NĂNG CLIENT (Qt6)

### 1.1. Giao diện Đăng nhập & Đăng ký

✅ **LoginWindow** - Cửa sổ đăng nhập/đăng ký
- **Kết nối Server:**
  - Nhập địa chỉ host (mặc định: localhost)
  - Nhập port (mặc định: 9000)
  - Nút "Kết nối" để thiết lập kết nối TCP
  - Hiển thị trạng thái kết nối (Đã kết nối/Chưa kết nối)
  
- **Đăng ký tài khoản:**
  - Form đăng ký với 3 trường: Username, Password, Confirm Password
  - Validation: username tối đa 32 ký tự, password tối thiểu 3 ký tự
  - Kiểm tra password xác nhận khớp
  - Gửi request `CMD_REQ_REGISTER` đến server
  - Hiển thị thông báo thành công/thất bại
  
- **Đăng nhập:**
  - Form đăng nhập với Username và Password
  - Validation đầu vào
  - Gửi request `CMD_REQ_LOGIN` đến server
  - Lưu token sau khi đăng nhập thành công
  - Chuyển sang cửa sổ chọn chế độ chơi
  
- **Đăng xuất:**
  - Nút đăng xuất sau khi đã đăng nhập
  - Gửi request `CMD_REQ_LOGOUT` đến server
  - Xóa token và reset trạng thái

- **UI/UX:**
  - Giao diện gradient đẹp mắt (tím-xanh)
  - Tab widget cho Đăng nhập/Đăng ký
  - Hiển thị thông báo lỗi/thành công với màu sắc phù hợp
  - Disable/enable các nút theo trạng thái kết nối

### 1.2. Cửa sổ Chọn Chế độ Chơi

✅ **GameModeSelectionWindow** - Cửa sổ chọn chế độ
- **Hiển thị:**
  - Chào mừng người chơi với username
  - Tiêu đề game "💰 AI LÀ TRIỆU PHÚ 💰"
  - Giao diện gradient đẹp mắt
  
- **Chế độ chơi:**
  - **QuickMode:** Nút chọn chế độ nhanh (15 câu hỏi)
    - Mở cửa sổ QuickMode khi click
  - **1vN Mode:** Nút chế độ đối kháng (chưa phát triển)
    - Hiện tại bị disable, hiển thị "Sắp ra mắt"

### 1.3. Cửa sổ QuickMode Game

✅ **QuickModeWindow** - Cửa sổ chơi QuickMode
- **Hiển thị câu hỏi:**
  - Label hiển thị nội dung câu hỏi
  - 4 nút đáp án: A, B, C, D với màu sắc khác nhau
  - Hiển thị số câu hỏi hiện tại (ví dụ: "Câu hỏi: 5/15")
  
- **Logic chơi:**
  - 15 câu hỏi mẫu được hardcode (tạm thời)
  - Khi chọn đáp án:
    - Disable tất cả các nút
    - Highlight đáp án đã chọn (xanh nếu đúng, đỏ nếu sai)
    - Highlight đáp án đúng (nếu chọn sai)
    - Delay 1.5 giây trước khi chuyển câu tiếp theo
  
- **Kết quả:**
  - Nếu trả lời đúng: Tự động chuyển sang câu tiếp theo
  - Nếu trả lời sai: Hiển thị cửa sổ LoseWindow
  - Nếu hoàn thành 15 câu: Hiển thị cửa sổ WinWindow
  
- **UI/UX:**
  - Giao diện gradient hồng-đỏ đẹp mắt
  - Nút đáp án có hiệu ứng hover và pressed
  - Câu hỏi được hiển thị trong khung trắng với border

### 1.4. Cửa sổ Kết quả

✅ **WinWindow** - Cửa sổ chiến thắng
- Hiển thị khi hoàn thành 15 câu hỏi
- Thông tin hiển thị:
  - Tiêu đề "🎉 CHÚC MỪNG! 🎉"
  - Điểm số đạt được (ví dụ: "Điểm số: 15/15")
  - Thông điệp chúc mừng
- Nút hành động:
  - "🔄 Chơi lại" - Bắt đầu game mới
  - "🏠 Về menu chính" - Quay lại cửa sổ chọn chế độ

✅ **LoseWindow** - Cửa sổ thua
- Hiển thị khi trả lời sai
- Thông tin hiển thị:
  - Tiêu đề "😔 Rất tiếc!"
  - Số câu đã trả lời đúng
  - Thông điệp động viên
- Nút hành động:
  - "🔄 Chơi lại" - Bắt đầu game mới
  - "🏠 Về menu chính" - Quay lại cửa sổ chọn chế độ

### 1.5. Network Client

✅ **NetworkClient** - Lớp xử lý mạng
- **Kết nối:**
  - Kết nối TCP đến server với host và port
  - Quản lý trạng thái kết nối (connected/disconnected)
  - Xử lý lỗi kết nối (connection refused, host not found, etc.)
  
- **Giao thức:**
  - Gửi/nhận packet với format: Header (8 bytes) + JSON payload
  - Header gồm: cmd (2 bytes), user_id (2 bytes), length (4 bytes)
  - Sử dụng network byte order (Big Endian)
  - Buffer để xử lý packet không đầy đủ
  
- **Commands đã implement:**
  - `CMD_REQ_REGISTER` (0x0101) - Đăng ký
  - `CMD_REQ_LOGIN` (0x0103) - Đăng nhập
  - `CMD_REQ_LOGOUT` (0x0106) - Đăng xuất
  - Xử lý response tương ứng: `CMD_RES_REGISTER`, `CMD_RES_LOGIN`, `CMD_RES_LOGOUT`
  
- **Signals & Slots:**
  - `connected()` - Khi kết nối thành công
  - `disconnected()` - Khi mất kết nối
  - `registerResponse()` - Phản hồi đăng ký
  - `loginResponse()` - Phản hồi đăng nhập
  - `logoutResponse()` - Phản hồi đăng xuất
  - `errorOccurred()` - Khi có lỗi xảy ra

---

## 2. TÍNH NĂNG SERVER (C)

### 2.1. Authentication Service

✅ **auth_service.c** - Dịch vụ xác thực
- **Đăng ký (Register):**
  - `auth_signup()` - Tạo tài khoản mới
  - Hash password bằng bcrypt/Argon2
  - Kiểm tra username đã tồn tại
  - Lưu vào database (dao_users_create)
  - Trả về `AUTH_OK` hoặc mã lỗi
  
- **Đăng nhập (Login):**
  - `auth_login()` - Xác thực người dùng
  - Kiểm tra username và password
  - Tạo session token (64 ký tự)
  - Lưu session vào database (dao_sessions_create)
  - Trả về token và thông tin session
  
- **Dispatcher:**
  - `auth_dispatch()` - Xử lý các command authentication
  - Xử lý `CMD_REQ_REGISTER` (0x0101)
  - Xử lý `CMD_REQ_LOGIN` (0x0103)
  - Gửi response/error phù hợp

### 2.2. QuickMode Service

✅ **quickmode_service.c** - Dịch vụ QuickMode
- **Debug function:**
  - `qm_debug_start()` - Tạo session và round test
  - Tạo quickmode session cho user
  - Lấy câu hỏi ngẫu nhiên từ database
  - Tạo round và lưu vào database
  
- **Dispatcher:**
  - `quickmode_dispatch()` - Xử lý các command QuickMode
  - Command category: 0x05xx
  - (Đang trong quá trình phát triển)

### 2.3. Stats Service

✅ **stats_service.c** - Dịch vụ thống kê
- **Get Profile:**
  - `stats_handle_get_profile()` - Lấy thông tin profile người dùng
  - Lấy thống kê từ database (dao_stats_get_profile)
  - Trả về JSON với thông tin user và stats
  
- **Leaderboard:**
  - `stats_handle_leaderboard()` - Lấy bảng xếp hạng
  - Lấy top players từ database (dao_stats_get_leaderboard)
  - Sắp xếp theo điểm số/win rate
  - Trả về JSON array
  
- **Match History:**
  - `stats_handle_match_history()` - Lấy lịch sử trận đấu
  - Lấy các trận đấu của user từ database
  - Trả về JSON array với thông tin các trận đấu

### 2.4. Friends Service

✅ **friends_service.c** - Dịch vụ bạn bè và chat
- **Tìm kiếm người dùng:**
  - `handle_search_user()` - Tìm kiếm user theo username (partial match)
  - Trả về danh sách users với avatar
  
- **Thông tin bạn bè:**
  - `handle_get_friend_info()` - Lấy thông tin bạn bè với online status
  - `handle_get_pending_requests()` - Lấy danh sách lời mời kết bạn đang chờ
  
- **Mời bạn bè:**
  - `handle_invite_friend()` - Mời bạn bè vào phòng chơi
  - Kiểm tra quan hệ bạn bè và trạng thái online
  - Gửi notification cho bạn bè được mời
  
- **Chat:**
  - `handle_send_dm()` - Gửi tin nhắn riêng tư (Direct Message)
  - `handle_send_room_chat()` - Gửi tin nhắn trong phòng (broadcast)
  
- **Theo dõi trạng thái:**
  - `friends_notify_status_change()` - Thông báo thay đổi trạng thái cho bạn bè
  - `friends_get_user_status()` - Lấy trạng thái hiện tại (online/offline/in_game)
  
- **Dispatcher:**
  - `friends_dispatch()` - Xử lý các command bạn bè và chat (0x02xx, 0x03xx, 0x04xx)

### 2.5. OneVN Service (Multiplayer Mode)

✅ **onevn_service.c** - Dịch vụ chế độ 1vN
- **Game State Management:**
  - Quản lý trạng thái game trong memory (OneVNGameState)
  - Track: round hiện tại, điểm số, trạng thái loại bỏ, chuỗi đúng liên tiếp
  - Hỗ trợ tối đa 100 game đồng thời
  
- **Tính điểm:**
  - Điểm gốc: Dễ (1000), Vừa (1500), Khó (2000)
  - Bonus chuỗi đúng: 3 câu (+100), 4 câu (+200), 5+ câu (+500 mỗi câu)
  - Tính theo thời gian: `remainingPercent = 100 * (timeLeft / totalTime)`
  - Điểm câu = điểm gốc * remainingPercent + bonus
  
- **Logic game:**
  - `handle_start_game()` - Bắt đầu game 1vN (validate owner, member count, tạo session)
  - `send_next_question()` - Gửi câu hỏi tiếp theo cho tất cả players
  - `handle_submit_answer_1vn()` - Xử lý đáp án, tính điểm, loại bỏ
  - `round_timeout_callback()` - Xử lý timeout (15 giây mỗi round)
  - `end_game()` - Kết thúc game, xác định winner, cập nhật stats, hiển thị leaderboard
  
- **Dispatcher:**
  - `onevn_dispatch()` - Xử lý các command 1vN (0x06xx)

### 2.6. Session Manager

✅ **session_manager.c** - Quản lý session client
- **Session tracking:**
  - `session_manager_register()` - Đăng ký session mới
  - `session_manager_unregister()` - Hủy đăng ký session
  - `session_manager_get_by_user_id()` - Lấy session theo user_id
  - `session_manager_set_room()` - Cập nhật room_id của session
  
- **Broadcast:**
  - `session_manager_broadcast_to_room()` - Broadcast message đến tất cả members trong room
  - `session_manager_send_to_user()` - Gửi message đến user cụ thể (nếu online)
  - `session_manager_get_room_sessions()` - Lấy danh sách sessions trong room

### 2.7. Timer System

✅ **timer.c** - Hệ thống timer
- **Timer management:**
  - `timer_create()` - Tạo timer với callback function
  - `timer_cancel()` - Hủy timer
  - `timer_run_callbacks()` - Chạy callbacks cho các timer đã hết hạn
  - Sử dụng cho round timeout trong game 1vN (15 giây)

### 2.8. Dispatcher

✅ **dispatcher.c** - Router xử lý command
- **Routing theo category:**
  - 0x01xx → `auth_dispatch()` - Authentication
  - 0x02xx → `friends_dispatch()` - Friends system (search, get info, pending requests)
  - 0x03xx → `friends_dispatch()` - Chat (DM, room chat)
  - 0x04xx → Room handlers + `friends_dispatch()` - Room management + invite friend
  - 0x05xx → `quickmode_dispatch()` - QuickMode game
  - 0x06xx → `onevn_dispatch()` - Chế độ 1vN (đã hoàn thiện)
  - 0x07xx → `stats_handle_*()` - Stats
  
- **Room Management đã implement:**
  - `CMD_REQ_CREATE_ROOM` (0x0401) - Tạo phòng với config (easy_count, medium_count, hard_count)
  - `CMD_REQ_JOIN_ROOM` (0x0403) - Tham gia phòng (thông báo status change)
  - `CMD_REQ_LEAVE_ROOM` (0x040A) - Rời phòng (thông báo status change)
  - `CMD_REQ_START_GAME` (0x040C) - Bắt đầu game 1vN (đã hoàn thiện)
  - `CMD_REQ_INVITE_FRIEND` (0x0406) - Mời bạn bè vào phòng (đã hoàn thiện)
  
- **Friends handlers đã implement:**
  - `CMD_REQ_ADD_FRIEND` - Gửi notification cho người được mời
  - `CMD_REQ_RESPOND_FRIEND` - Thông báo status khi accept
  - `CMD_REQ_LIST_FRIENDS` - Lấy danh sách bạn bè

### 2.9. Server Core

✅ **server.c** - Server chính
- **TCP Server:**
  - Tạo socket và bind port (mặc định: 9000)
  - Listen cho kết nối từ client
  - Accept kết nối mới
  - Quản lý nhiều client đồng thời
  
- **Client Session:**
  - `ClientSession` struct để quản lý mỗi client
  - Lưu socket, user_id, token
  - Quản lý vòng đời session
  
- **Event Loop:**
  - Xử lý kết nối mới
  - Đọc dữ liệu từ client
  - Parse packet header và payload
  - Dispatch đến handler phù hợp
  - Gửi response về client

### 2.10. Protocol

✅ **protocol.c** - Xử lý giao thức
- **Packet Structure:**
  - Header: cmd (2 bytes), user_id (2 bytes), length (4 bytes)
  - Payload: JSON string
  
- **Helper Functions:**
  - `protocol_send_response()` - Gửi response với JSON payload
  - `protocol_send_error()` - Gửi error response
  - `protocol_send_simple_ok()` - Gửi OK response không có payload

### 2.11. Utilities

✅ **JSON Utils** - Xử lý JSON
- `util_json_get_string()` - Lấy string từ JSON
- `util_json_get_int64()` - Lấy số nguyên từ JSON
- `util_json_get_double()` - Lấy số thực từ JSON
- `util_json_parse_user_id_array()` - Parse array user IDs từ JSON
- `util_json_parse_int64_array()` - Parse array int64 từ JSON
- `util_json_escape()` - Escape string cho JSON
- Parse JSON payload từ client

✅ **Crypto Utils** - Mã hóa
- Hash password (bcrypt/Argon2)
- Generate session token (64 ký tự)

✅ **Timer Utils** - Quản lý thời gian
- Timer cho câu hỏi (15 giây)
- Track response time

---

## 3. TÍNH NĂNG DATABASE

### 3.1. Database Schema (PostgreSQL)

✅ **Bảng users:**
- `user_id` (BIGINT, PRIMARY KEY, AUTO INCREMENT)
- `username` (VARCHAR(32), UNIQUE, NOT NULL)
- `password` (VARCHAR(255), NOT NULL) - Đã hash
- `avatar_img` (VARCHAR(512), NULL)
- `quickmode_games` (INT, DEFAULT 0) - Số game QuickMode đã chơi
- `quickmode_wins` (INT, DEFAULT 0) - Số lần thắng QuickMode
- `onevn_games` (INT, DEFAULT 0) - Số game 1vN đã chơi
- `onevn_wins` (INT, DEFAULT 0) - Số lần thắng 1vN
- `created_at` (TIMESTAMPTZ, DEFAULT NOW())

✅ **Bảng user_sessions:**
- `id` (BIGINT, PRIMARY KEY)
- `user_id` (BIGINT, FOREIGN KEY → users)
- `access_token` (CHAR(64), UNIQUE, NOT NULL)
- `last_heartbeat` (TIMESTAMPTZ, DEFAULT NOW())
- `expires_at` (TIMESTAMPTZ, NOT NULL)
- Index trên `user_id`, `expires_at`, `last_heartbeat`

✅ **Bảng messages:**
- `id` (BIGINT, PRIMARY KEY)
- `sender_id` (BIGINT, FOREIGN KEY → users)
- `receiver_id` (BIGINT, FOREIGN KEY → users, NULL nếu là room chat)
- `room_id` (BIGINT, FOREIGN KEY → room, NULL nếu là DM)
- `message` (TEXT, NOT NULL)
- `created_at` (TIMESTAMPTZ, DEFAULT NOW())
- `is_delivered` (BOOLEAN, DEFAULT FALSE)
- `delivered_at` (TIMESTAMPTZ, NULL)
- `is_read` (BOOLEAN, DEFAULT FALSE)
- `read_at` (TIMESTAMPTZ, NULL)
- Constraint: Phải có receiver_id HOẶC room_id (không cả hai)

✅ **Bảng friend_relationships:**
- `id` (BIGINT, PRIMARY KEY)
- `user_id` (BIGINT, FOREIGN KEY → users)
- `peer_user_id` (BIGINT, FOREIGN KEY → users)
- `status` (TEXT, CHECK: 'PENDING', 'ACCEPTED', 'DECLINED', 'BLOCKED')
- `created_at` (TIMESTAMPTZ, DEFAULT NOW())
- `responded_at` (TIMESTAMPTZ, NULL)
- Unique constraint trên (user_id, peer_user_id)
- Constraint: user_id ≠ peer_user_id

✅ **Bảng question:**
- `question_id` (BIGINT, PRIMARY KEY)
- `difficulty_level` (TEXT, CHECK: 'EASY', 'MEDIUM', 'HARD')
- `content` (TEXT, NOT NULL) - Nội dung câu hỏi
- `opA`, `opB`, `opC`, `opD` (VARCHAR(512), NOT NULL) - 4 đáp án
- `correct_op` (CHAR(1), CHECK: 'A', 'B', 'C', 'D')
- `explanation` (TEXT, NULL) - Giải thích đáp án
- Index trên `difficulty_level`
- Có sample data (6 câu hỏi mẫu)

✅ **Bảng room:**
- `room_id` (BIGINT, PRIMARY KEY)
- `owner_id` (BIGINT, FOREIGN KEY → users)
- `status` (TEXT, CHECK: 'WAITING', 'STARTING', 'IN_PROGRESS', 'FINISHED')
- `max_number_players` (INT, DEFAULT 8)
- `created_at` (TIMESTAMPTZ, DEFAULT NOW())
- `started_at` (TIMESTAMPTZ, NULL)
- `ended_at` (TIMESTAMPTZ, NULL)
- Index trên `status`
- View `rooms` để query dễ dàng

✅ **Bảng room_members:**
- `room_id` (BIGINT, FOREIGN KEY → room)
- `user_id` (BIGINT, FOREIGN KEY → users)
- `joined_at` (TIMESTAMPTZ, DEFAULT NOW())
- PRIMARY KEY (room_id, user_id)

✅ **Bảng onevn_sessions:**
- `session_id` (BIGINT, PRIMARY KEY)
- `room_id` (BIGINT, FOREIGN KEY → room)
- `status` (TEXT, CHECK: 'IN_PROGRESS', 'FINISHED', 'ABORTED')
- `winner_id` (BIGINT, FOREIGN KEY → users, NULL)
- `started_at` (TIMESTAMPTZ, DEFAULT NOW())
- `ended_at` (TIMESTAMPTZ, NULL)
- `players` (JSONB, DEFAULT '[]') - Thông tin players: [{user_id, score, final_rank, eliminated}]
- Unique index trên (room_id, status) WHERE status = 'IN_PROGRESS'
- GIN index trên `players` JSONB

### 3.2. Data Access Objects (DAO)

✅ **dao_users.c** - Quản lý users
- `dao_users_create()` - Tạo user mới
- `dao_users_check_password()` - Kiểm tra password
- `dao_users_find_by_username()` - Lấy user theo username
- `dao_users_find_by_id()` - Lấy user theo ID
- `dao_users_search_by_username()` - Tìm kiếm user theo username (partial match)

✅ **dao_sessions.c** - Quản lý sessions
- `dao_sessions_create()` - Tạo session mới
- `dao_sessions_get_by_token()` - Lấy session theo token
- `dao_sessions_delete()` - Xóa session
- `dao_sessions_cleanup_expired()` - Dọn dẹp session hết hạn

✅ **dao_question.c** - Quản lý câu hỏi
- `dao_question_get_random()` - Lấy câu hỏi ngẫu nhiên theo độ khó
- `dao_question_get_by_id()` - Lấy câu hỏi theo ID

✅ **dao_rooms.c** - Quản lý phòng
- `dao_rooms_create()` - Tạo phòng mới
- `dao_rooms_create_with_config()` - Tạo phòng với config số câu hỏi (easy/medium/hard)
- `dao_rooms_join()` - Tham gia phòng
- `dao_rooms_leave()` - Rời phòng
- `dao_rooms_get_members()` - Lấy danh sách thành viên
- `dao_rooms_get_config()` - Lấy config số câu hỏi của phòng
- `dao_rooms_get_owner()` - Lấy owner_id của phòng
- `dao_rooms_update_status()` - Cập nhật trạng thái phòng

✅ **dao_friends.c** - Quản lý bạn bè
- `dao_friends_send_request()` - Gửi lời mời kết bạn
- `dao_friends_respond_request()` - Phản hồi lời mời
- `dao_friends_list()` - Lấy danh sách bạn bè
- `dao_friends_get_pending_requests()` - Lấy danh sách lời mời đang chờ
- `dao_friends_get_info()` - Lấy thông tin bạn bè với stats
- `dao_friends_are_friends()` - Kiểm tra quan hệ bạn bè
- `dao_friends_update_status()` - Cập nhật trạng thái quan hệ

✅ **dao_chat.c** - Quản lý chat
- `dao_chat_send_dm()` - Gửi tin nhắn 1-1
- `dao_chat_send_room_message()` - Gửi tin nhắn phòng
- `dao_chat_fetch_offline()` - Lấy tin nhắn offline
- `dao_chat_mark_delivered()` - Đánh dấu đã gửi
- `dao_chat_mark_read()` - Đánh dấu đã đọc

✅ **dao_stats.c** - Quản lý thống kê
- `dao_stats_get_profile()` - Lấy profile user
- `dao_stats_get_leaderboard()` - Lấy bảng xếp hạng
- `dao_stats_get_match_history()` - Lấy lịch sử trận đấu
- `dao_stats_record_match()` - Ghi lại kết quả trận đấu
- `dao_stats_update_onevn_game()` - Cập nhật stats sau game 1vN (games, wins, highest_score, avg_response_time)

✅ **dao_onevn.c** - Quản lý 1vN sessions
- `dao_onevn_create_session()` - Tạo session 1vN
- `dao_onevn_update_player()` - Cập nhật điểm player
- `dao_onevn_end_session()` - Kết thúc session

---

## 4. TÍNH NĂNG NETWORK & PROTOCOL

### 4.1. Protocol Structure

✅ **Packet Format:**
```
[Header: 8 bytes]
  - cmd: uint16_t (2 bytes) - Command type
  - user_id: uint16_t (2 bytes) - User ID (0 nếu chưa login)
  - length: uint32_t (4 bytes) - Payload length
  
[Payload: variable length]
  - JSON string (UTF-8)
```

✅ **Byte Order:**
- Network byte order (Big Endian) cho header
- JSON payload là text (UTF-8)

### 4.2. Command Definitions

✅ **0x01xx - Authentication:**
- `CMD_REQ_REGISTER` (0x0101) - Đăng ký
- `CMD_RES_REGISTER` (0x0102) - Response đăng ký
- `CMD_REQ_LOGIN` (0x0103) - Đăng nhập
- `CMD_RES_LOGIN` (0x0104) - Response đăng nhập
- `CMD_NOTIFY_USER_STATUS` (0x0105) - Thông báo trạng thái user
- `CMD_REQ_LOGOUT` (0x0106) - Đăng xuất
- `CMD_RES_LOGOUT` (0x0107) - Response đăng xuất

✅ **0x02xx - Friends System:**
- `CMD_REQ_ADD_FRIEND` (0x0201) - Thêm bạn
- `CMD_RES_ADD_FRIEND` (0x0202) - Response thêm bạn
- `CMD_NOTIFY_FRIEND_REQ` (0x0203) - Thông báo lời mời kết bạn
- `CMD_REQ_RESPOND_FRIEND` (0x0204) - Phản hồi lời mời
- `CMD_RES_RESPOND_FRIEND` (0x0205) - Response phản hồi
- `CMD_REQ_LIST_FRIENDS` (0x0206) - Lấy danh sách bạn
- `CMD_RES_LIST_FRIENDS` (0x0207) - Response danh sách bạn
- `CMD_NOTIFY_FRIEND_STATUS` (0x0208) - Thông báo trạng thái bạn
- `CMD_REQ_SEARCH_USER` (0x0209) - Tìm kiếm user
- `CMD_RES_SEARCH_USER` (0x020A) - Response tìm kiếm
- `CMD_REQ_GET_FRIEND_INFO` (0x020B) - Lấy thông tin bạn bè
- `CMD_RES_GET_FRIEND_INFO` (0x020C) - Response thông tin bạn bè
- `CMD_REQ_GET_PENDING_REQ` (0x020D) - Lấy lời mời đang chờ
- `CMD_RES_GET_PENDING_REQ` (0x020E) - Response lời mời đang chờ

✅ **0x03xx - Chat:**
- `CMD_REQ_SEND_DM` (0x0301) - Gửi tin nhắn riêng
- `CMD_RES_SEND_DM` (0x0302) - Response gửi DM
- `CMD_NOTIFY_DM` (0x0303) - Nhận tin nhắn riêng
- `CMD_REQ_SEND_ROOM_CHAT` (0x0304) - Gửi chat phòng
- `CMD_RES_SEND_ROOM_CHAT` (0x0305) - Response chat phòng
- `CMD_NOTIFY_ROOM_CHAT` (0x0306) - Nhận chat phòng
- `CMD_REQ_FETCH_OFFLINE` (0x0307) - Lấy tin nhắn offline
- `CMD_RES_FETCH_OFFLINE` (0x0308) - Response tin nhắn offline

✅ **0x04xx - Room Management:**
- `CMD_REQ_CREATE_ROOM` (0x0401) - Tạo phòng (với config easy/medium/hard count)
- `CMD_RES_CREATE_ROOM` (0x0402) - Response tạo phòng
- `CMD_REQ_JOIN_ROOM` (0x0403) - Tham gia phòng
- `CMD_RES_JOIN_ROOM` (0x0404) - Response tham gia phòng
- `CMD_NOTIFY_ROOM_UPDATE` (0x0405) - Thông báo cập nhật phòng
- `CMD_REQ_INVITE_FRIEND` (0x0406) - Mời bạn vào phòng
- `CMD_RES_INVITE_FRIEND` (0x0407) - Response mời bạn
- `CMD_NOTIFY_ROOM_INVITE` (0x0408) - Thông báo lời mời phòng
- `CMD_REQ_RESPOND_INVITE` (0x0409) - Phản hồi lời mời phòng
- `CMD_RES_RESPOND_INVITE` (0x040A) - Response phản hồi lời mời
- `CMD_REQ_LEAVE_ROOM` (0x040B) - Rời phòng
- `CMD_RES_LEAVE_ROOM` (0x040B) - Response rời phòng
- `CMD_REQ_START_GAME` (0x040C) - Bắt đầu game 1vN
- `CMD_RES_START_GAME` (0x040D) - Response bắt đầu game

✅ **0x05xx - QuickMode Game:**
- `CMD_NOTIFY_GAME_START` (0x0501) - Bắt đầu game
- `CMD_NOTIFY_QUESTION` (0x0502) - Câu hỏi mới
- `CMD_REQ_SUBMIT_ANSWER` (0x0503) - Gửi đáp án
- `CMD_RES_SUBMIT_ANSWER` (0x0504) - Response gửi đáp án
- `CMD_NOTIFY_ANSWER_RESULT` (0x0505) - Kết quả đáp án
- `CMD_REQ_USE_LIFELINE` (0x0506) - Dùng lifeline
- `CMD_RES_USE_LIFELINE` (0x0507) - Response lifeline
- `CMD_NOTIFY_ROUND_END` (0x0508) - Kết thúc vòng
- `CMD_NOTIFY_GAME_OVER` (0x0509) - Kết thúc game

✅ **0x06xx - 1vN Mode:**
- `CMD_NOTIFY_GAME_START_1VN` (0x0601) - Bắt đầu 1vN
- `CMD_NOTIFY_QUESTION_1VN` (0x0602) - Câu hỏi cho tất cả
- `CMD_REQ_SUBMIT_ANSWER_1VN` (0x0603) - Gửi đáp án 1vN
- `CMD_RES_SUBMIT_ANSWER_1VN` (0x0604) - Response đáp án 1vN
- `CMD_NOTIFY_ELIMINATION` (0x0605) - Thông báo loại bỏ
- `CMD_NOTIFY_GAME_OVER_1VN` (0x0606) - Kết thúc game 1vN

✅ **0x07xx - Stats:**
- `CMD_REQ_GET_PROFILE` (0x0701) - Lấy profile
- `CMD_RES_GET_PROFILE` (0x0702) - Response profile
- `CMD_REQ_LEADERBOARD` (0x0703) - Lấy bảng xếp hạng
- `CMD_RES_LEADERBOARD` (0x0704) - Response bảng xếp hạng
- `CMD_REQ_MATCH_HISTORY` (0x0705) - Lấy lịch sử trận
- `CMD_RES_MATCH_HISTORY` (0x0706) - Response lịch sử trận

✅ **0x08xx - System:**
- `CMD_REQ_PING` (0x0801) - Ping
- `CMD_RES_PING` (0x0802) - Response ping
- `CMD_NOTIFY_ERROR` (0x0803) - Thông báo lỗi
- `CMD_REQ_RECONNECT` (0x0804) - Kết nối lại
- `CMD_RES_RECONNECT` (0x0805) - Response kết nối lại

### 4.3. Helper Functions

✅ **Command Helpers:**
- `get_cmd_category()` - Lấy category từ command (high byte)
- `get_cmd_specific()` - Lấy command cụ thể (low byte)
- `is_request_cmd()` - Kiểm tra có phải request không
- `is_response_cmd()` - Kiểm tra có phải response không

---

## 5. TÍNH NĂNG GAME

### 5.1. QuickMode (Chế độ cơ bản)

✅ **Game Rules (Đã thiết kế):**
- 15 câu hỏi tổng cộng:
  - 5 câu dễ (EASY)
  - 5 câu trung bình (MEDIUM)
  - 5 câu khó (HARD)
- 15 giây mỗi câu hỏi
- 1 câu trả lời sai = Game Over
- Điều kiện thắng: Trả lời đúng cả 15 câu

✅ **Client Implementation:**
- Cửa sổ game với UI đẹp
- Hiển thị câu hỏi và 4 đáp án
- Xử lý click đáp án
- Highlight đáp án đúng/sai
- Tự động chuyển câu tiếp theo hoặc kết thúc game
- 15 câu hỏi mẫu hardcode (tạm thời)

✅ **Server Implementation (Đang phát triển):**
- `qm_debug_start()` - Tạo session test
- Database schema cho quickmode_sessions và quickmode_rounds
- Command definitions đầy đủ (0x05xx)
- Dispatcher handler (chưa hoàn thiện)

### 5.2. 1vN Mode (Chế độ đối kháng)

✅ **Status: Đã hoàn thiện**
- **Game Rules:**
  - Owner tạo phòng và chọn số lượng câu hỏi từng mức độ (dễ/vừa/khó, tối đa tổng 30 câu)
  - Phòng cần ít nhất 2 thành viên để bắt đầu (tối đa 8)
  - Tất cả players cùng trả lời mỗi câu hỏi
  - Trả lời sai hoặc timeout → bị loại
  - Kết thúc khi: chỉ còn 1 player, hết câu hỏi, hoặc tất cả đều sai
  - Người có điểm cao nhất thắng
  
- **Tính điểm:**
  - Điểm gốc: Dễ (1000), Vừa (1500), Khó (2000)
  - Bonus chuỗi đúng: 3 câu (+100), 4 câu (+200), 5+ câu (+500 mỗi câu)
  - Tính theo thời gian: `remainingPercent = 100 * (timeLeft / totalTime)`
  - Điểm câu = điểm gốc * remainingPercent + bonus
  
- **Server Implementation:**
  - ✅ Game state management (OneVNGameState)
  - ✅ Round progression với timer (15 giây mỗi round)
  - ✅ Scoring system đầy đủ
  - ✅ Elimination logic
  - ✅ Broadcast mechanism (gửi câu hỏi, kết quả cho tất cả players)
  - ✅ Leaderboard và kết thúc game
  - ✅ Stats update sau game
  
- **Commands đã implement:**
  - `CMD_REQ_START_GAME` (0x040C) - Bắt đầu game
  - `CMD_NOTIFY_GAME_START_1VN` (0x0601) - Thông báo bắt đầu
  - `CMD_NOTIFY_QUESTION_1VN` (0x0602) - Gửi câu hỏi
  - `CMD_REQ_SUBMIT_ANSWER_1VN` (0x0603) - Gửi đáp án
  - `CMD_NOTIFY_ANSWER_RESULT_1VN` (0x0604) - Kết quả đáp án
  - `CMD_NOTIFY_ELIMINATION` (0x0605) - Thông báo loại bỏ
  - `CMD_NOTIFY_GAME_OVER_1VN` (0x0606) - Kết thúc game với leaderboard

### 5.3. Room-based Game

✅ **Room Management:**
- Tạo phòng với config (CREATE_ROOM với easy_count, medium_count, hard_count)
- Tham gia phòng (JOIN_ROOM)
- Rời phòng (LEAVE_ROOM)
- Mời bạn bè vào phòng (INVITE_FRIEND)
- Database schema đầy đủ
- Dispatcher handlers đã implement
- Broadcast room updates cho tất cả members

✅ **Start Game:**
- Command `CMD_REQ_START_GAME` đã hoàn thiện
- Handler đầy đủ cho game 1vN (onevn_service)
- Validate owner, member count, room config
- Tạo session và khởi tạo game state

---

## 6. TÍNH NĂNG BẢO MẬT

### 6.1. Authentication

✅ **Password Security:**
- Hash password (bcrypt/Argon2) - Không lưu plain text
- Kiểm tra password khi đăng nhập

✅ **Session Management:**
- Tạo session token (64 ký tự) khi đăng nhập
- Lưu session vào database với expires_at
- Cleanup session hết hạn

### 6.2. Input Validation

✅ **Client-side:**
- Validation username (tối đa 32 ký tự)
- Validation password (tối thiểu 3 ký tự)
- Kiểm tra password xác nhận khớp

✅ **Server-side:**
- Parse và validate JSON payload
- Kiểm tra user_id trong session
- Xử lý lỗi và gửi error response

---

## 7. TỔNG KẾT

### ✅ ĐÃ HOÀN THÀNH

1. **Client (Qt6):**
   - ✅ Giao diện đăng nhập/đăng ký đầy đủ
   - ✅ Cửa sổ chọn chế độ chơi
   - ✅ Cửa sổ QuickMode với 15 câu hỏi
   - ✅ Cửa sổ kết quả (Win/Lose)
   - ✅ Network client với TCP socket
   - ✅ Xử lý packet và JSON

2. **Server (C):**
   - ✅ TCP server với multi-client support
   - ✅ Authentication service (Register/Login/Logout)
   - ✅ Protocol handler với packet parsing
   - ✅ Dispatcher routing theo command category
   - ✅ Room management (Create/Join/Leave với config)
   - ✅ Stats service (Profile/Leaderboard/History)
   - ✅ Friends service (Search, Get info, Pending requests, Invite, Status tracking)
   - ✅ Chat service (DM, Room chat với broadcast)
   - ✅ Session manager (Track online users, Broadcast to room/user)
   - ✅ Timer system (15 giây cho mỗi round)
   - ✅ OneVN service (Multiplayer mode đầy đủ)
   - ✅ QuickMode service (đang phát triển)

3. **Database:**
   - ✅ Schema đầy đủ cho tất cả tính năng
   - ✅ DAO layer cho tất cả bảng
   - ✅ Index và constraints hợp lý
   - ✅ Sample data (questions)

4. **Protocol:**
   - ✅ Packet format chuẩn (Header + JSON)
   - ✅ Command definitions đầy đủ (0x01xx - 0x08xx)
   - ✅ Helper functions
   - ✅ JSON parsing utilities (string, int64, double, arrays)
   - ✅ Broadcast mechanism (room broadcast, user notification)

### ⚠️ ĐANG PHÁT TRIỂN

1. **QuickMode Game Logic:**
   - ⚠️ Dispatcher handler chưa hoàn thiện
   - ⚠️ Score calculation chưa implement
   - ⚠️ Timer logic chưa tích hợp đầy đủ

### ❌ CHƯA CÓ

1. **Advanced Features:**
   - ❌ Lifeline system (50:50, etc.) cho QuickMode
   - ❌ List/Search rooms (chỉ có create/join bằng room_id)
   - ❌ Offline message storage (DM có thể gửi nhưng chưa lưu khi user offline)

---

## 8. CẤU TRÚC FILE

### Client (Qt6)
```
client/qt6/
├── main.cpp                    # Entry point
├── LoginWindow.h/cpp/ui        # Cửa sổ đăng nhập
├── GameModeSelectionWindow.h/cpp # Cửa sổ chọn chế độ
├── QuickModeWindow.h/cpp       # Cửa sổ game QuickMode
├── WinWindow.h/cpp             # Cửa sổ thắng
├── LoseWindow.h/cpp            # Cửa sổ thua
├── NetworkClient.h/cpp         # Network client
└── CMakeLists.txt              # Build config
```

### Server (C)
```
server/
├── src/
│   ├── main.c                  # Entry point
│   ├── service/
│   │   ├── auth_service.c       # Authentication
│   │   ├── quickmode_service.c  # QuickMode game
│   │   ├── stats_service.c      # Statistics
│   │   ├── friends_service.c    # Friends & Chat
│   │   ├── onevn_service.c     # 1vN Multiplayer mode
│   │   ├── session_manager.c    # Session & Broadcast
│   │   ├── dispatcher.c        # Command router
│   │   ├── protocol.c          # Protocol helpers
│   │   ├── server.c            # TCP server
│   │   └── client_session.c    # Client session management
│   ├── dao/                    # Data Access Objects
│   │   ├── dao_users.c
│   │   ├── dao_sessions.c
│   │   ├── dao_question.c
│   │   ├── dao_rooms.c
│   │   ├── dao_friends.c
│   │   ├── dao_chat.c
│   │   ├── dao_stats.c
│   │   └── dao_onevn.c
│   └── utils/                  # Utilities
│       ├── json.c
│       ├── crypto.c
│       └── timer.c
├── include/                    # Headers
└── db.sql                      # Database schema
```

---

## 9. KẾT LUẬN

Dự án đã triển khai được một hệ thống game "Ai là triệu phú" với:

- ✅ **Foundation vững chắc:** Client-Server architecture, Database schema, Protocol definitions
- ✅ **Core features:** Authentication, Room management, Basic game flow
- ✅ **Multiplayer mode hoàn chỉnh:** 1vN mode với đầy đủ logic game, scoring, timer, broadcast
- ✅ **Social features:** Friends system (search, invite, status tracking), Chat (DM, room chat)
- ✅ **Real-time features:** Broadcast mechanism, Friend status updates, Room member updates
- ✅ **UI/UX đẹp:** Giao diện Qt6 với gradient và animations
- ⚠️ **Cần hoàn thiện:** QuickMode game logic, Lifeline system

Dự án đã có đầy đủ tính năng cho một game multiplayer với hệ thống xã hội (friends, chat, leaderboard) và chế độ chơi 1vN hoàn chỉnh.

---

**Tài liệu được cập nhật:** 2024  
**Phiên bản:** 2.0

