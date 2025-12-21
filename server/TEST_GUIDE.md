# 🧪 HƯỚNG DẪN CHẠY TEST

## 📋 TỔNG QUAN

Project có **8 test files** để kiểm tra các tính năng khác nhau:

| Test File | Mô tả | Loại Test |
|-----------|-------|-----------|
| `test_db.c` | Test kết nối database | Unit Test |
| `test_auth.c` | Test đăng ký/đăng nhập | Unit Test |
| `test_rooms.c` | Test tạo/join phòng | Unit Test |
| `test_friends.c` | Test gửi/chấp nhận lời mời kết bạn | Unit Test |
| `test_chat.c` | Test gửi/nhận tin nhắn | Unit Test |
| `test_quickmode.c` | Test chế độ chơi nhanh (15 câu) | Integration Test |
| `test_onevn.c` | Test multiplayer 1vN với menu tương tác | Integration Test |
| `test_onevn_interactive.c` | Test multiplayer 1vN cho phép nhập đáp án | Interactive Test |

---

## 🔧 YÊU CẦU TRƯỚC KHI CHẠY TEST

### 1. **PostgreSQL Database**
- PostgreSQL đã được cài đặt và đang chạy
- Database `ltm_group04` đã được tạo
- Đã chạy `db.sql` để tạo schema và sample data

### 2. **Environment Variable**
Cần set biến môi trường `DB_CONN` để kết nối database:

**Format:**
```
host=HOST port=PORT dbname=DATABASE user=USERNAME password=PASSWORD
```

**Ví dụ:**
```
host=localhost port=5432 dbname=ltm_group04 user=postgres password=1
```

### 3. **Build Tools**
- `gcc` compiler
- `make` tool
- PostgreSQL development libraries (`libpq-dev`)

---

## 🚀 CÁCH BUILD VÀ CHẠY TEST

### **Bước 1: Setup Database**

```bash
# Tạo database và schema
psql -U postgres -f server/db.sql
```

File `db.sql` sẽ:
- Tạo database `ltm_group04`
- Tạo tất cả tables (users, rooms, questions, ...)
- Insert 12 test users (Owner, Player2-8, Alice, Bob, Charlie, Diana)
- Insert 30 test questions (10 EASY, 10 MEDIUM, 10 HARD)

### **Bước 2: Set Environment Variable**

#### **Linux/WSL/macOS:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
```

Hoặc dùng script:
```bash
source server/setup_env.sh
```

#### **Windows PowerShell:**
```powershell
$env:DB_CONN = "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
```

Hoặc dùng script:
```powershell
. .\server\setup_env.ps1
```

### **Bước 3: Build Tests**

```bash
cd server

# Build tất cả tests
make all

# Hoặc build từng test riêng
make test_db
make test_auth
make test_rooms
make test_friends
make test_chat
make test_quickmode
make test_onevn
make test_onevn_interactive
```

### **Bước 4: Chạy Tests**

Sau khi build, các file test sẽ nằm trong thư mục `build/`:

```bash
# Linux/WSL/macOS
./build/test_db
./build/test_auth
./build/test_rooms
./build/test_friends
./build/test_chat
./build/test_quickmode
./build/test_onevn
./build/test_onevn_interactive

# Windows PowerShell
.\build\test_db.exe
.\build\test_auth.exe
.\build\test_rooms.exe
.\build\test_friends.exe
.\build\test_chat.exe
.\build\test_quickmode.exe
.\build\test_onevn.exe
.\build\test_onevn_interactive.exe
```

---

## 📝 CHI TIẾT TỪNG TEST

### 1. **test_db** - Test Database Connection

**Mục đích:** Kiểm tra kết nối database có hoạt động không.

**Cách chạy:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_db
```

**Output mong đợi:**
```
Connecting with conninfo: host=localhost port=5432 dbname=ltm_group04 user=postgres password=1
[OK] Connected to database.
Running test query: SELECT 1;
[OK] Query succeeded. Rows=1, Cols=1
```

---

### 2. **test_auth** - Test Authentication

**Mục đích:** Test đăng ký và đăng nhập.

**Cách chạy:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_auth
```

**Tính năng:**
- Test signup (đăng ký user mới)
- Test login (đăng nhập)
- Test tạo session và access token

**Lưu ý:** Test này sẽ tạo users mới trong database.

---

### 3. **test_rooms** - Test Room Management

**Mục đích:** Test tạo phòng và join phòng.

**Cách chạy:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_rooms
```

**Tính năng:**
- Tạo phòng mới (user_id = 1 làm owner)
- User 2 join vào phòng

**Output mong đợi:**
```
Create room OK, id = 1
User 2 joined room
```

---

### 4. **test_friends** - Test Friend System

**Mục đích:** Test tính năng kết bạn.

**Cách chạy:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_friends
```

**Tính năng:**
- Gửi lời mời kết bạn (user 1 → user 2)
- Chấp nhận lời mời
- Lấy danh sách bạn bè

**Yêu cầu:** User ID 1 và 2 phải tồn tại trong database.

---

### 5. **test_chat** - Test Chat System

**Mục đích:** Test gửi/nhận tin nhắn.

**Cách chạy:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_chat
```

**Tính năng:**
- Gửi tin nhắn 1-1 (user 1 → user 2)
- Gửi tin nhắn trong phòng
- Đọc tin nhắn chưa đọc
- Đánh dấu đã đọc

**Yêu cầu:** User ID 1 và 2 phải tồn tại trong database.

---

### 6. **test_quickmode** - Test Quick Mode

**Mục đích:** Test chế độ chơi nhanh (15 câu hỏi).

**Cách chạy:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_quickmode
```

**Tính năng:**
- Tự động lấy 15 câu hỏi (5 EASY, 5 MEDIUM, 5 HARD)
- Mô phỏng trả lời (random)
- Tính điểm với bonus
- Hiển thị kết quả cuối cùng

**Lưu ý:** Test này không lưu kết quả vào database, chỉ test game logic.

---

### 7. **test_onevn** - Test Multiplayer 1vN (Menu)

**Mục đích:** Test multiplayer mode với menu tương tác.

**Cách chạy:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_onevn
```

**Menu Options:**
1. **Tạo phòng mới** - Với config số câu hỏi từng mức độ
2. **Mời người chơi** - Chọn users từ danh sách có sẵn
3. **Xem danh sách thành viên** - Hiển thị tất cả members
4. **Bắt đầu game** - (Placeholder)
5. **Mô phỏng chơi game** - Tự động simulate game flow
6. **Xem stats** - Hiển thị stats của players
7. **Xem leaderboard** - Hiển thị bảng xếp hạng

**Flow Test Đề Xuất:**
```
1. Chọn option 1 → Tạo phòng (ví dụ: 10 dễ, 10 vừa, 10 khó)
2. Chọn option 2 → Mời players (chọn từ danh sách users)
3. Chọn option 3 → Xem danh sách members
4. Chọn option 5 → Mô phỏng game (tự động)
5. Chọn option 6 → Xem stats
6. Chọn option 7 → Xem leaderboard
```

**Tính năng đặc biệt:**
- Hiển thị danh sách users có sẵn trong database
- Cho phép chọn users bằng STT
- Tự động loại bỏ users đã có trong phòng
- Mô phỏng game với random answers
- Tính điểm với bonus và thời gian

**Yêu cầu:**
- Database có ít nhất 1 user (để làm owner)
- Database có ít nhất 30 câu hỏi (để test đầy đủ)

---

### 8. **test_onevn_interactive** - Test Multiplayer 1vN (Interactive)

**Mục đích:** Test multiplayer mode cho phép nhập đáp án thực tế.

**Cách chạy:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_onevn_interactive
```

**Tính năng:**
- Tự động tạo phòng với config mặc định (5 dễ, 5 vừa, 5 khó)
- Hiển thị câu hỏi với timer 15 giây
- Cho phép nhập đáp án bằng phím **A, B, C, D**
- Tính điểm real-time với bonus
- Tự động loại bỏ nếu sai hoặc hết thời gian
- Hiển thị leaderboard cuối game

**Cách chơi:**
1. Chạy test
2. Đọc câu hỏi
3. Nhấn phím **A, B, C**, hoặc **D** để trả lời
4. Nhấn **Q** để thoát sớm
5. Xem điểm số và kết quả

**Tính năng đặc biệt:**
- **Non-blocking input**: Có thể nhập đáp án bất cứ lúc nào trong 15 giây
- **Timer countdown**: Hiển thị thời gian còn lại
- **Score calculation**: Tính điểm với bonus và thời gian như game thật
- **Consecutive bonus**: Track chuỗi đúng liên tiếp

---

## 🔄 CHẠY TẤT CẢ TESTS

### **Linux/WSL/macOS:**

Tạo script `run_all_tests.sh`:

```bash
#!/bin/bash
set -e

export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"

echo "=== Building all tests ==="
cd server
make clean
make all

echo -e "\n=== Running test_db ==="
./build/test_db

echo -e "\n=== Running test_auth ==="
./build/test_auth

echo -e "\n=== Running test_rooms ==="
./build/test_rooms

echo -e "\n=== Running test_friends ==="
./build/test_friends

echo -e "\n=== Running test_chat ==="
./build/test_chat

echo -e "\n=== Running test_quickmode ==="
./build/test_quickmode

echo -e "\n=== All tests completed! ==="
echo "Note: test_onevn and test_onevn_interactive require manual interaction"
```

Chạy:
```bash
chmod +x run_all_tests.sh
./run_all_tests.sh
```

### **Windows PowerShell:**

Tạo script `run_all_tests.ps1`:

```powershell
$env:DB_CONN = "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"

Write-Host "=== Building all tests ==="
cd server
make clean
make all

Write-Host "`n=== Running test_db ==="
.\build\test_db.exe

Write-Host "`n=== Running test_auth ==="
.\build\test_auth.exe

Write-Host "`n=== Running test_rooms ==="
.\build\test_rooms.exe

Write-Host "`n=== Running test_friends ==="
.\build\test_friends.exe

Write-Host "`n=== Running test_chat ==="
.\build\test_chat.exe

Write-Host "`n=== Running test_quickmode ==="
.\build\test_quickmode.exe

Write-Host "`n=== All tests completed! ==="
Write-Host "Note: test_onevn and test_onevn_interactive require manual interaction"
```

Chạy:
```powershell
.\run_all_tests.ps1
```

---

## 🐛 TROUBLESHOOTING

### **Lỗi: "Set DB_CONN env first"**

**Nguyên nhân:** Chưa set biến môi trường `DB_CONN`.

**Giải pháp:**
```bash
# Linux/WSL/macOS
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"

# Windows PowerShell
$env:DB_CONN = "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
```

**Kiểm tra:**
```bash
# Linux/WSL/macOS
echo $DB_CONN

# Windows PowerShell
$env:DB_CONN
```

---

### **Lỗi: "Database connection failed"**

**Nguyên nhân:**
- PostgreSQL chưa chạy
- Connection string sai
- Database chưa được tạo
- Username/password sai

**Giải pháp:**

1. **Kiểm tra PostgreSQL đang chạy:**
```bash
# Linux/WSL
sudo systemctl status postgresql
# hoặc
pg_isready

# Windows
# Kiểm tra trong Services hoặc pgAdmin
```

2. **Kiểm tra database đã tồn tại:**
```bash
psql -U postgres -l | grep ltm_group04
```

3. **Tạo database nếu chưa có:**
```bash
psql -U postgres -f server/db.sql
```

4. **Kiểm tra connection string:**
```bash
# Test kết nối trực tiếp
psql -U postgres -d ltm_group04 -c "SELECT 1;"
```

---

### **Lỗi: "No questions found"**

**Nguyên nhân:** Database chưa có câu hỏi.

**Giải pháp:**
```bash
# Chạy lại db.sql để insert questions
psql -U postgres -d ltm_group04 -f server/db.sql
```

Hoặc kiểm tra:
```sql
SELECT COUNT(*) FROM question;
-- Phải có ít nhất 30 câu hỏi
```

---

### **Lỗi: "Room creation failed" hoặc "User not found"**

**Nguyên nhân:** Database chưa có users.

**Giải pháp:**
```bash
# Chạy lại db.sql để insert test users
psql -U postgres -d ltm_group04 -f server/db.sql
```

Hoặc kiểm tra:
```sql
SELECT user_id, username FROM users;
-- Phải có ít nhất 1 user (Owner)
```

---

### **Lỗi: "make: command not found"**

**Nguyên nhân:** Chưa cài đặt `make`.

**Giải pháp:**

**Linux/WSL:**
```bash
sudo apt-get install make
```

**macOS:**
```bash
# make đã có sẵn, hoặc cài Xcode Command Line Tools
xcode-select --install
```

**Windows:**
- Cài đặt WSL hoặc dùng MinGW/MSYS2
- Hoặc build thủ công với gcc

---

### **Lỗi: "libpq-fe.h: No such file or directory"**

**Nguyên nhân:** Chưa cài PostgreSQL development libraries.

**Giải pháp:**

**Linux/WSL (Ubuntu/Debian):**
```bash
sudo apt-get install libpq-dev
```

**Linux/WSL (Fedora/RHEL):**
```bash
sudo dnf install postgresql-devel
```

**macOS:**
```bash
brew install postgresql
```

**Windows:**
- Cài PostgreSQL từ trang chủ (bao gồm development files)
- Hoặc dùng vcpkg: `vcpkg install libpq`

---

### **Lỗi: "undefined reference to `PQconnectdb`"**

**Nguyên nhân:** Chưa link với PostgreSQL library.

**Giải pháp:**
- Kiểm tra Makefile có `-lpq` trong `LDFLAGS`
- Đảm bảo PostgreSQL libraries đã được cài đặt

---

### **Lỗi khi chạy test_onevn: "No users available"**

**Nguyên nhân:** Database chưa có users hoặc tất cả users đã có trong phòng.

**Giải pháp:**
```bash
# Kiểm tra users trong database
psql -U postgres -d ltm_group04 -c "SELECT user_id, username FROM users;"

# Nếu không có users, chạy lại db.sql
psql -U postgres -d ltm_group04 -f server/db.sql
```

---

## 📊 VERIFY DATABASE SETUP

Để đảm bảo database đã setup đúng, chạy các lệnh sau:

```sql
-- Kiểm tra users
SELECT COUNT(*) as user_count FROM users;
-- Kỳ vọng: >= 12 users

-- Kiểm tra questions
SELECT difficulty_level, COUNT(*) as count 
FROM question 
GROUP BY difficulty_level;
-- Kỳ vọng: 10 EASY, 10 MEDIUM, 10 HARD

-- Kiểm tra tables
SELECT table_name 
FROM information_schema.tables 
WHERE table_schema = 'public';
-- Kỳ vọng: users, rooms, question, messages, friend_relationships, ...
```

---

## 📚 TÀI LIỆU THAM KHẢO

- **README_TEST_ONEVN.md**: Hướng dẫn chi tiết về test 1vN
- **ENV_SETUP.md**: Hướng dẫn setup environment variables
- **SETUP_DATABASE.md**: Hướng dẫn setup database
- **QUICK_START.md**: Hướng dẫn nhanh để bắt đầu

---

## ✅ CHECKLIST TRƯỚC KHI CHẠY TEST

- [ ] PostgreSQL đã được cài đặt và đang chạy
- [ ] Database `ltm_group04` đã được tạo
- [ ] Đã chạy `db.sql` để tạo schema và data
- [ ] Biến môi trường `DB_CONN` đã được set
- [ ] Đã build tất cả tests (`make all`)
- [ ] Database có ít nhất 12 users
- [ ] Database có ít nhất 30 questions (10 EASY, 10 MEDIUM, 10 HARD)

---

**Tạo bởi:** AI Assistant  
**Version:** 1.0  
**Last Updated:** 2024

