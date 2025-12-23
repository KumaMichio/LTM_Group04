# Hướng Dẫn Test Chế Độ 1vN Trên Cùng 1 Máy

## Yêu Cầu
- Server đang chạy trên WSL (localhost:9000)
- Qt6 client đã được build thành công
- Có ít nhất 2 user accounts trong database

## Cách 1: Sử dụng Script PowerShell (Đơn giản nhất)

### Bước 1: Chạy script
```powershell
cd c:\Users\admin\Desktop\LTM_HW\Project102\LTM_Group04\client\qt6
.\test_1vn_2clients.ps1
```

### Bước 2: Đăng nhập
- **Client 1**: Đăng nhập với `testuser1` / `testpass1`
- **Client 2**: Đăng nhập với `testuser2` / `testpass2`

### Bước 3: Tạo và Join Room
1. Trên **Client 1**: 
   - Chọn chế độ "1vN Mode"
   - Tạo room (Create Room)
   - Chọn số lượng câu hỏi (Easy/Medium/Hard)
   - Ghi nhớ **Room ID** được hiển thị

2. Trên **Client 2**:
   - Chọn chế độ "1vN Mode" 
   - Join room (nhập Room ID từ Client 1)

### Bước 4: Start Game
- Khi cả 2 players đã ở trong room
- **Client 1** (host) bấm nút "Start Game"
- Game sẽ bắt đầu!

---

## Cách 2: Chạy Thủ Công

### Bước 1: Mở Terminal thứ nhất
```powershell
cd c:\Users\admin\Desktop\LTM_HW\Project102\LTM_Group04\client\qt6\build\Desktop_Qt_6_10_1_MinGW_64_bit-Debug
.\MillionaireClient.exe
```

### Bước 2: Mở Terminal thứ hai (terminal mới)
```powershell
cd c:\Users\admin\Desktop\LTM_HW\Project102\LTM_Group04\client\qt6\build\Desktop_Qt_6_10_1_MinGW_64_bit-Debug
.\MillionaireClient.exe
```

### Bước 3-4: Giống như Cách 1

---

## Cách 3: Từ Qt Creator (Cho Developer)

### Bước 1: Chạy instance đầu tiên
- Nhấn **Ctrl+R** hoặc click nút Run (▶)
- Đợi app mở ra

### Bước 2: Chạy instance thứ hai
**Trong khi instance đầu tiên vẫn đang chạy:**
- Nhấn **Ctrl+R** lần nữa
- Qt Creator sẽ hỏi có muốn stop app đang chạy không
- **Chọn "No" hoặc "Keep Running"**
- Instance thứ hai sẽ được launch

### Bước 3-4: Giống như Cách 1

---

## Test Accounts Có Sẵn

Trong database đã có các test users (xem file `server/test_users_friends.sql`):

| Username | Password |
|----------|----------|
| testuser1 | testpass1 |
| testuser2 | testpass2 |
| testuser3 | testpass3 |
| alice | alice123 |
| bob | bob123 |
| charlie | charlie123 |

---

## Test với nhiều hơn 2 người

Để test với 3-4 người chơi:

1. Chạy thêm instances (lặp lại Cách 1 hoặc 2)
2. Đăng nhập với các accounts khác (`testuser3`, `alice`, `bob`, v.v.)
3. Tất cả join vào cùng 1 Room ID
4. Host (người tạo room) start game

---

## Troubleshooting

### Lỗi: "Không kết nối được đến server"
- Kiểm tra server có đang chạy trên WSL không:
  ```bash
  # Trên WSL
  cd /mnt/c/Users/admin/Desktop/LTM_HW/Project102/LTM_Group04/server
  ./build/server
  ```
- Kiểm tra port 9000 có bị block không

### Lỗi: "Login failed"
- Kiểm tra users đã được tạo trong database:
  ```bash
  # Trên WSL
  psql -U postgres -d ltm_group04 -f server/test_users_friends.sql
  ```

### Lỗi: "Cannot join room"
- Đảm bảo Room ID nhập đúng
- Kiểm tra room chưa bị xóa
- Game có thể đã bắt đầu (không join được khi game đang chạy)

### Muốn test lại từ đầu
- Đóng tất cả client windows
- Restart server trên WSL
- Chạy lại script

---

## Lưu Ý Quan Trọng

1. **Server phải được start trước** khi chạy clients
2. **Mỗi client phải login với username khác nhau**
3. **Cùng 1 user không thể login 2 lần** (server sẽ từ chối)
4. **Host (người tạo room) mới có quyền start game**
5. Nếu 1 player disconnect, game có thể bị ảnh hưởng

---

## Workflow Hoàn Chỉnh

```
1. [WSL] Start Server
   └─> ./build/server

2. [Windows] Run Client 1
   └─> Login: testuser1/testpass1
   └─> Select: 1vN Mode
   └─> Create Room (easy:3, medium:2, hard:1)
   └─> Nhận Room ID: 12345

3. [Windows] Run Client 2  
   └─> Login: testuser2/testpass2
   └─> Select: 1vN Mode
   └─> Join Room: 12345

4. [Client 1] Start Game (khi thấy 2 players trong room)

5. [Cả 2 clients] Answer questions
   └─> Người trả lời đúng + nhanh hơn = điểm cao hơn
   └─> Trả lời sai = bị loại
   └─> Người còn lại cuối cùng = Winner!
```

Good luck testing! 🎮
