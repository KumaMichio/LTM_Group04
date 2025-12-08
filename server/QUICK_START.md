# 🚀 Quick Start Guide

## ❓ Câu Hỏi: Có Cần pgAdmin4 Không?

**Trả lời: KHÔNG CẦN!** Chỉ cần PostgreSQL trên WSL là đủ.

Bạn có thể:
- ✅ Dùng command line (`psql`) - Đơn giản, nhanh
- ✅ Dùng script tự động - Dễ nhất
- ❌ Không cần pgAdmin4 (chỉ là GUI, không bắt buộc)

---

## 📋 Setup Nhanh (3 Bước)

### **Bước 1: Đảm bảo PostgreSQL đang chạy**

```bash
# Kiểm tra PostgreSQL
sudo systemctl status postgresql

# Nếu chưa chạy, start nó
sudo systemctl start postgresql

# Hoặc check port
netstat -tuln | grep 5432
```

### **Bước 2: Tạo Database**

**Cách A: Dùng script tự động (Khuyến nghị)**
```bash
cd server
bash create_database.sh
```

**Cách B: Manual**
```bash
# Connect với postgres user
sudo -u postgres psql

# Tạo database
CREATE DATABASE ltm_group04;

# Chạy schema
\c ltm_group04
\i db.sql
\q
```

**Cách C: Một lệnh**
```bash
# Set password nếu cần
export PGPASSWORD="1"

# Tạo database và chạy schema
psql -U postgres -c "CREATE DATABASE ltm_group04;" 2>/dev/null || true
psql -U postgres -d ltm_group04 -f server/db.sql
```

### **Bước 3: Set DB_CONN và Test**

```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_db
```

---

## 🔧 Nếu Gặp Lỗi Authentication

### **Reset Password:**
```bash
sudo -u postgres psql
ALTER USER postgres WITH PASSWORD '1';
\q
```

### **Hoặc tạo user mới:**
```bash
sudo -u postgres psql
CREATE USER ltm_user WITH PASSWORD 'ltm_pass';
CREATE DATABASE ltm_group04 OWNER ltm_user;
GRANT ALL PRIVILEGES ON DATABASE ltm_group04 TO ltm_user;
\q
```

Sau đó:
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=ltm_user password=ltm_pass"
```

---

## ✅ Verify Setup

```bash
# Test connection
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_db

# Test với psql
psql "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1" -c "SELECT COUNT(*) FROM users;"
```

---

## 🎯 Workflow Hoàn Chỉnh

```bash
# 1. Start PostgreSQL (nếu chưa chạy)
sudo systemctl start postgresql

# 2. Reset password (nếu cần)
sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD '1';"

# 3. Tạo database
cd server
bash create_database.sh

# 4. Set environment
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"

# 5. Test
./build/test_db
./build/test_auth
```

---

## 💡 Tips

1. **Không cần pgAdmin4** - Command line đủ dùng
2. **Dùng script** - Tự động hóa, ít lỗi
3. **Lưu DB_CONN** - Thêm vào `~/.bashrc` để không phải set lại
4. **Check logs** - Nếu lỗi, check `/var/log/postgresql/` hoặc `journalctl -u postgresql`

---

## 🆘 Troubleshooting

### **PostgreSQL không chạy:**
```bash
sudo systemctl start postgresql
sudo systemctl enable postgresql  # Auto-start on boot
```

### **Port 5432 đã được dùng:**
```bash
# Check process
sudo lsof -i :5432
# Hoặc
sudo netstat -tulpn | grep 5432
```

### **Permission denied:**
```bash
# Check PostgreSQL data directory permissions
sudo chown -R postgres:postgres /var/lib/postgresql
```

