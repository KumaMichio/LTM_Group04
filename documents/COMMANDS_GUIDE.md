# 📋 HƯỚNG DẪN COMMAND TYPES

## 1. ĐỀ CẬP TỐI

Các command types trong dự án được định nghĩa tập trung trong file:

```
📁 server/include/service/commands.h
```

## 2. CẤU TRÚC ĐỊNH NGHĨA

Command type là một `uint16_t` với format:
```
0xMAJORMINOR

- MAJOR (high byte): Thể loại lệnh
- MINOR (low byte): Lệnh cụ thể
```

### Ví dụ:
- `CMD_REQ_REGISTER = 0x0101`
  - MAJOR = 0x01 (Authentication)
  - MINOR = 0x01 (Request Register)

- `CMD_RES_REGISTER = 0x0102`
  - MAJOR = 0x01 (Authentication)
  - MINOR = 0x02 (Response Register)

## 3. CÁC THỂ LOẠI LỆNH

### 🔐 **0x01xx – Authentication (Xác thực)**
```c
CMD_REQ_REGISTER        0x0101    // Đăng ký
CMD_RES_REGISTER        0x0102
CMD_REQ_LOGIN           0x0103    // Đăng nhập
CMD_RES_LOGIN           0x0104
CMD_NOTIFY_USER_STATUS  0x0105    // Thông báo trạng thái
CMD_REQ_LOGOUT          0x0106    // Đăng xuất
CMD_RES_LOGOUT          0x0107
```

### 👥 **0x02xx – Friends (Bạn bè)**
```c
CMD_REQ_ADD_FRIEND      0x0201    // Thêm bạn
CMD_RES_ADD_FRIEND      0x0202
CMD_NOTIFY_FRIEND_REQ   0x0203    // Yêu cầu bạn đến
CMD_REQ_RESPOND_FRIEND  0x0204    // Phản hồi yêu cầu
CMD_RES_RESPOND_FRIEND  0x0205
CMD_REQ_LIST_FRIENDS    0x0206    // Danh sách bạn
CMD_RES_LIST_FRIENDS    0x0207
CMD_NOTIFY_FRIEND_STATUS 0x0208   // Trạng thái bạn
```

### 💬 **0x03xx – Chat (Tin nhắn)**
```c
CMD_REQ_SEND_DM         0x0301    // Gửi tin riêng
CMD_RES_SEND_DM         0x0302
CMD_NOTIFY_DM           0x0303    // Nhận tin riêng
CMD_REQ_SEND_ROOM_CHAT  0x0304    // Gửi chat room
CMD_RES_SEND_ROOM_CHAT  0x0305
CMD_NOTIFY_ROOM_CHAT    0x0306    // Nhận chat room
CMD_REQ_FETCH_OFFLINE   0x0307    // Lấy tin offline
CMD_RES_FETCH_OFFLINE   0x0308
```

### 🏠 **0x04xx – Rooms (Phòng)**
```c
CMD_REQ_CREATE_ROOM     0x0401    // Tạo phòng
CMD_RES_CREATE_ROOM     0x0402
CMD_REQ_JOIN_ROOM       0x0403    // Tham gia phòng
CMD_RES_JOIN_ROOM       0x0404
CMD_NOTIFY_ROOM_UPDATE  0x0405    // Cập nhật phòng
CMD_REQ_INVITE_FRIEND   0x0406    // Mời bạn
CMD_NOTIFY_ROOM_INVITE  0x0407    // Nhận lời mời
CMD_REQ_RESPOND_INVITE  0x0408    // Phản hồi lời mời
CMD_RES_RESPOND_INVITE  0x0409
CMD_REQ_LEAVE_ROOM      0x040A    // Rời phòng
CMD_RES_LEAVE_ROOM      0x040B
CMD_REQ_START_GAME      0x040C    // Bắt đầu game
CMD_RES_START_GAME      0x040D
```

### 🎮 **0x05xx – Game: Quickmode (Solo 15Q)**
```c
CMD_NOTIFY_GAME_START   0x0501    // Bắt đầu game
CMD_NOTIFY_QUESTION     0x0502    // Câu hỏi mới
CMD_REQ_SUBMIT_ANSWER   0x0503    // Gửi đáp án
CMD_RES_SUBMIT_ANSWER   0x0504
CMD_NOTIFY_ANSWER_RESULT 0x0505   // Kết quả đáp án
CMD_REQ_USE_LIFELINE    0x0506    // Dùng lifeline
CMD_RES_USE_LIFELINE    0x0507
CMD_NOTIFY_ROUND_END    0x0508    // Kết thúc vòng
CMD_NOTIFY_GAME_OVER    0x0509    // Kết thúc game
```

### ⚔️ **0x06xx – Game: 1vN (Competitive)**
```c
CMD_NOTIFY_GAME_START_1VN 0x0601  // Bắt đầu 1vN
CMD_NOTIFY_QUESTION_1VN   0x0602  // Câu hỏi cho tất cả
CMD_REQ_SUBMIT_ANSWER_1VN 0x0603  // Gửi đáp án
CMD_RES_SUBMIT_ANSWER_1VN 0x0604
CMD_NOTIFY_ELIMINATION    0x0605  // Người chơi bị loại
CMD_NOTIFY_GAME_OVER_1VN  0x0606  // Kết thúc game
```

### 📊 **0x07xx – Stats & Leaderboard**
```c
CMD_REQ_GET_PROFILE     0x0701    // Lấy profile
CMD_RES_GET_PROFILE     0x0702
CMD_REQ_LEADERBOARD     0x0703    // Lấy bảng xếp hạng
CMD_RES_LEADERBOARD     0x0704
CMD_REQ_MATCH_HISTORY   0x0705    // Lịch sử trận
CMD_RES_MATCH_HISTORY   0x0706
```

### 🔌 **0x08xx – System (Hệ thống)**
```c
CMD_REQ_PING            0x0801    // Ping kiểm tra
CMD_RES_PING            0x0802
CMD_NOTIFY_ERROR        0x0803    // Thông báo lỗi
CMD_REQ_RECONNECT       0x0804    // Kết nối lại
CMD_RES_RECONNECT       0x0805
```

## 4. CÁCH SỬ DỤNG

### ✅ Cách cũ (KHÔNG nên dùng):
```c
case 0x0101:  // Khó hiểu
    protocol_send_simple_ok(sess, 0x0102);
```

### ✅ Cách mới (NÊN dùng):
```c
#include "service/commands.h"

case CMD_REQ_REGISTER:
    protocol_send_simple_ok(sess, CMD_RES_REGISTER);
```

## 5. HÀM HELPER

File `commands.h` cung cấp các function helper:

```c
// Lấy category (high byte)
uint8_t cat = get_cmd_category(0x0101);  // Returns 0x01

// Lấy command cụ thể (low byte)
uint8_t cmd = get_cmd_specific(0x0101);   // Returns 0x01

// Kiểm tra xem có phải request không
if (is_request_cmd(CMD_REQ_REGISTER)) { }   // true

// Kiểm tra xem có phải response không
if (is_response_cmd(CMD_RES_REGISTER)) { }  // true
```

## 6. CẬP NHẬT HIỆN TẠI

✅ **auth_service.c** - Đã cập nhật sử dụng CMD_* constants

🔄 **Cần cập nhật:**
- `dispatcher.c` (chat, friends, rooms handlers)
- `stats_service.c` (stats commands)
- `quickmode_service.c` (game commands)
- Các file dispatcher khác

## 7. QUY TẮC ĐẶT TÊN

### Pattern REQ/RES:
- **REQ** = Request (request từ client)
- **RES** = Response (phản hồi từ server)
- Thường REQ + RES đi thành cặp (0x0101, 0x0102)

### Pattern NOTIFY:
- **NOTIFY** = Thông báo/broadcast từ server
- Không có REQ tương ứng (server gửi chủ động)

## 8. Thêm Command Mới

Khi thêm command mới:

1. **Chọn MAJOR category:**
   - 0x01 = Auth
   - 0x02 = Friends
   - 0x03 = Chat
   - 0x04 = Rooms
   - 0x05 = Game Quickmode
   - 0x06 = Game 1vN
   - 0x07 = Stats
   - 0x08 = System

2. **Chọn MINOR** (01, 02, 03, ...)

3. **Thêm define vào `commands.h`:**
   ```c
   #define CMD_REQ_NEW_FEATURE  0x02XX  // Thêm ở category 0x02
   #define CMD_RES_NEW_FEATURE  0x02YY
   ```

4. **Include `commands.h` trong handler file**

5. **Sử dụng CMD_* thay vì hard-code**

---

**Tóm tắt:** Commands được quản lý tập trung trong `commands.h` để dễ bảo trì và tránh lỗi hard-code!
