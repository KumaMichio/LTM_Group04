# 📦 Hướng Dẫn Migration Database

## 🎯 Migration Script Là Gì?

**Migration script** là một script SQL giúp bạn:
1. **Chuyển đổi cấu trúc database** từ schema cũ sang schema mới
2. **Giữ lại dữ liệu** hiện có (nếu có)
3. **Tự động map** các trường cũ sang trường mới
4. **Xử lý các thay đổi** như đổi tên bảng, đổi tên cột, gộp bảng, etc.

---

## ⚠️ LƯU Ý QUAN TRỌNG

### **Khi Nào Cần Migration?**
- ✅ Bạn đã có dữ liệu trong database cũ
- ✅ Bạn muốn giữ lại dữ liệu khi chuyển sang schema mới
- ✅ Bạn đang ở production và không thể mất dữ liệu

### **Khi Nào KHÔNG Cần Migration?**
- ✅ Database mới, chưa có dữ liệu
- ✅ Chỉ đang development, có thể drop và tạo lại
- ✅ Dữ liệu test, không quan trọng

---

## 📋 Các Bước Migration

### **Bước 1: Backup Database**
```bash
# Backup toàn bộ database trước khi migration
pg_dump -U postgres ltm_group04 > backup_before_migration_$(date +%Y%m%d_%H%M%S).sql
```

### **Bước 2: Chạy Migration Script**
```bash
# Chạy script migration
psql -U postgres -d ltm_group04 -f server/migration_old_to_new.sql
```

### **Bước 3: Verify Dữ Liệu**
```sql
-- Kiểm tra số lượng records
SELECT 'users' as table_name, COUNT(*) FROM users
UNION ALL
SELECT 'messages', COUNT(*) FROM messages
UNION ALL
SELECT 'friend_relationships', COUNT(*) FROM friend_relationships;
```

### **Bước 4: Test Application**
- Chạy server và test các chức năng
- Đảm bảo không có lỗi

### **Bước 5: Cleanup (Sau Khi Verify)**
```sql
-- Xóa các bảng backup sau khi đã verify
DROP TABLE IF EXISTS users_backup;
DROP TABLE IF EXISTS messages_backup;
-- ... các bảng backup khác
```

---

## 🔄 Các Thay Đổi Chính Trong Migration

### **1. Users Table**
- **Cũ**: `users` + `user_stats` (2 bảng)
- **Mới**: `users` (gộp stats vào)
- **Migration**: JOIN và gộp dữ liệu

### **2. Messages Table**
- **Cũ**: `content`, `msg_id`
- **Mới**: `message`, `id`
- **Migration**: Map `content` → `message`, `msg_id` → `id`

### **3. Friends Table**
- **Cũ**: `friends` (composite PK) + `friend_request` (riêng)
- **Mới**: `friend_relationships` (id PK, gộp cả hai)
- **Migration**: 
  - `friend_id` → `peer_user_id`
  - Gộp dữ liệu từ 2 bảng
  - Thêm `DECLINED` status

### **4. Question Table**
- **Cũ**: `op_a`, `op_b`, `op_c`, `op_d`
- **Mới**: `opA`, `opB`, `opC`, `opD`
- **Migration**: Map các trường

### **5. Room Table**
- **Cũ**: Có `mode`, `current_number_players`
- **Mới**: Bỏ `mode`, bỏ `current_number_players`
- **Migration**: Bỏ các trường không cần

### **6. Room Members**
- **Cũ**: Có `role`
- **Mới**: Bỏ `role`
- **Migration**: Bỏ trường `role`

### **7. 1vN Sessions**
- **Cũ**: `onevn_sessions` + `onevn_players` + `onevn_answers` + `onevn_rounds` (nhiều bảng)
- **Mới**: `onevn_sessions` với `players` JSONB
- **Migration**: Aggregate dữ liệu từ `onevn_players` thành JSONB

### **8. Quickmode**
- **Cũ**: Có các bảng `quickmode_sessions`, `quickmode_rounds`, `quickmode_answers`
- **Mới**: Không có bảng (không lưu trữ)
- **Migration**: Xóa các bảng (dữ liệu sẽ mất)

---

## 🚨 Xử Lý Lỗi

### **Lỗi: Constraint Violation**
```sql
-- Nếu có duplicate data, script sẽ skip (ON CONFLICT DO NOTHING)
-- Kiểm tra và xử lý duplicate trước khi migration
```

### **Lỗi: Foreign Key**
```sql
-- Script đã drop foreign keys trước khi drop tables
-- Nếu vẫn lỗi, check xem có bảng nào đang reference không
```

### **Lỗi: Sequence**
```sql
-- Script đã reset sequences
-- Nếu vẫn lỗi, reset manual:
SELECT setval('users_user_id_seq', (SELECT MAX(user_id) FROM users));
```

---

## ✅ Checklist Migration

- [ ] Backup database
- [ ] Review migration script
- [ ] Chạy migration script
- [ ] Verify số lượng records
- [ ] Test application
- [ ] Verify dữ liệu quan trọng
- [ ] Cleanup backup tables (sau khi verify)

---

## 🔄 Rollback (Nếu Cần)

Nếu migration thất bại, restore từ backup:

```bash
# Drop database hiện tại
psql -U postgres -c "DROP DATABASE ltm_group04;"

# Tạo lại database
psql -U postgres -c "CREATE DATABASE ltm_group04;"

# Restore từ backup
psql -U postgres -d ltm_group04 < backup_before_migration_YYYYMMDD_HHMMSS.sql
```

---

## 💡 Tips

1. **Luôn backup trước khi migration**
2. **Test trên database copy trước**
3. **Verify dữ liệu sau migration**
4. **Giữ backup tables cho đến khi chắc chắn**
5. **Document các thay đổi quan trọng**

