# 🌐 Hướng Dẫn Setup và Test Project với 4 Máy Tính Vật Lý

## 📋 Tổng Quan

Hướng dẫn này sẽ giúp bạn setup project để chạy với:
- **Máy 1 (Server)**: Chạy server và PostgreSQL
- **Máy 2 (Room Owner)**: Chạy Qt client, đóng vai trò owner tạo phòng
- **Máy 3 (Player 1)**: Chạy Qt client, tham gia phòng
- **Máy 4 (Player 2)**: Chạy Qt client, tham gia phòng
- **Máy 5 (Player 3)**: Chạy Qt client, tham gia phòng (optional - có thể dùng 1 trong các máy trên)

---

## 🔧 PHẦN 1: SETUP MÁY SERVER (Máy 1)

### Bước 1: Cài đặt PostgreSQL

#### Windows:
1. Tải PostgreSQL từ [postgresql.org](https://www.postgresql.org/download/windows/)
2. Cài đặt với password cho user `postgres` (ví dụ: `1`)
3. Ghi nhớ port mặc định (thường là `5432`)

#### Linux/WSL:
```bash
sudo apt-get update
sudo apt-get install postgresql postgresql-contrib
sudo systemctl start postgresql
sudo systemctl enable postgresql
```

### Bước 2: Tạo Database

```bash
# Windows (PowerShell)
psql -U postgres -c "CREATE DATABASE ltm_group04;"
psql -U postgres -d ltm_group04 -f server/db.sql

# Linux/WSL
sudo -u postgres psql -c "CREATE DATABASE ltm_group04;"
sudo -u postgres psql -d ltm_group04 -f server/db.sql
```

### Bước 3: Build Server

#### Windows:
```powershell
cd server
mkdir build
cd build
cmake ..
cmake --build .
```

#### Linux/WSL:
```bash
cd server
mkdir -p build
cd build
cmake ..
make
```

### Bước 4: Lấy IP Address của Máy Server

#### Windows:
```powershell
# PowerShell
ipconfig
# Tìm "IPv4 Address" trong phần adapter đang dùng (Wi-Fi hoặc Ethernet)
# Ví dụ: 192.168.1.100
```

#### Linux/WSL:
```bash
# Linux
hostname -I
# hoặc
ip addr show | grep "inet " | grep -v 127.0.0.1

# WSL
hostname -I
# Hoặc từ Windows PowerShell:
wsl hostname -I
```

**Ghi nhớ IP này!** (Ví dụ: `192.168.1.100`)

### Bước 5: Cấu hình Firewall

#### Windows:
```powershell
# Mở port 9000 cho TCP
New-NetFirewallRule -DisplayName "Millionaire Server" -Direction Inbound -LocalPort 9000 -Protocol TCP -Action Allow

# Hoặc qua GUI:
# Control Panel → Windows Defender Firewall → Advanced Settings
# → Inbound Rules → New Rule → Port → TCP → 9000 → Allow
```

#### Linux:
```bash
# Ubuntu/Debian
sudo ufw allow 9000/tcp
sudo ufw reload

# Hoặc iptables
sudo iptables -A INPUT -p tcp --dport 9000 -j ACCEPT
```

### Bước 6: Chạy Server

#### Windows (PowerShell):
```powershell
cd server
$env:DB_CONN = "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
$env:SERVER_MODE = "1"
$env:SERVER_PORT = "9000"
.\build\server.exe
```

#### Linux/WSL:
```bash
cd server
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
export SERVER_MODE="1"
export SERVER_PORT="9000"
./build/server
```

**Kiểm tra server đang chạy:**
```bash
# Windows
netstat -an | findstr :9000

# Linux
netstat -tuln | grep 9000
# hoặc
ss -tuln | grep 9000
```

Bạn sẽ thấy output: `=== SERVER STARTED ===`

---

## 💻 PHẦN 2: SETUP MÁY CLIENT (Máy 2, 3, 4)

**Lặp lại các bước sau cho tất cả các máy client (Máy 2, 3, 4)**

### Bước 1: Cài đặt Qt6

#### Windows:
- Tải và cài đặt từ [qt.io/download](https://www.qt.io/download)
- Chọn Qt 6.x và MinGW hoặc MSVC

#### Linux:
```bash
sudo apt-get install qt6-base-dev qt6-base-dev-tools cmake build-essential
```

### Bước 2: Build Client

#### Windows:
```powershell
cd client\qt6
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

#### Linux:
```bash
cd client/qt6
mkdir -p build
cd build
cmake ..
make
```

### Bước 3: Chạy Client

#### Windows:
```powershell
cd client\qt6\build
.\MillionaireClient.exe
```

#### Linux:
```bash
cd client/qt6/build
./MillionaireClient
```

### Bước 4: Kết nối đến Server

1. Mở ứng dụng client
2. Trong phần **"Cài đặt Server"**:
   - **Host**: Nhập IP của máy server (ví dụ: `192.168.1.100`)
   - **Port**: `9000`
3. Click **"Kết nối"**
4. Kiểm tra trạng thái: "Trạng thái: Đã kết nối" (màu xanh)

---

## 🧪 PHẦN 3: KỊCH BẢN TEST

### Test Case 1: Kết nối cơ bản (Tất cả clients)

**Mục tiêu**: Kiểm tra tất cả clients có thể kết nối đến server

**Bước thực hiện**:

**Trên Máy 1 (Server)**:
1. ✅ Server đang chạy và lắng nghe trên port 9000

**Trên Máy 2, 3, 4 (Tất cả clients)**:
1. ✅ Mở ứng dụng client
2. ✅ Nhập IP của Máy 1 (ví dụ: `192.168.1.100`)
3. ✅ Nhập Port: `9000`
4. ✅ Click "Kết nối"
5. ✅ Kiểm tra: Trạng thái hiển thị "Đã kết nối" (màu xanh)

**Kết quả mong đợi**: Tất cả 3 clients kết nối thành công đến server

---

### Test Case 2: Đăng ký và Đăng nhập (Tất cả players)

**Mục tiêu**: Test authentication flow cho tất cả players

**Bước thực hiện**:

**Trên Máy 2 (Room Owner - player_owner)**:
1. ✅ Kết nối đến server
2. ✅ Tab "Đăng ký":
   - Username: `player_owner`
   - Password: `pass123`
   - Confirm Password: `pass123`
   - Click "Đăng ký"
3. ✅ Kiểm tra: "Đăng ký thành công!"
4. ✅ Tab "Đăng nhập":
   - Username: `player_owner`
   - Password: `pass123`
   - Click "Đăng nhập"
5. ✅ Kiểm tra: Mở cửa sổ "Chọn chế độ chơi"

**Trên Máy 3 (Player 1 - player1)**:
1. ✅ Kết nối đến server
2. ✅ Đăng ký: `player1` / `pass123`
3. ✅ Đăng nhập thành công

**Trên Máy 4 (Player 2 - player2)**:
1. ✅ Kết nối đến server
2. ✅ Đăng ký: `player2` / `pass123`
3. ✅ Đăng nhập thành công

**Trên Máy 5 (Player 3 - player3 - nếu có)**:
1. ✅ Kết nối đến server
2. ✅ Đăng ký: `player3` / `pass123`
3. ✅ Đăng nhập thành công

**Kết quả mong đợi**: Tất cả 3-4 users đăng ký và đăng nhập thành công

---

### Test Case 3: Tìm kiếm và Kết bạn (Network friends)

**Mục tiêu**: Test friends system giữa các players

**Bước thực hiện**:

**Trên Máy 2 (player_owner)**:
1. ✅ Đăng nhập với `player_owner`
2. ✅ Tìm kiếm user: `player1`
3. ✅ Gửi lời mời kết bạn đến `player1`
4. ✅ Tìm kiếm user: `player2`
5. ✅ Gửi lời mời kết bạn đến `player2`

**Trên Máy 3 (player1)**:
1. ✅ Đăng nhập với `player1`
2. ✅ Nhận notification: "player_owner đã gửi lời mời kết bạn"
3. ✅ Chấp nhận lời mời
4. ✅ Gửi lời mời kết bạn đến `player2`

**Trên Máy 4 (player2)**:
1. ✅ Đăng nhập với `player2`
2. ✅ Nhận notification: "player_owner đã gửi lời mời kết bạn"
3. ✅ Chấp nhận lời mời
4. ✅ Nhận notification: "player1 đã gửi lời mời kết bạn"
5. ✅ Chấp nhận lời mời

**Kết quả mong đợi**: 
- Tất cả players nhận được notifications
- Tất cả kết bạn thành công
- Mọi người thấy trạng thái online của nhau trong friend list

---

### Test Case 4: Chat Direct Message (Multi-user)

**Mục tiêu**: Test chat 1-1 giữa nhiều users

**Bước thực hiện**:

**Trên Máy 2 (player_owner)**:
1. ✅ Gửi DM đến `player1`: "Hello player1, ready to play?"

**Trên Máy 3 (player1)**:
1. ✅ Nhận tin nhắn: "Hello player1, ready to play?"
2. ✅ Gửi reply đến `player_owner`: "Yes, I'm ready!"
3. ✅ Gửi DM đến `player2`: "Hey player2, join us!"

**Trên Máy 4 (player2)**:
1. ✅ Nhận tin nhắn: "Hey player2, join us!"
2. ✅ Gửi reply đến `player1`: "On my way!"

**Kết quả mong đợi**: 
- Tất cả tin nhắn được gửi và nhận real-time
- Mỗi client chỉ nhận tin nhắn gửi đến mình
- Không có tin nhắn bị lộn hoặc gửi nhầm

---

### Test Case 5: Tạo phòng và Join phòng (Multi-player)

**Mục tiêu**: Test room management với nhiều players

**Bước thực hiện**:

**Trên Máy 2 (player_owner - Owner)**:
1. ✅ Tạo phòng với config:
   - Easy: 10 câu
   - Medium: 10 câu
   - Hard: 10 câu
   - Tổng: 30 câu
2. ✅ Kiểm tra: Nhận `room_id` từ server (ví dụ: `room_id = 123`)
3. ✅ Ghi nhớ `room_id` để chia sẻ với các players khác

**Trên Máy 3 (player1)**:
1. ✅ Join phòng với `room_id = 123`
2. ✅ Kiểm tra: Nhận notification "Đã tham gia phòng"
3. ✅ Kiểm tra: Thấy `player_owner` trong danh sách members

**Trên Máy 4 (player2)**:
1. ✅ Join phòng với `room_id = 123`
2. ✅ Kiểm tra: Nhận notification "Đã tham gia phòng"
3. ✅ Kiểm tra: Thấy `player_owner` và `player1` trong danh sách members

**Trên Máy 5 (player3 - nếu có)**:
1. ✅ Join phòng với `room_id = 123`
2. ✅ Kiểm tra: Thấy tất cả members trong phòng

**Trên Máy 2 (player_owner)**:
1. ✅ Kiểm tra: Thấy tất cả players đã join (player1, player2, player3)
2. ✅ Kiểm tra: Số lượng members = 3 hoặc 4 (tùy có player3)

**Kết quả mong đợi**: 
- Phòng được tạo thành công với config đúng
- Tất cả players join được phòng
- Tất cả players nhận notification về room update
- Mọi người thấy đầy đủ danh sách members

---

### Test Case 6: Mời bạn vào phòng (Multi-invite)

**Mục tiêu**: Test invite friend feature với nhiều players

**Bước thực hiện**:

**Trên Máy 2 (player_owner - trong phòng)**:
1. ✅ Mời `player1` vào phòng (nếu chưa join)
2. ✅ Mời `player2` vào phòng
3. ✅ Mời `player3` vào phòng (nếu có)

**Trên Máy 3 (player1)**:
1. ✅ Nhận notification: "player_owner mời bạn vào phòng [room_id]"
2. ✅ Chấp nhận lời mời (nếu chưa join)
3. ✅ Kiểm tra: Tự động join phòng hoặc đã trong phòng

**Trên Máy 4 (player2)**:
1. ✅ Nhận notification: "player_owner mời bạn vào phòng [room_id]"
2. ✅ Chấp nhận lời mời
3. ✅ Kiểm tra: Tự động join phòng

**Trên Máy 5 (player3 - nếu có)**:
1. ✅ Nhận notification: "player_owner mời bạn vào phòng [room_id]"
2. ✅ Chấp nhận lời mời
3. ✅ Kiểm tra: Tự động join phòng

**Kết quả mong đợi**: 
- Tất cả players nhận được invite
- Tất cả join phòng thành công sau khi accept
- Owner thấy tất cả members đã join

---

### Test Case 7: Chat trong phòng (Multi-player broadcast)

**Mục tiêu**: Test room chat broadcast với nhiều players

**Bước thực hiện**:

**Trên Máy 2 (player_owner - trong phòng)**:
1. ✅ Gửi tin nhắn: "Everyone ready? We'll start soon!"

**Trên Máy 3 (player1 - trong phòng)**:
1. ✅ Nhận tin nhắn: "Everyone ready? We'll start soon!"
2. ✅ Gửi reply: "Yes, I'm ready!"
3. ✅ Kiểm tra: Cả owner và player2, player3 thấy tin nhắn

**Trên Máy 4 (player2 - trong phòng)**:
1. ✅ Nhận cả 2 tin nhắn (từ owner và player1)
2. ✅ Gửi reply: "Ready too!"

**Trên Máy 5 (player3 - trong phòng, nếu có)**:
1. ✅ Nhận tất cả tin nhắn từ mọi người
2. ✅ Gửi reply: "Let's go!"

**Trên Máy 2 (player_owner)**:
1. ✅ Nhận tất cả tin nhắn từ players
2. ✅ Kiểm tra: Tất cả tin nhắn được broadcast đúng

**Kết quả mong đợi**: 
- Tất cả players trong phòng thấy tất cả tin nhắn
- Broadcast hoạt động đúng cho tất cả members
- Không có tin nhắn bị mất hoặc gửi nhầm

---

### Test Case 8: Game 1vN - Bắt đầu game (Multi-player)

**Mục tiêu**: Test multiplayer game start với nhiều players

**Bước thực hiện**:

**Trên Máy 2 (player_owner - Owner)**:
1. ✅ Kiểm tra: Phòng có ít nhất 2 members (tối đa 8)
2. ✅ Click "Bắt đầu game"
3. ✅ Kiểm tra: Nhận notification "Game started"
4. ✅ Kiểm tra: Nhận câu hỏi đầu tiên với timer 15 giây

**Trên Máy 3 (player1)**:
1. ✅ Nhận notification "Game started"
2. ✅ Nhận câu hỏi đầu tiên (giống hệt owner và các players khác)
3. ✅ Kiểm tra: Timer 15 giây bắt đầu đếm ngược

**Trên Máy 4 (player2)**:
1. ✅ Nhận notification "Game started"
2. ✅ Nhận câu hỏi đầu tiên (giống hệt mọi người)
3. ✅ Kiểm tra: Timer 15 giây bắt đầu đếm ngược

**Trên Máy 5 (player3 - nếu có)**:
1. ✅ Nhận notification "Game started"
2. ✅ Nhận câu hỏi đầu tiên
3. ✅ Kiểm tra: Timer đồng bộ với mọi người

**Kết quả mong đợi**: 
- Game bắt đầu thành công
- Tất cả players nhận cùng 1 câu hỏi tại cùng thời điểm
- Timer 15 giây bắt đầu đếm ngược đồng bộ cho tất cả
- Không có player nào bị bỏ sót

---

### Test Case 9: Game 1vN - Trả lời câu hỏi (Multi-player scoring)

**Mục tiêu**: Test game logic và scoring với nhiều players

**Bước thực hiện**:

**Round 1 - Câu hỏi Dễ:**

**Trên Máy 2 (player_owner)**:
1. ✅ Trả lời câu hỏi đúng trong 12 giây
2. ✅ Kiểm tra: Nhận điểm (base 1000 * time_percent + bonus)
3. ✅ Kiểm tra: Không bị loại

**Trên Máy 3 (player1)**:
1. ✅ Trả lời câu hỏi đúng trong 8 giây
2. ✅ Kiểm tra: Nhận điểm cao hơn owner (vì trả lời nhanh hơn)
3. ✅ Kiểm tra: Không bị loại

**Trên Máy 4 (player2)**:
1. ✅ Trả lời câu hỏi đúng trong 5 giây
2. ✅ Kiểm tra: Nhận điểm cao nhất (trả lời nhanh nhất)
3. ✅ Kiểm tra: Không bị loại

**Trên Máy 5 (player3 - nếu có)**:
1. ✅ Trả lời câu hỏi đúng trong 3 giây
2. ✅ Kiểm tra: Nhận điểm cao nhất
3. ✅ Kiểm tra: Không bị loại

**Tất cả players:**
1. ✅ Nhận notification về kết quả round
2. ✅ Thấy leaderboard tạm thời (player2/player3 dẫn đầu)
3. ✅ Chờ câu hỏi tiếp theo

**Kết quả mong đợi**: 
- Tất cả players trả lời đúng
- Điểm số phản ánh đúng thời gian trả lời (nhanh hơn = điểm cao hơn)
- Leaderboard hiển thị đúng thứ tự
- Tất cả nhận được kết quả round

---

### Test Case 10: Game 1vN - Loại bỏ và Kết thúc (Multi-player elimination)

**Mục tiêu**: Test elimination và game end với nhiều players

**Bước thực hiện**:

**Round 2 - Câu hỏi Vừa:**

**Trên Máy 2 (player_owner)**:
1. ✅ Trả lời sai câu hỏi
2. ✅ Kiểm tra: Bị loại, nhận notification "Eliminated"
3. ✅ Kiểm tra: Không nhận câu hỏi tiếp theo

**Trên Máy 3 (player1)**:
1. ✅ Nhận notification: "player_owner đã bị loại"
2. ✅ Trả lời đúng câu hỏi trong 10 giây
3. ✅ Kiểm tra: Vẫn trong game, nhận điểm

**Trên Máy 4 (player2)**:
1. ✅ Nhận notification: "player_owner đã bị loại"
2. ✅ Trả lời đúng câu hỏi trong 7 giây
3. ✅ Kiểm tra: Vẫn trong game, điểm tăng

**Trên Máy 5 (player3 - nếu có)**:
1. ✅ Nhận notification: "player_owner đã bị loại"
2. ✅ Trả lời đúng câu hỏi trong 4 giây
3. ✅ Kiểm tra: Vẫn trong game, điểm cao nhất

**Round 3 - Câu hỏi Khó:**

**Trên Máy 3 (player1)**:
1. ✅ Trả lời sai câu hỏi
2. ✅ Kiểm tra: Bị loại

**Trên Máy 4 (player2)**:
1. ✅ Nhận notification: "player1 đã bị loại"
2. ✅ Trả lời đúng câu hỏi
3. ✅ Kiểm tra: Vẫn trong game

**Trên Máy 5 (player3 - nếu có)**:
1. ✅ Nhận notification: "player1 đã bị loại"
2. ✅ Trả lời đúng câu hỏi
3. ✅ Kiểm tra: Vẫn trong game

**Kết thúc game (khi hết câu hỏi hoặc chỉ còn 1 người):**

**Tất cả players (kể cả đã bị loại):**
1. ✅ Nhận notification "Game Over"
2. ✅ Nhận leaderboard cuối cùng:
   - Top 1: player3 hoặc player2 (tùy điểm số)
   - Top 2: player còn lại
   - Top 3: player1 (đã bị loại ở round 3)
   - Top 4: player_owner (đã bị loại ở round 2)
3. ✅ Kiểm tra: Stats được cập nhật (games played, wins, scores)

**Kết quả mong đợi**: 
- Players bị loại đúng khi trả lời sai
- Tất cả players nhận notification về elimination
- Game kết thúc đúng điều kiện (hết câu hỏi hoặc chỉ còn 1 người)
- Leaderboard hiển thị đúng thứ tự theo điểm số
- Stats được cập nhật cho tất cả players

---

### Test Case 11: Friend Status Updates (Multi-player tracking)

**Mục tiêu**: Test real-time status tracking với nhiều players

**Bước thực hiện**:

**Khi join phòng:**

**Trên Máy 2 (player_owner)**:
1. ✅ Join phòng
2. ✅ Kiểm tra: Friend list hiển thị:
   - player1 status = "in_game"
   - player2 status = "in_game"
   - player3 status = "in_game" (nếu có)

**Trên Máy 3 (player1)**:
1. ✅ Join phòng
2. ✅ Kiểm tra: Friend list hiển thị:
   - player_owner status = "in_game"
   - player2 status = "in_game"
   - player3 status = "in_game" (nếu có)

**Trên Máy 4 (player2)**:
1. ✅ Join phòng
2. ✅ Kiểm tra: Friend list hiển thị tất cả friends status = "in_game"

**Khi game bắt đầu:**

**Tất cả players:**
1. ✅ Kiểm tra: Friend list vẫn hiển thị status = "in_game" cho tất cả friends trong phòng

**Khi game kết thúc:**

**Trên Máy 2 (player_owner - bị loại sớm)**:
1. ✅ Leave phòng sau khi bị loại
2. ✅ Kiểm tra: Status chuyển về "online"
3. ✅ Kiểm tra: Friends thấy status thay đổi

**Trên Máy 3 (player1 - bị loại)**:
1. ✅ Leave phòng
2. ✅ Kiểm tra: Status chuyển về "online"

**Trên Máy 4 (player2 - còn lại)**:
1. ✅ Kiểm tra: Friend list hiển thị:
   - player_owner status = "online"
   - player1 status = "online"
   - player3 status = "in_game" (nếu vẫn trong game)

**Kết quả mong đợi**: 
- Status updates real-time cho tất cả friends
- Mọi người thấy status thay đổi của nhau
- Status chính xác (online/in_game/offline)

---

## 🔍 TROUBLESHOOTING

### Vấn đề 1: Client không kết nối được đến server

**Nguyên nhân có thể**:
- Firewall chặn port 9000
- IP address sai
- Server chưa chạy

**Giải pháp**:
```bash
# Kiểm tra server có đang chạy
# Trên Máy 1:
netstat -an | findstr :9000  # Windows
netstat -tuln | grep 9000    # Linux

# Kiểm tra firewall
# Windows: Control Panel → Firewall → Advanced Settings
# Linux: sudo ufw status

# Test kết nối từ Máy 2:
# Windows PowerShell:
Test-NetConnection -ComputerName 192.168.1.100 -Port 9000

# Linux:
telnet 192.168.1.100 9000
# hoặc
nc -zv 192.168.1.100 9000
```

### Vấn đề 2: Database connection failed

**Nguyên nhân có thể**:
- PostgreSQL chưa chạy
- DB_CONN sai format
- Database chưa được tạo

**Giải pháp**:
```bash
# Kiểm tra PostgreSQL đang chạy
# Windows:
Get-Service postgresql*

# Linux:
sudo systemctl status postgresql

# Kiểm tra database tồn tại
psql -U postgres -l | grep ltm_group04
```

### Vấn đề 3: Client build failed

**Nguyên nhân có thể**:
- Qt6 chưa cài đặt
- CMake version cũ
- Thiếu dependencies

**Giải pháp**:
```bash
# Kiểm tra Qt6
qmake6 --version

# Kiểm tra CMake
cmake --version  # Cần >= 3.16

# Rebuild từ đầu
rm -rf build
mkdir build
cd build
cmake ..
make
```

### Vấn đề 4: Không ping được giữa 2 máy

**Nguyên nhân có thể**:
- Không cùng mạng LAN
- Firewall chặn ICMP
- IP address sai

**Giải pháp**:
```bash
# Kiểm tra cùng mạng:
# Máy 1 IP: 192.168.1.100
# Máy 2 IP: 192.168.1.101 (phải cùng subnet 192.168.1.x)

# Test ping:
# Từ Máy 2:
ping 192.168.1.100

# Nếu ping không được, kiểm tra:
# - Cả 2 đều kết nối cùng router/switch
# - Firewall không chặn ICMP (có thể bỏ qua, TCP vẫn hoạt động)
```

---

## 📝 CHECKLIST TRƯỚC KHI TEST

### Máy Server (Máy 1):
- [ ] PostgreSQL đã cài và chạy
- [ ] Database `ltm_group04` đã được tạo
- [ ] Server đã được build
- [ ] IP address đã được ghi nhớ (ví dụ: `192.168.1.100`)
- [ ] Firewall đã mở port 9000
- [ ] Server đang chạy và lắng nghe trên port 9000
- [ ] Có thể ping từ các máy client đến Máy 1 (optional)

### Máy Client - Room Owner (Máy 2):
- [ ] Qt6 đã cài đặt
- [ ] Client đã được build
- [ ] Có thể ping đến IP của Máy 1 (optional)
- [ ] Có thể kết nối đến port 9000 của Máy 1 (test bằng telnet/nc)
- [ ] Đã ghi nhớ IP server để nhập vào client

### Máy Client - Player 1 (Máy 3):
- [ ] Qt6 đã cài đặt
- [ ] Client đã được build
- [ ] Có thể kết nối đến port 9000 của Máy 1
- [ ] Đã ghi nhớ IP server

### Máy Client - Player 2 (Máy 4):
- [ ] Qt6 đã cài đặt
- [ ] Client đã được build
- [ ] Có thể kết nối đến port 9000 của Máy 1
- [ ] Đã ghi nhớ IP server

### Máy Client - Player 3 (Máy 5 - nếu có):
- [ ] Qt6 đã cài đặt
- [ ] Client đã được build
- [ ] Có thể kết nối đến port 9000 của Máy 1
- [ ] Đã ghi nhớ IP server

---

## 🎯 KẾT QUẢ MONG ĐỢI

Sau khi hoàn thành tất cả test cases, bạn sẽ có:

1. ✅ 3-4 clients có thể kết nối đến cùng 1 server
2. ✅ Authentication hoạt động cho tất cả players (register, login)
3. ✅ Friends system hoạt động giữa nhiều players (search, add, status)
4. ✅ Chat hoạt động (DM giữa nhiều users, room chat broadcast)
5. ✅ Room management hoạt động với nhiều players (create, join, invite)
6. ✅ Multiplayer game (1vN) hoạt động đầy đủ với 3-4 players
7. ✅ Real-time updates hoạt động (status, notifications, eliminations)
8. ✅ Scoring system hoạt động đúng với nhiều players
9. ✅ Leaderboard hiển thị đúng thứ tự
10. ✅ Elimination logic hoạt động đúng

---

## 📊 BẢNG TÓM TẮT TEST CASES

| Test Case | Tính năng | Số Players | Trạng thái | Ghi chú |
|-----------|-----------|------------|------------|---------|
| TC1 | Kết nối cơ bản | 3-4 | ✅ | Kiểm tra network connectivity cho tất cả |
| TC2 | Đăng ký/Đăng nhập | 3-4 | ✅ | Authentication flow cho nhiều users |
| TC3 | Tìm kiếm/Kết bạn | 3-4 | ✅ | Friends system network |
| TC4 | Chat DM | 3-4 | ✅ | Direct messaging giữa nhiều users |
| TC5 | Tạo/Join phòng | 3-4 | ✅ | Room management với nhiều players |
| TC6 | Mời bạn vào phòng | 3-4 | ✅ | Invite feature multi-player |
| TC7 | Chat trong phòng | 3-4 | ✅ | Room broadcast cho tất cả members |
| TC8 | Bắt đầu game 1vN | 3-4 | ✅ | Game initialization đồng bộ |
| TC9 | Trả lời câu hỏi | 3-4 | ✅ | Game logic & scoring với nhiều players |
| TC10 | Loại bỏ/Kết thúc | 3-4 | ✅ | Elimination & leaderboard multi-player |
| TC11 | Status updates | 3-4 | ✅ | Real-time tracking cho tất cả friends |

---

## 🔐 LƯU Ý BẢO MẬT

Khi test trên mạng LAN:
- ✅ An toàn: Chỉ trong mạng nội bộ
- ⚠️ Cảnh báo: Không expose server ra internet mà không có firewall/proxy
- 💡 Gợi ý: Dùng VPN nếu cần test từ xa

---

## 📞 HỖ TRỢ

Nếu gặp vấn đề:
1. Kiểm tra lại checklist
2. Xem phần Troubleshooting
3. Kiểm tra logs của server và client
4. Đảm bảo cả 2 máy trong cùng mạng LAN

---

## 🎮 SƠ ĐỒ KIẾN TRÚC TEST

```
                    ┌─────────────┐
                    │   Máy 1     │
                    │   Server    │
                    │ PostgreSQL  │
                    │  Port 9000  │
                    └──────┬──────┘
                           │
            ┌──────────────┼──────────────┐
            │              │              │
      ┌─────▼─────┐  ┌─────▼─────┐  ┌─────▼─────┐
      │   Máy 2   │  │   Máy 3   │  │   Máy 4   │
      │  Owner    │  │  Player 1 │  │  Player 2 │
      │ Qt Client │  │ Qt Client │  │ Qt Client │
      └───────────┘  └───────────┘  └───────────┘
      
      (Optional: Máy 5 - Player 3)
```

**Luồng hoạt động:**
1. Máy 1 chạy server và database
2. Máy 2, 3, 4 kết nối đến server
3. Máy 2 (Owner) tạo phòng
4. Máy 3, 4 join phòng
5. Máy 2 bắt đầu game
6. Tất cả players chơi game 1vN cùng lúc

---

**Lưu ý quan trọng**: 
- Đảm bảo tất cả 4 máy tính đều trong cùng một mạng LAN (cùng router/switch) để có thể kết nối với nhau
- Nếu dùng WiFi, tất cả phải kết nối cùng một WiFi network
- Đảm bảo firewall trên Máy 1 (Server) đã mở port 9000 cho inbound connections

