# 🚀 Hướng Dẫn Chạy Qt6 Client

## 📋 Yêu Cầu

1. **Qt6** đã được cài đặt
2. **CMake 3.16+**
3. **C++17 compiler** (g++, clang++)
4. **Server đang chạy** (port 9000 mặc định, hoặc port bạn đã set qua `SERVER_PORT`)

## 🔧 Bước 1: Cài Đặt Qt6 (Nếu Chưa Có)

### **WSL / Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-base-dev-tools cmake build-essential
```

### **Windows:**
Tải và cài đặt từ [Qt Official Website](https://www.qt.io/download)

### **Kiểm tra Qt6 đã cài:**
```bash
qmake6 --version
# hoặc
cmake --find-package -DNAME=Qt6 -DCOMPILER_ID=GNU -DLANGUAGE=CXX
```

## 🔨 Bước 2: Build Client

### **WSL / Linux:**

```bash
cd client/qt6
mkdir -p build
cd build
cmake ..
make
```

Nếu build thành công, bạn sẽ thấy file `MillionaireClient` trong thư mục `build/`.

### **Windows (PowerShell):**

```powershell
cd client\qt6
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

Hoặc với Visual Studio:
```powershell
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

### **Qt Creator IDE (Khuyến nghị cho Windows):**

Qt Creator hỗ trợ CMake projects. Các bước:

#### **Bước 1: Mở Project trong Qt Creator**

1. **Khởi động Qt Creator**
2. **File → Open File or Project...** (hoặc `Ctrl+O`)
3. **Chọn file `CMakeLists.txt`** trong thư mục `client/qt6/`
4. **Click "Open"**

#### **Bước 2: Configure Project**

1. Qt Creator sẽ hiện **"Configure Project"** dialog
2. **Chọn Kit:**
   - **Desktop Qt 6.x.x MinGW 64-bit** (khuyến nghị cho Windows)
   - Hoặc **Desktop Qt 6.x.x MSVC2019 64-bit** (nếu có Visual Studio)
3. **Chọn Build Directory:** 
   - Mặc định: `client/qt6/build-MillionaireClient-Desktop_Qt_6_x_x_MinGW_64_bit-Debug`
   - Hoặc chọn thư mục `build` hiện có
4. **Click "Configure Project"**

#### **Bước 3: Build Project**

1. **Build → Build Project "MillionaireClient"** (hoặc `Ctrl+B`)
2. Hoặc click nút **Build** (🔨) trên thanh toolbar
3. Xem kết quả trong **Compile Output** panel ở dưới

#### **Bước 4: Run Project**

1. **Build → Run** (hoặc `Ctrl+R`)
2. Hoặc click nút **Run** (▶️) trên thanh toolbar
3. Application sẽ chạy và hiển thị login window

#### **Cấu hình Build (nếu cần):**

1. **Projects** (bên trái) → Chọn **Build Settings**
2. **Build Directory:** Có thể thay đổi
3. **Build Steps:**
   - **CMake:** Có thể thêm arguments như `-DCMAKE_BUILD_TYPE=Release`
   - **Build:** Có thể set số jobs parallel (`-j4`)
4. **Run Settings:**
   - **Run in terminal:** Bật nếu muốn xem console output

#### **Troubleshooting Qt Creator:**

**Lỗi: "No suitable kit found"**
- **Giải pháp:** 
  - **Tools → Options → Kits**
  - Đảm bảo có Qt6 kit được cấu hình
  - Nếu chưa có, **Add** kit mới và chọn Qt6 installation path

**Lỗi: "CMake Error: Could not find Qt6"**
- **Giải pháp:**
  - **Projects → Build Settings → CMake**
  - Thêm CMake variable: `CMAKE_PREFIX_PATH` = `C:\Qt\6.x.x\mingw_64` (hoặc path Qt6 của bạn)
  - Click **"Run CMake"** để reconfigure

**Lỗi: "Cannot find compiler"**
- **Giải pháp:**
  - **Tools → Options → Kits → Compilers**
  - Đảm bảo MinGW hoặc MSVC compiler được cấu hình đúng
  - Nếu chưa có, Qt Creator sẽ tự detect hoặc bạn cần cài MinGW/MSVC

**Build thành công nhưng không chạy được:**
- Kiểm tra **Run Settings → Executable** có đúng không
- Kiểm tra **Working Directory** có đúng không
- Xem **Application Output** panel để xem lỗi runtime

#### **Tips cho Qt Creator:**

1. **Debug Mode:**
   - Set **Build Configuration** = **Debug** để debug
   - Set **Release** để build optimized version

2. **Clean Build:**
   - **Build → Clean Project "MillionaireClient"** để xóa build files
   - Sau đó build lại

3. **View UI Files:**
   - Double-click `LoginWindow.ui` để mở Qt Designer
   - Chỉnh sửa UI trực tiếp trong Qt Creator

4. **Quick Build:**
   - Dùng `Ctrl+B` để build nhanh
   - `Ctrl+R` để build và run

## 🎮 Bước 3: Chạy Client

### **WSL / Linux:**

```bash
cd client/qt6/build
./MillionaireClient
```

Hoặc chạy từ thư mục gốc:
```bash
cd client/qt6/build
DISPLAY=:0 ./MillionaireClient
```

**Lưu ý:** Nếu chạy trong WSL, bạn cần:
- Cài X server (XLaunch, VcXsrv, hoặc X410)
- Set DISPLAY: `export DISPLAY=:0` hoặc `export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0.0`

### **Windows:**

```powershell
cd client\qt6\build
.\MillionaireClient.exe
```

Hoặc double-click file `MillionaireClient.exe` trong Windows Explorer.

## 🌐 Kết Nối Cross-Platform: Qt Client (Windows) ↔ Server (WSL)

**✅ CÓ THỂ!** Bạn hoàn toàn có thể chạy Qt client trên Windows và kết nối đến server đang chạy trong WSL.

### **Cách hoạt động:**
- WSL và Windows **chia sẻ cùng network stack**
- Server trong WSL lắng nghe trên `localhost` (hoặc `0.0.0.0`)
- Client trên Windows có thể kết nối qua `localhost` hoặc IP của WSL

### **Các bước:**

1. **Chạy server trong WSL:**
   ```bash
   # Trong WSL terminal
   cd server
   export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
   export SERVER_MODE="1"
   export SERVER_PORT="9000"  # hoặc port khác
   ./build/server
   ```

2. **Build và chạy Qt client trên Windows:**
   ```powershell
   # Trong PowerShell (Windows)
   cd client\qt6\build
   .\MillionaireClient.exe
   ```

3. **Kết nối từ client:**
   - **Host**: `localhost` (hoặc `127.0.0.1`) - **Điều này hoạt động!**
   - **Port**: `9000` (hoặc port bạn đã set)
   - Click **"Kết nối"**

### **Tại sao `localhost` hoạt động?**
- WSL2: Server lắng nghe trên `0.0.0.0` hoặc `localhost` → Windows có thể truy cập qua `localhost`
- WSL1: Tự động forward ports → `localhost` hoạt động trực tiếp

### **Nếu `localhost` không hoạt động, dùng WSL IP:**
```bash
# Trong WSL, lấy IP của WSL
hostname -I
# Hoặc
ip addr show eth0 | grep "inet " | awk '{print $2}' | cut -d/ -f1
```

Sau đó nhập IP này vào client (ví dụ: `172.x.x.x`)

### **Kiểm tra kết nối:**
```powershell
# Trong PowerShell (Windows), kiểm tra server có đang lắng nghe không
netstat -an | findstr :9000
# Hoặc
Test-NetConnection -ComputerName localhost -Port 9000
```

## 📝 Sử Dụng Client

1. **Kết nối đến server:**
   - Nhập **Host**: `localhost` (hoặc IP server)
   - Nhập **Port**: `9000` (mặc định, hoặc port server đang chạy)
   - Click **"Kết nối"**
   - Kiểm tra trạng thái: "Trạng thái: Đã kết nối" (màu xanh)

2. **Đăng ký tài khoản mới:**
   - Chuyển sang tab **"Đăng ký"**
   - Nhập tên đăng nhập (tối đa 32 ký tự)
   - Nhập mật khẩu (tối thiểu 3 ký tự)
   - Nhập lại mật khẩu để xác nhận
   - Click **"Đăng ký"**
   - Nếu thành công: "Đăng ký thành công! Vui lòng đăng nhập."

3. **Đăng nhập:**
   - Ở tab **"Đăng nhập"**
   - Nhập tên đăng nhập và mật khẩu
   - Click **"Đăng nhập"**
   - Nếu thành công: "Đăng nhập thành công!" và nút "Đăng xuất" sẽ được kích hoạt

4. **Đăng xuất:**
   - Click **"Đăng xuất"** sau khi đã đăng nhập

## 🐛 Troubleshooting

### **Lỗi: "CMake Error: Could not find Qt6"**

**Giải pháp:**
```bash
# Tìm Qt6 installation
find /usr -name "Qt6Config.cmake" 2>/dev/null

# Set CMAKE_PREFIX_PATH
cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6
```

Hoặc trên Windows:
```powershell
cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2019_64"
```

### **Lỗi: "No display" (WSL)**

**Giải pháp:**
1. Cài X server (VcXsrv hoặc X410)
2. Set DISPLAY:
   ```bash
   export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0.0
   ```
3. Chạy lại client

### **Lỗi: "Connection refused"**

**Kiểm tra:**
- Server có đang chạy không: 
  - WSL: `ps aux | grep server`
  - Windows: `netstat -an | findstr :9000`
- Port có đúng không (mặc định **9000**, không phải 8080)
- Firewall có chặn không
- Nếu server chạy trong WSL, thử dùng WSL IP thay vì `localhost`:
  ```bash
  # Trong WSL
  hostname -I
  ```

### **Lỗi: "Cannot find -lQt6::Core"**

**Giải pháp:**
```bash
# Rebuild với clean
cd build
rm -rf *
cmake ..
make
```

### **Lỗi Build: "undefined reference"**

**Giải pháp:**
- Đảm bảo tất cả file .cpp đã được thêm vào CMakeLists.txt
- Clean và rebuild:
  ```bash
  cd build
  rm -rf *
  cmake ..
  make
  ```

## 📋 Quick Start (Tóm Tắt)

```bash
# 1. Cài Qt6 (nếu chưa có)
sudo apt-get install qt6-base-dev qt6-base-dev-tools cmake

# 2. Build
cd client/qt6
mkdir -p build && cd build
cmake ..
make

# 3. Chạy (WSL cần X server)
export DISPLAY=:0  # hoặc IP của Windows host
./MillionaireClient
```

## 🔗 Liên Kết

- [Qt6 Documentation](https://doc.qt.io/qt-6/)
- [CMake Documentation](https://cmake.org/documentation/)
- Server setup: Xem `server/HOW_TO_RUN_SERVER.md`


