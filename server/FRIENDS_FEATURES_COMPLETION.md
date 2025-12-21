# Hoàn thiện tính năng bạn bè - Server

## Tổng quan
Đã hoàn thiện toàn bộ tính năng bạn bè cho server, bao gồm: kết bạn, xem thông tin bạn bè, xem trạng thái online/offline/in-game, mời vào phòng chơi, và chat (DM và room chat).

## Các tính năng đã implement

### 1. Tìm kiếm người dùng
- **Command**: `CMD_REQ_SEARCH_USER` (0x0209)
- **Response**: `CMD_RES_SEARCH_USER` (0x020A)
- **Chức năng**: Tìm kiếm người dùng theo username (partial match, case-insensitive)
- **Payload request**: `{"query": "username", "limit": 20}`
- **Payload response**: JSON array các user: `[{"user_id": 1, "username": "...", "avatar_img": "..."}, ...]`
- **DAO**: `dao_users_search_by_username()`

### 2. Xem thông tin bạn bè với trạng thái
- **Command**: `CMD_REQ_GET_FRIEND_INFO` (0x020B)
- **Response**: `CMD_RES_GET_FRIEND_INFO` (0x020C)
- **Chức năng**: Lấy thông tin chi tiết của bạn bè bao gồm:
  - Thông tin cơ bản (user_id, username, avatar)
  - Thống kê (quickmode_games, quickmode_wins, onevn_games, onevn_wins)
  - Trạng thái quan hệ (friend_status: ACCEPTED, PENDING, NONE)
  - Trạng thái online (online_status: "online", "offline", "in_game")
  - Room ID hiện tại (nếu đang trong phòng)
- **Payload request**: `{"friend_id": 123}`
- **Payload response**: `{"user_id": 123, "username": "...", "avatar_img": "...", "quickmode_games": 10, ..., "online_status": "online", "room_id": 0}`
- **DAO**: `dao_friends_get_info()`

### 3. Xem danh sách lời mời kết bạn đang chờ
- **Command**: `CMD_REQ_GET_PENDING_REQ` (0x020D)
- **Response**: `CMD_RES_GET_PENDING_REQ` (0x020E)
- **Chức năng**: Lấy danh sách các lời mời kết bạn đang chờ phản hồi
- **Payload response**: JSON array: `[{"user_id": 1, "username": "...", "avatar_img": "...", "created_at": "..."}, ...]`
- **DAO**: `dao_friends_get_pending_requests()`

### 4. Mời bạn bè vào phòng chơi
- **Command**: `CMD_REQ_INVITE_FRIEND` (0x0406)
- **Response**: `CMD_RES_INVITE_FRIEND` (0x0407)
- **Notification**: `CMD_NOTIFY_ROOM_INVITE` (0x0408) - gửi cho bạn bè được mời
- **Chức năng**: 
  - Kiểm tra quan hệ bạn bè
  - Kiểm tra bạn bè có online không
  - Gửi thông báo mời vào phòng cho bạn bè
- **Payload request**: `{"friend_id": 123, "room_id": 456}` (room_id có thể lấy từ session)
- **Payload notification**: `{"from_user_id": 1, "from_username": "...", "room_id": 456}`
- **Service**: `friends_dispatch()` -> `handle_invite_friend()`

### 5. Theo dõi và thông báo trạng thái bạn bè
- **Notification**: `CMD_NOTIFY_FRIEND_STATUS` (0x0208)
- **Chức năng**: 
  - Tự động thông báo cho tất cả bạn bè khi:
    - User login (status: "online")
    - User join room (status: "in_game", room_id: ...)
    - User leave room (status: "online")
    - User logout/disconnect (status: "offline")
  - Broadcast đến tất cả bạn bè của user
- **Payload notification**: `{"user_id": 123, "status": "online|offline|in_game", "room_id": 456}`
- **Service**: `friends_notify_status_change()` -> `broadcast_status_to_friends()`
- **Helper**: `friends_get_user_status()` - lấy trạng thái hiện tại của user

### 6. Chat - Direct Messages (DM)
- **Command**: `CMD_REQ_SEND_DM` (0x0301)
- **Response**: `CMD_RES_SEND_DM` (0x0302)
- **Notification**: `CMD_NOTIFY_DM` (0x0303) - gửi cho người nhận
- **Chức năng**: 
  - Gửi tin nhắn riêng tư giữa 2 người dùng
  - Nếu người nhận online: gửi real-time notification
  - Nếu người nhận offline: có thể lưu vào DB (chưa implement)
- **Payload request**: `{"to_user_id": 123, "message": "Hello!"}`
- **Payload notification**: `{"from_user_id": 1, "from_username": "...", "message": "Hello!", "timestamp": 1234567890}`
- **Service**: `friends_dispatch()` -> `handle_send_dm()`

### 7. Chat - Room Chat
- **Command**: `CMD_REQ_SEND_ROOM_CHAT` (0x0304)
- **Response**: `CMD_RES_SEND_ROOM_CHAT` (0x0305)
- **Notification**: `CMD_NOTIFY_ROOM_CHAT` (0x0306) - broadcast cho tất cả thành viên trong phòng
- **Chức năng**: 
  - Gửi tin nhắn trong phòng chơi
  - Broadcast đến tất cả thành viên trong phòng
- **Payload request**: `{"message": "Hello room!", "room_id": 456}` (room_id có thể lấy từ session)
- **Payload notification**: `{"user_id": 1, "username": "...", "message": "Hello room!", "timestamp": 1234567890}`
- **Service**: `friends_dispatch()` -> `handle_send_room_chat()`

### 8. Cải thiện kết bạn hiện có
- **CMD_REQ_ADD_FRIEND**: Thêm gửi notification `CMD_NOTIFY_FRIEND_REQ` cho người được mời
- **CMD_REQ_RESPOND_FRIEND**: 
  - Nếu chấp nhận: thông báo trạng thái cho cả 2 người
  - Tự động cập nhật friend status cho cả 2 bên

## Files đã tạo/sửa đổi

### Files mới:
1. **`server/include/service/friends_service.h`**
   - Header cho friends service
   - Declare `friends_dispatch()`, `friends_notify_status_change()`, `friends_get_user_status()`

2. **`server/src/service/friends_service.c`**
   - Implementation đầy đủ cho tất cả tính năng bạn bè
   - Xử lý search, get info, pending requests, invite, chat

### Files đã sửa đổi:

1. **`server/include/dao/dao_friends.h`**
   - Thêm: `dao_friends_get_pending_requests()`
   - Thêm: `dao_friends_get_info()`
   - Thêm: `dao_friends_are_friends()`

2. **`server/src/dao/dao_friends.c`**
   - Implement các hàm mới

3. **`server/include/dao/dao_users.h`**
   - Thêm: `dao_users_search_by_username()`

4. **`server/src/dao/dao_users.c`**
   - Implement `dao_users_search_by_username()`

5. **`server/include/service/commands.h`**
   - Thêm commands: `CMD_REQ_SEARCH_USER`, `CMD_RES_SEARCH_USER`
   - Thêm commands: `CMD_REQ_GET_FRIEND_INFO`, `CMD_RES_GET_FRIEND_INFO`
   - Thêm commands: `CMD_REQ_GET_PENDING_REQ`, `CMD_RES_GET_PENDING_REQ`
   - Thêm commands: `CMD_RES_INVITE_FRIEND`, `CMD_NOTIFY_ROOM_INVITE`
   - Cập nhật: `CMD_REQ_RESPOND_INVITE`, `CMD_RES_RESPOND_INVITE` (điều chỉnh số)

6. **`server/src/service/dispatcher.c`**
   - Cập nhật `CMD_REQ_ADD_FRIEND`: thêm gửi notification
   - Cập nhật `CMD_REQ_RESPOND_FRIEND`: thêm thông báo status khi accept
   - Route các command mới đến `friends_dispatch()`
   - Cập nhật `CMD_REQ_JOIN_ROOM`: thông báo status change
   - Cập nhật `CMD_REQ_LEAVE_ROOM`: thông báo status change
   - Thêm xử lý `CMD_REQ_INVITE_FRIEND`

7. **`server/src/service/auth_service.c`**
   - Cập nhật `CMD_REQ_LOGIN`: thông báo status "online" cho bạn bè khi login thành công

8. **`server/src/service/server.c`**
   - Cập nhật: thông báo status "offline" cho bạn bè khi disconnect

## Luồng hoạt động

### 1. Kết bạn
```
Client A → CMD_REQ_ADD_FRIEND {"friend_id": B}
Server → Client A: CMD_RES_ADD_FRIEND OK
Server → Client B: CMD_NOTIFY_FRIEND_REQ {"from_user_id": A, "from_username": "..."}

Client B → CMD_REQ_RESPOND_FRIEND {"from_user": A, "accept": true}
Server → Client B: CMD_RES_RESPOND_FRIEND OK
Server → Friends of A: CMD_NOTIFY_FRIEND_STATUS {"user_id": B, "status": "online", ...}
Server → Friends of B: CMD_NOTIFY_FRIEND_STATUS {"user_id": A, "status": "online", ...}
```

### 2. Theo dõi trạng thái
```
User A login → Server thông báo "online" cho tất cả bạn bè của A
User A join room → Server thông báo "in_game" với room_id cho tất cả bạn bè
User A leave room → Server thông báo "online" cho tất cả bạn bè
User A disconnect → Server thông báo "offline" cho tất cả bạn bè
```

### 3. Mời vào phòng
```
Client A (trong room 123) → CMD_REQ_INVITE_FRIEND {"friend_id": B}
Server kiểm tra: là bạn bè? online?
Server → Client A: CMD_RES_INVITE_FRIEND OK
Server → Client B: CMD_NOTIFY_ROOM_INVITE {"from_user_id": A, "from_username": "...", "room_id": 123}
```

### 4. Chat
```
DM:
Client A → CMD_REQ_SEND_DM {"to_user_id": B, "message": "Hi"}
Server → Client A: CMD_RES_SEND_DM OK
Server → Client B: CMD_NOTIFY_DM {"from_user_id": A, "from_username": "...", "message": "Hi", ...}

Room Chat:
Client A (trong room 123) → CMD_REQ_SEND_ROOM_CHAT {"message": "Hello"}
Server → Client A: CMD_RES_SEND_ROOM_CHAT OK
Server → All members in room 123: CMD_NOTIFY_ROOM_CHAT {"user_id": A, "username": "...", "message": "Hello", ...}
```

## Lưu ý kỹ thuật

1. **Session Management**: Sử dụng `session_manager` để track online users và room memberships
2. **Broadcast**: Sử dụng `session_manager_broadcast_to_room()` và `session_manager_send_to_user()` để gửi notifications
3. **JSON Parsing**: Sử dụng các hàm `util_json_*` để parse và build JSON
4. **Status Tracking**: Status được track real-time dựa trên session state (online/offline) và room_id (in_game)
5. **Friend Relationship**: Kiểm tra quan hệ bạn bè qua `dao_friends_are_friends()` trước khi cho phép một số hành động

## Các tính năng có thể mở rộng

1. **Offline Messages**: Lưu DM vào DB khi người nhận offline, fetch khi login
2. **Message History**: Lưu lịch sử chat (DM và room chat) vào DB
3. **Read Receipts**: Đánh dấu đã đọc cho tin nhắn
4. **Typing Indicators**: Thông báo khi đang gõ
5. **Friend Groups**: Nhóm bạn bè
6. **Block User**: Chặn người dùng (đã có trong DB nhưng chưa implement logic)

