# 🗄️ Hướng Dẫn Tạo Database

## ❓ Có Cần pgAdmin4 Không?

**KHÔNG CẦN!** Chỉ cần PostgreSQL trên WSL là đủ.

- ✅ **Command line (`psql`)** - Đơn giản, nhanh, đủ dùng
- ✅ **Script tự động** - Dễ nhất
- ❌ **pgAdmin4** - Chỉ là GUI, không bắt buộc

---

## 🚀 Cách Nhanh Nhất (1 Lệnh)

### **WSL/Linux:**
```bash
cd server
bash setup_database.sh
```

Script này sẽ:
1. ✅ Kiểm tra PostgreSQL đang chạy
2. ✅ Tạo database `ltm_group04`
3. ✅ Chạy schema SQL
4. ✅ Hiển thị DB_CONN để set

---

## 📋 Các Bước Manual

### **Bước 1: Kiểm tra PostgreSQL**
```bash
# Check status
sudo systemctl status postgresql

# Nếu chưa chạy
sudo systemctl start postgresql
```

### **Bước 2: Tạo Database**

**Cách A: Dùng sudo (peer authentication) - Khuyến nghị**
```bash
# Connect không cần password
sudo -u postgres psql

# Trong psql:
CREATE DATABASE ltm_group04;
\q

# Chạy schema
sudo -u postgres psql -d ltm_group04 -f server/db.sql
```

**Cách B: Dùng password**
```bash
# Set password
export PGPASSWORD="1"

# Tạo database
psql -U postgres -h localhost -c "CREATE DATABASE ltm_group04;"

# Chạy schema
psql -U postgres -h localhost -d ltm_group04 -f server/db.sql
```

### **Bước 3: Set DB_CONN**

**Nếu dùng peer auth (không cần password):**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres"
```

**Nếu dùng password:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
```

### **Bước 4: Test**
```bash
./build/test_db
```

---

## 🔧 Fix Peer Authentication

Nếu gặp lỗi "Peer authentication failed":

### **Option 1: Dùng sudo (Khuyến nghị)**
```bash
# Thay vì: psql -U postgres
# Dùng: sudo -u postgres psql
sudo -u postgres psql
```

### **Option 2: Set password và dùng TCP**
```bash
# Reset password
sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD '1';"

# Dùng TCP connection (thêm -h localhost)
psql -U postgres -h localhost -d postgres
```

### **Option 3: Đổi pg_hba.conf**
```bash
# Tìm file
sudo find /etc -name pg_hba.conf
# Hoặc
sudo find /var/lib -name pg_hba.conf

# Edit file, tìm dòng:
# local   all             postgres                                peer
# Đổi thành:
# local   all             postgres                                md5

# Restart
sudo systemctl restart postgresql
```

---

## ✅ Verify Database Đã Tạo

```bash
# List databases
sudo -u postgres psql -l

# Hoặc
psql -U postgres -h localhost -l

# Check tables
psql -U postgres -h localhost -d ltm_group04 -c "\dt"
```

---

## 🎯 Workflow Hoàn Chỉnh

```bash
# 1. Start PostgreSQL
sudo systemctl start postgresql

# 2. Tạo database (chọn 1 trong 2 cách)

# Cách A: Peer auth
sudo -u postgres psql -c "CREATE DATABASE ltm_group04;"
sudo -u postgres psql -d ltm_group04 -f server/db.sql

# Cách B: Password auth
export PGPASSWORD="1"
psql -U postgres -h localhost -c "CREATE DATABASE ltm_group04;"
psql -U postgres -h localhost -d ltm_group04 -f server/db.sql

# 3. Set DB_CONN
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"

# 4. Test
./build/test_db
```

---

## 💡 Tips

1. **Dùng script tự động** - `bash setup_database.sh` - Dễ nhất
2. **Peer auth** - Không cần password khi dùng `sudo -u postgres`
3. **TCP connection** - Thêm `-h localhost` để dùng password auth
4. **Lưu DB_CONN** - Thêm vào `~/.bashrc` để không phải set lại

---

## 🆘 Troubleshooting

### **"Peer authentication failed"**
→ Dùng `sudo -u postgres` thay vì `psql -U postgres`

### **"Database does not exist"**
→ Chạy: `sudo -u postgres psql -c "CREATE DATABASE ltm_group04;"`

### **"Password authentication failed"**
→ Reset: `sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD '1';"`

### **"Connection refused"**
→ Start PostgreSQL: `sudo systemctl start postgresql`

