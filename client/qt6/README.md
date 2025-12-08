# Qt6 Client - Ai là triệu phú

GUI client cho game "Ai là triệu phú" sử dụng Qt6.

## Yêu cầu

- Qt6 (Core, Widgets, Network)
- CMake 3.16+
- C++17 compiler

## Cài đặt Qt6

### Windows
Tải và cài đặt từ [Qt Official Website](https://www.qt.io/download) hoặc sử dụng Qt Maintenance Tool.

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install qt6-base-dev qt6-base-dev-tools
```

### macOS
```bash
brew install qt6
```

## Build

```bash
cd client/qt6
mkdir build
cd build
cmake ..
make
# hoặc trên Windows với Visual Studio:
# cmake .. -G "Visual Studio 17 2022"
# cmake --build .
```

## Chạy

```bash
./MillionaireClient
# hoặc trên Windows:
# .\MillionaireClient.exe
```

## Sử dụng

1. **Kết nối đến server:**
   - Nhập host (mặc định: localhost)
   - Nhập port (mặc định: 8080)
   - Click "Kết nối"

2. **Đăng ký tài khoản mới:**
   - Chuyển sang tab "Đăng ký"
   - Nhập tên đăng nhập và mật khẩu
   - Click "Đăng ký"

3. **Đăng nhập:**
   - Ở tab "Đăng nhập"
   - Nhập tên đăng nhập và mật khẩu
   - Click "Đăng nhập"

4. **Đăng xuất:**
   - Click "Đăng xuất" sau khi đã đăng nhập

## Protocol

Client sử dụng binary protocol với header 8 bytes + JSON payload:
- Header: cmd (2 bytes) + user_id (2 bytes) + length (4 bytes) - network byte order
- Payload: JSON string

Commands:
- 0x0101: Register
- 0x0103: Login
- 0x0106: Logout

## Troubleshooting

### Lỗi kết nối
- Kiểm tra server có đang chạy không
- Kiểm tra host và port có đúng không
- Kiểm tra firewall

### Lỗi build
- Đảm bảo Qt6 đã được cài đặt đúng
- Kiểm tra CMake có tìm thấy Qt6: `cmake ..` sẽ hiển thị Qt6 path
- Trên Windows, có thể cần set `CMAKE_PREFIX_PATH` đến Qt6 installation

## Cấu trúc files

```
client/qt6/
├── CMakeLists.txt      # CMake configuration
├── main.cpp           # Entry point
├── LoginWindow.h      # Login window header
├── LoginWindow.cpp    # Login window implementation
├── LoginWindow.ui     # UI form (Qt Designer)
├── NetworkClient.h    # Network client header
├── NetworkClient.cpp  # Network client implementation
└── README.md         # This file
```

