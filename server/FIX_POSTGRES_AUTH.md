# 🔐 Fix PostgreSQL Authentication Error

## ❌ Lỗi: "password authentication failed for user postgres"

Lỗi này xảy ra khi password không đúng hoặc user không có quyền.

---

## 🔧 Các Cách Fix

### **Cách 1: Reset Password cho User postgres**

#### **Bước 1: Tìm file pg_hba.conf**
```bash
# Tìm vị trí file
sudo find /etc -name pg_hba.conf 2>/dev/null
# Hoặc
sudo find /var/lib -name pg_hba.conf 2>/dev/null
# Hoặc trên Windows với PostgreSQL mặc định:
# C:\Program Files\PostgreSQL\<version>\data\pg_hba.conf
```

#### **Bước 2: Tạm thời cho phép local connection không cần password**
Mở file `pg_hba.conf` và tìm dòng:
```
host    all             all             127.0.0.1/32            md5
```
Đổi thành:
```
host    all             all             127.0.0.1/32            trust
```

#### **Bước 3: Restart PostgreSQL**
```bash
# Linux/WSL
sudo systemctl restart postgresql
# Hoặc
sudo service postgresql restart

# Windows: Restart service trong Services
```

#### **Bước 4: Reset password**
```bash
psql -U postgres
# Hoặc nếu vẫn cần password, dùng:
sudo -u postgres psql
```

Trong psql:
```sql
ALTER USER postgres WITH PASSWORD '1';
-- Hoặc password bạn muốn
\q
```

#### **Bước 5: Đổi lại pg_hba.conf về md5**
Đổi lại:
```
host    all             all             127.0.0.1/32            md5
```
Restart PostgreSQL lại.

---

### **Cách 2: Tạo User Mới (Khuyến nghị)**

#### **Bước 1: Connect với quyền admin**
```bash
# Nếu có sudo access
sudo -u postgres psql

# Hoặc nếu biết password của user khác
psql -U <admin_user> -d postgres
```

#### **Bước 2: Tạo user và database mới**
```sql
-- Tạo user mới
CREATE USER ltm_user WITH PASSWORD 'ltm_password';

-- Tạo database
CREATE DATABASE ltm_group04 OWNER ltm_user;

-- Cấp quyền
GRANT ALL PRIVILEGES ON DATABASE ltm_group04 TO ltm_user;

-- Exit
\q
```

#### **Bước 3: Update DB_CONN**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=ltm_user password=ltm_password"
```

---

### **Cách 3: Dùng Peer Authentication (Linux/WSL)**

Nếu bạn đang chạy với user có cùng tên với PostgreSQL user:

#### **Bước 1: Check current user**
```bash
whoami
# Nếu output là "postgres", bạn có thể dùng peer auth
```

#### **Bước 2: Update pg_hba.conf**
```
local   all             postgres                                peer
host    all             postgres        127.0.0.1/32            peer
```

#### **Bước 3: Connect không cần password**
```bash
psql -U postgres -d postgres
```

---

### **Cách 4: Tìm Password Hiện Tại**

Nếu bạn quên password, có thể reset:

#### **Linux/WSL:**
```bash
# Stop PostgreSQL
sudo systemctl stop postgresql

# Start PostgreSQL với --single mode
sudo -u postgres /usr/lib/postgresql/*/bin/postgres --single -D /var/lib/postgresql/*/main

# Trong PostgreSQL prompt:
ALTER USER postgres WITH PASSWORD 'new_password';
```

#### **Windows:**
1. Mở Services (services.msc)
2. Stop PostgreSQL service
3. Mở Command Prompt as Administrator
4. Chạy:
```cmd
cd "C:\Program Files\PostgreSQL\<version>\bin"
pg_ctl.exe -D "C:\Program Files\PostgreSQL\<version>\data" -o "-F -p 5432" start
```

---

## 🧪 **Test Connection**

### **Test với psql:**
```bash
# Test với password mới
psql -U postgres -h localhost -d postgres -W
# Nhập password khi được hỏi

# Hoặc với connection string
psql "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
```

### **Test với test_db:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=YOUR_PASSWORD"
./build/test_db
```

---

## 🔍 **Troubleshooting**

### **Lỗi: "could not connect to server"**
- ✅ Kiểm tra PostgreSQL đang chạy: `sudo systemctl status postgresql`
- ✅ Kiểm tra port 5432: `netstat -tuln | grep 5432`

### **Lỗi: "database does not exist"**
- ✅ Tạo database: `createdb -U postgres ltm_group04`
- ✅ Hoặc chạy: `psql -U postgres -f server/db.sql`

### **Lỗi: "permission denied"**
- ✅ Kiểm tra user có quyền truy cập database
- ✅ Grant quyền: `GRANT ALL PRIVILEGES ON DATABASE ltm_group04 TO postgres;`

---

## 💡 **Quick Fix Script**

Tạo file `fix_postgres.sh`:

```bash
#!/bin/bash
# Quick fix: Reset postgres password to "1"

echo "Attempting to reset postgres password..."
echo "You may need to enter sudo password"

# Try to connect and reset
sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD '1';" 2>/dev/null

if [ $? -eq 0 ]; then
    echo "✅ Password reset successful!"
    echo "New password: 1"
    echo ""
    echo "Set DB_CONN:"
    echo "export DB_CONN=\"host=localhost port=5432 dbname=ltm_group04 user=postgres password=1\""
else
    echo "❌ Failed. Try manual steps above."
fi
```

---

## 📝 **Recommended Setup**

**Tạo user riêng cho project (an toàn hơn):**

```sql
-- Connect as postgres
sudo -u postgres psql

-- Tạo user và database
CREATE USER ltm_user WITH PASSWORD 'ltm_pass_123';
CREATE DATABASE ltm_group04 OWNER ltm_user;
GRANT ALL PRIVILEGES ON DATABASE ltm_group04 TO ltm_user;

-- Exit
\q
```

**Update DB_CONN:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=ltm_user password=ltm_pass_123"
```

