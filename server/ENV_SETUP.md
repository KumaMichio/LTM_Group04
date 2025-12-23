# 🔧 Hướng Dẫn Set Biến Môi Trường DB_CONN

## 📋 Tổng Quan

Các chương trình cần biến môi trường `DB_CONN` để kết nối PostgreSQL.

**Format:**
```
host=HOST port=PORT dbname=DATABASE user=USERNAME password=PASSWORD
```

---

## 🐧 **WSL / Linux / macOS**

### **Cách 1: Export trực tiếp (tạm thời)**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
```

### **Cách 2: Dùng script setup_env.sh**
```bash
# Chạy script để set
source server/setup_env.sh

# Hoặc
. server/setup_env.sh
```

### **Cách 3: Set trong ~/.bashrc (vĩnh viễn)**
```bash
# Thêm vào ~/.bashrc
echo 'export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"' >> ~/.bashrc
source ~/.bashrc
```

### **Cách 4: Set khi chạy command**
```bash
DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1" ./build/server
```

---

## 🪟 **Windows PowerShell**

### **Cách 1: Set trong session hiện tại**
```powershell
$env:DB_CONN = "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
```

### **Cách 2: Dùng script setup_env.ps1**
```powershell
. .\server\setup_env.ps1
```

### **Cách 3: Set khi chạy command**
```powershell
$env:DB_CONN = "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"; .\build\server.exe
```

### **Cách 4: Set vĩnh viễn (System-wide)**
```powershell
# User-level
[System.Environment]::SetEnvironmentVariable("DB_CONN", "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1", "User")

# System-level (cần admin)
[System.Environment]::SetEnvironmentVariable("DB_CONN", "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1", "Machine")
```

---

## 🧪 **Chạy Tests**

### **WSL/Linux:**
```bash
# Set env và chạy test
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_auth
./build/test_db
./build/test_friends
```

### **Windows PowerShell:**
```powershell
$env:DB_CONN = "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
.\build\test_auth.exe
.\build\test_db.exe
```

---

## 🚀 **Chạy Server**

### **WSL/Linux:**
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
export SERVER_MODE="1"
export SERVER_PORT="9000"
./build/server
```

### **Windows PowerShell:**
```powershell
$env:DB_CONN = "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
$env:SERVER_MODE = "1"
$env:SERVER_PORT = "9000"
.\build\server.exe
```

---

## ⚙️ **Cấu Hình Database**

### **Kiểm tra PostgreSQL đang chạy:**
```bash
# Linux/WSL
sudo systemctl status postgresql
# hoặc
pg_isready

# Windows
# Kiểm tra trong Services hoặc pgAdmin
```

### **Tạo database nếu chưa có:**
```bash
# Chạy schema SQL
psql -U postgres -f server/db.sql
```

### **Connection String Examples:**

**Local PostgreSQL:**
```
host=localhost port=5432 dbname=ltm_group04 user=postgres password=1
```

**Remote PostgreSQL:**
```
host=192.168.1.100 port=5432 dbname=ltm_group04 user=myuser password=mypass
```

**Với SSL:**
```
host=localhost port=5432 dbname=ltm_group04 user=postgres password=1 sslmode=require
```

---

## 🔍 **Verify Environment Variable**

### **Linux/WSL:**
```bash
echo $DB_CONN
```

### **Windows PowerShell:**
```powershell
$env:DB_CONN
```

---

## ⚠️ **Troubleshooting**

### **Lỗi: "Set DB_CONN env first"**
- ✅ Đảm bảo đã set `DB_CONN` trước khi chạy
- ✅ Kiểm tra: `echo $DB_CONN` (Linux) hoặc `$env:DB_CONN` (PowerShell)

### **Lỗi: "DB connection failed"**
- ✅ Kiểm tra PostgreSQL đang chạy
- ✅ Kiểm tra connection string đúng
- ✅ Kiểm tra database `ltm_group04` đã tồn tại
- ✅ Kiểm tra username/password đúng

### **Lỗi: "database does not exist"**
- ✅ Chạy `psql -U postgres -f server/db.sql` để tạo database

---

## 💡 **Quick Start**

**WSL/Linux:**
```bash
cd server
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
./build/test_db  # Test connection
```

**Windows PowerShell:**
```powershell
cd server
$env:DB_CONN = "host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
.\build\test_db.exe  # Test connection
```

