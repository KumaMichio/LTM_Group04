# 🧪 HƯỚNG DẪN TEST MULTIPLAYER MODE (1vN)

## 📋 TỔNG QUAN

Có 2 file test cho tính năng multiplayer mode:

1. **test_onevn.c** - Test tự động với menu tương tác
2. **test_onevn_interactive.c** - Test interactive cho phép user nhập đáp án thực tế

---

## 🚀 CÁCH CHẠY

### 1. Build Test Files

```bash
cd server
make test_onevn
make test_onevn_interactive
```

Hoặc build tất cả:
```bash
make all
```

### 2. Set Environment Variable

```bash
export DB_CONN="postgresql://username:password@localhost:5432/ltm_group04"
```

### 3. Chạy Test

#### Test với Menu (test_onevn)
```bash
./build/test_onevn
```

#### Test Interactive (test_onevn_interactive)
```bash
./build/test_onevn_interactive
```

---

## 📝 TEST_ONEVN.C - MENU TEST

### Tính năng

Menu test cho phép:

1. **Tạo phòng mới** - Với config số câu hỏi từng mức độ
   - Nhập số câu hỏi dễ (0-30)
   - Nhập số câu hỏi vừa (0-30)
   - Nhập số câu hỏi khó (0-30)
   - Tổng không quá 30 câu

2. **Mời người chơi** - Thêm players vào phòng
   - Nhập số lượng players muốn mời
   - Tự động tạo user IDs
   - Tối đa 8 players

3. **Xem danh sách thành viên** - Hiển thị tất cả members trong room

4. **Bắt đầu game** - (Placeholder - cần server)

5. **Mô phỏng chơi game** - Tự động simulate game flow:
   - Tạo 1vN session
   - Lấy câu hỏi theo độ khó
   - Mô phỏng players trả lời (random)
   - Tính điểm với bonus và thời gian
   - Loại bỏ players khi sai
   - Hiển thị leaderboard cuối game

6. **Xem stats** - Hiển thị stats của tất cả players

7. **Xem leaderboard** - Hiển thị bảng xếp hạng

### Flow Test

```
1. Tạo phòng → Nhập config (ví dụ: 10 dễ, 10 vừa, 10 khó)
2. Mời người chơi → Nhập số lượng (ví dụ: 4 players)
3. Xem danh sách → Kiểm tra members
4. Mô phỏng game → Xem game chạy tự động
5. Xem stats → Kiểm tra stats đã update
```

---

## 🎮 TEST_ONEVN_INTERACTIVE.C - INTERACTIVE TEST

### Tính năng

Test interactive cho phép bạn chơi như một player thực sự:

- Tự động tạo phòng với config mặc định (5 dễ, 5 vừa, 5 khó)
- Hiển thị câu hỏi với timer 15 giây
- Nhập đáp án bằng phím A, B, C, D
- Tính điểm real-time với bonus
- Tự động loại bỏ nếu sai hoặc hết thời gian

### Cách chơi

1. Chạy test
2. Đọc câu hỏi
3. Nhấn phím A, B, C, hoặc D để trả lời
4. Nhấn Q để thoát
5. Xem điểm số và kết quả

### Tính năng đặc biệt

- **Non-blocking input**: Có thể nhập đáp án bất cứ lúc nào trong 15 giây
- **Timer countdown**: Hiển thị thời gian còn lại
- **Score calculation**: Tính điểm với bonus và thời gian như game thật
- **Consecutive bonus**: Track chuỗi đúng liên tiếp

---

## 📊 VÍ DỤ OUTPUT

### Test Menu

```
╔════════════════════════════════════════════════════════╗
║     🎮 MULTIPLAYER MODE (1vN) - TEST SIMULATION       ║
╚════════════════════════════════════════════════════════╝

1. Tạo phòng mới (với config số câu hỏi)
2. Mời người chơi vào phòng
3. Xem danh sách thành viên trong phòng
4. Bắt đầu game
5. Mô phỏng chơi game (tự động)
6. Xem stats của players
7. Xem leaderboard
0. Thoát

Chọn: 1

=== TẠO PHÒNG MỚI ===
Nhập số câu hỏi DỄ (0-30): 10
Nhập số câu hỏi VỪA (0-30): 10
Nhập số câu hỏi KHÓ (0-30): 10
✅ Tạo phòng thành công!
   Room ID: 1
   Config: 10 dễ, 10 vừa, 10 khó (Tổng: 30 câu)
```

### Game Simulation

```
╔════════════════════════════════════════════════════════╗
║  ROUND 1/30 - EASY                                      ║
╚════════════════════════════════════════════════════════╝

Câu hỏi: Thủ đô của Việt Nam là thành phố nào?
A. Hồ Chí Minh
B. Hà Nội
C. Đà Nẵng
D. Huế

⏱️  Thời gian: 15 giây

--- Kết quả trả lời ---
  Owner: ✅ Đúng (B) - +1000 điểm (Tổng: 1000) - Thời gian: 12.3s
  Player1: ✅ Đúng (B) - +950 điểm (Tổng: 950) - Thời gian: 13.5s
  Player2: ❌ Sai (A, đáp án đúng: B) - BỊ LOẠI
```

### Final Leaderboard

```
╔════════════════════════════════════════════════════════╗
║                    GAME OVER                          ║
╚════════════════════════════════════════════════════════╝

📊 BẢNG XẾP HẠNG:
┌─────┬─────────────┬────────┬──────────┐
│ Hạng│ Người chơi  │ Điểm   │ Trạng thái│
├─────┼─────────────┼────────┼──────────┤
│   1 │ 👑 Owner    │  25000 │ Còn lại  │
│   2 │   Player1   │  23000 │ Còn lại  │
│   3 │   Player2   │   5000 │ Loại     │
└─────┴─────────────┴────────┴──────────┘

🏆 NGƯỜI THẮNG: Owner với 25000 điểm!
```

---

## 🔧 YÊU CẦU

- PostgreSQL database đã setup
- Environment variable `DB_CONN` đã set
- Database có sample questions (chạy `db.sql`)
- Có ít nhất 1 user trong database (để làm owner)

---

## 📝 NOTES

### Test Limitations

1. **Session Manager**: Test không sử dụng session_manager thực sự (vì không có persistent connections)
2. **Broadcast**: Test không test broadcast mechanism (cần server đang chạy)
3. **Timer**: Timer trong test_onevn_interactive dùng `time()` với độ chính xác 1 giây

### Improvements

Để test đầy đủ hơn:
- Chạy server và test với real TCP connections
- Test với multiple clients đồng thời
- Test broadcast mechanism
- Test timer precision

---

## 🐛 TROUBLESHOOTING

### Lỗi: "DB_CONN not set"
```bash
export DB_CONN="postgresql://user:pass@localhost:5432/dbname"
```

### Lỗi: "Database connection failed"
- Kiểm tra PostgreSQL đang chạy
- Kiểm tra connection string đúng
- Kiểm tra database đã được tạo

### Lỗi: "No questions found"
- Chạy `db.sql` để tạo sample questions
- Hoặc thêm questions vào database

### Lỗi: "Room creation failed"
- Đảm bảo user_id = 1 tồn tại trong database
- Hoặc tạo user trước khi test

---

**Tạo bởi:** AI Assistant  
**Version:** 1.0

