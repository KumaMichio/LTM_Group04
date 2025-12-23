# 📦 **VAI TRÒ CỦA STRUCT `Packet` TRONG SERVER**

## **1️⃣ ĐỊNH NGHĨA STRUCT**

```c
// Header file: include/service/protocol.h

typedef struct {
    uint16_t cmd;        // CommandType: 0x0101, 0x0103, ...
    uint16_t user_id;    // 0 nếu chưa login, else user ID
    uint32_t length;     // độ dài payload (bytes)
} PacketHeader;

typedef struct {
    PacketHeader header;
    char payload[];      // dữ liệu theo định dạng JSON (flexible array)
} Packet;
```

---

## **2️⃣ CẤU TRÚC VÀ LAYOUT**

### **Memory Layout**

```
┌─────────────────────────────────────────────────────────────────┐
│                          PACKET                                 │
├─────────────────────────────────────────────────────────────────┤
│                    PACKET HEADER (8 bytes)                      │
├──────────────────┬──────────────────┬──────────────────────────┤
│ cmd              │ user_id          │ length                   │
│ uint16_t (2 B)   │ uint16_t (2 B)   │ uint32_t (4 B)           │
│ 0x0103 (LOGIN)   │ 0 (not logged)   │ 37 (bytes of payload)    │
├──────────────────┴──────────────────┴──────────────────────────┤
│              PAYLOAD (variable length)                          │
├─────────────────────────────────────────────────────────────────┤
│ JSON Data (UTF-8 string)                                        │
│ {"username":"alice","password":"secret123"}                    │
└─────────────────────────────────────────────────────────────────┘
```

### **Byte Order (Network Byte Order = Big Endian)**

```
On client sending login (cmd=0x0103):

Raw bytes transmitted over TCP:
┌──┬──┬──┬──┬──┬──┬──┬──┬ ... payload bytes ...
│01│03│00│00│00│00│00│25│
└──┴──┴──┴──┴──┴──┴──┴──┴
  cmd    user_id  length
  (htons) (htons) (htonl)

Convert from network byte order on server:
cmd = ntohs(0x0103) = 0x0103
user_id = ntohs(0x0000) = 0
length = ntohl(0x00000025) = 37 bytes
```

---

## **3️⃣ VAI TRÒ CHÍNH**

### **A. ĐỊNH NGHĨA PROTOCOL (Giao Thức Truyền Thông)**

`Packet` struct định nghĩa **format chuẩn** của tất cả messages giữa client và server:

```
┌─────────────────────────────────────────────────────────────┐
│  STANDARD PACKET FORMAT                                     │
├─────────────────────────────────────────────────────────────┤
│  1. HEADER (8 bytes)                                        │
│     - cmd: định danh lệnh (authentication, chat, game, etc) │
│     - user_id: người gửi (0 nếu chưa login)                │
│     - length: kích thước payload                            │
│                                                              │
│  2. PAYLOAD (variable)                                      │
│     - JSON string chứa dữ liệu thực tế                      │
│     - Flexible size, size được định nghĩa trong header      │
└─────────────────────────────────────────────────────────────┘
```

### **B. PHÂN TÁCH LOGIC NETWORK VÀ BUSINESS LOGIC**

**Packet** cho phép tách biệt rõ ràng:

```
┌──────────────────────────────────────────────────────────────┐
│                    NETWORK LAYER                             │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Receive raw bytes → Parse PacketHeader              │   │
│  │ Read cmd, user_id, length from bytes                │   │
│  │ Allocate buffer & read payload                      │   │
│  │ Convert from network byte order (ntohs/ntohl)       │   │
│  └──────────────────────────────────────────────────────┘   │
└───────────────────────┬──────────────────────────────────────┘
                        │ Packet header + payload
                        ▼
┌──────────────────────────────────────────────────────────────┐
│                  DISPATCHER LAYER                            │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Route by cmd (0x01xx = Auth, 0x02xx = Friends, etc) │   │
│  │ Extract JSON from payload                           │   │
│  │ Call appropriate handler                            │   │
│  └──────────────────────────────────────────────────────┘   │
└───────────────────────┬──────────────────────────────────────┘
                        │ cmd, payload_json
                        ▼
┌──────────────────────────────────────────────────────────────┐
│                  BUSINESS LOGIC LAYER                        │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Parse JSON: extract username, password, etc         │   │
│  │ Validate data                                       │   │
│  │ Call DAO layer (database operations)                │   │
│  │ Generate response                                   │   │
│  └──────────────────────────────────────────────────────┘   │
└──────────────────────────────────────────────────────────────┘
```

---

## **4️⃣ LUỒNG HOẠT ĐỘNG (MESSAGE FLOW)**

### **CLIENT SIDE - Gửi Request**

```c
// Qt Client sends login request

// 1. Build packet manually (on client side)
PacketHeader hdr;
hdr.cmd = htons(0x0103);              // CMD_REQ_LOGIN
hdr.user_id = htons(0);               // Not logged in yet
hdr.length = htonl(37);               // 37 bytes payload

const char *json = "{\"username\":\"alice\",\"password\":\"secret123\"}";

// 2. Send header
socket.write((char*)&hdr, sizeof(hdr));

// 3. Send payload
socket.write(json, 37);

// Bytes sent over TCP:
// [01 03 00 00 00 00 00 25] [JSON bytes...]
```

### **SERVER SIDE - Nhận & Xử Lý Request**

```c
// server.c - main accept loop

// Step 1: Accept connection
int fd = accept(sockfd, ...);
ClientSession *sess = client_session_new(fd);

// Step 2: Read PacketHeader (8 bytes)
PacketHeader hdr;
ssize_t n = recv(fd, &hdr, sizeof(hdr), MSG_WAITALL);
// Returns exactly 8 bytes or fails

// Step 3: Parse header
uint16_t cmd = ntohs(hdr.cmd);              // 0x0103
uint16_t user_id = ntohs(hdr.user_id);     // 0
uint32_t len = ntohl(hdr.length);          // 37

// Step 4: Read payload (variable length)
char *payload = malloc(len + 1);
ssize_t r = recv(fd, payload, len, MSG_WAITALL);
// Returns exactly 37 bytes or fails
payload[len] = '\0';  // Null-terminate for safety

// Step 5: Dispatch to handler
dispatcher_handle_packet(sess, cmd, payload, len);

// Step 6: Free resources
if (payload) free(payload);
close(fd);
client_session_free(sess);
```

---

## **5️⃣ VÍ DỤ THỰC TẾ: LOGIN FLOW**

### **REQUEST (Client → Server)**

```
┌──────────────────────────────────────┐
│ Qt Client User Interface             │
│ - Input username: "alice"            │
│ - Input password: "secret123"        │
│ - Click Login button                 │
└────────────────────┬─────────────────┘
                     │
                     ▼
         Create Packet:
         cmd = 0x0103 (REQ_LOGIN)
         user_id = 0
         length = 37
         payload = {"username":"alice","password":"secret123"}
                     │
                     ▼
    ┌─────────────────────────────────────────────┐
    │  PACKET SENT OVER TCP:                      │
    ├─────────────────────────────────────────────┤
    │ Bytes 0-1:   01 03  (cmd in network order)  │
    │ Bytes 2-3:   00 00  (user_id=0)             │
    │ Bytes 4-7:   00 00 00 25  (length=37)       │
    │ Bytes 8-44:  JSON payload (37 bytes)        │
    │                                             │
    │ Total: 45 bytes                             │
    └─────────────────────────────────────────────┘
```

### **SERVER PROCESSING**

```
1. server.c receive() reads 8 bytes → PacketHeader
   ┌──────────────────┐
   │ cmd=0x0103       │
   │ user_id=0        │
   │ length=37        │
   └──────────────────┘

2. server.c receive() reads 37 bytes → payload
   payload = "{\"username\":\"alice\",\"password\":\"secret123\"}"

3. dispatcher_handle_packet(sess, 0x0103, payload, 37)
   - Extract major category: 0x0103 & 0xFF00 = 0x01 (Auth)
   - Call auth_dispatch()

4. auth_dispatch() handles CMD_REQ_LOGIN
   - util_json_get_string(payload, "username") → "alice"
   - util_json_get_string(payload, "password") → "secret123"
   - Call auth_login()

5. auth_login() → dao_users_check_password()
   - Query database: SELECT * FROM users WHERE username='alice'
   - Verify password hash
   - Success! Generate session token

6. Build response Packet
   cmd = 0x0104 (RES_LOGIN)
   user_id = 1 (alice's id)
   length = 50
   payload = "{\"token\": \"abc123xyz...\"}"

7. protocol_send_response() sends response
   - Build PacketHeader with htons/htonl
   - Send 8 bytes header + 50 bytes payload
```

### **RESPONSE (Server → Client)**

```
    ┌─────────────────────────────────────────────┐
    │  PACKET SENT OVER TCP:                      │
    ├─────────────────────────────────────────────┤
    │ Bytes 0-1:   01 04  (cmd=RES_LOGIN)         │
    │ Bytes 2-3:   00 01  (user_id=1)             │
    │ Bytes 4-7:   00 00 00 32  (length=50)       │
    │ Bytes 8-57:  JSON payload (50 bytes)        │
    │                                             │
    │ Total: 58 bytes                             │
    └─────────────────────────────────────────────┘
                     │
                     ▼
    Qt Client receives & parses
    - Read 8 bytes header
    - cmd=0x0104, user_id=1, len=50
    - Read 50 bytes payload
    - Parse JSON: {"token": "abc123xyz..."}
    - Store token locally
                     │
                     ▼
    ┌──────────────────────────────────────────┐
    │ Qt Client User Interface                 │
    │ - Login successful!                      │
    │ - Navigate to Lobby                      │
    │ - Store token for future requests        │
    └──────────────────────────────────────────┘
```

---

## **6️⃣ TẠI SAO CẦN STRUCT PACKET?**

### **1. Standardization (Chuẩn Hóa)**

```
Mà không Packet struct:
- Client không biết payload size trước khi nhận
- Server không biết khi nào payload kết thúc
- Dễ gặp lỗi parse

Với Packet struct:
- Header định nghĩa size trong `length` field
- Server biết chính xác bao nhiêu bytes để đọc
- Clear protocol definition
```

### **2. Flexibility (Linh Hoạt)**

```c
// Packet cho phép payload size bất kỳ:

LOGIN request:
PacketHeader (8 bytes) + JSON (37 bytes) = 45 bytes total

LEADERBOARD response:
PacketHeader (8 bytes) + JSON (10KB) = 10,008 bytes total

Game room creation:
PacketHeader (8 bytes) + JSON (200 bytes) = 208 bytes total

// Tất cả dùng chung struct, chỉ khác content
```

### **3. Network Byte Order Handling**

```c
// PacketHeader chứa các trường uint16_t, uint32_t
// Cần convert between host byte order (CPU) và network byte order (TCP)

On client (sending):
hdr.cmd = htons(0x0103);        // uint16_t 0x0103 → network bytes

On server (receiving):
uint16_t cmd = ntohs(hdr.cmd);  // network bytes → uint16_t 0x0103

// Struct giúp maintain consistency
```

### **4. Type Safety**

```c
// Struct định nghĩa rõ từng field:

struct {
    uint16_t cmd;        // ← Must be 2 bytes
    uint16_t user_id;    // ← Must be 2 bytes
    uint32_t length;     // ← Must be 4 bytes
}

// Compiler enforces: sizeof(PacketHeader) = 8 bytes
// If someone changes type, compile error → type safety
```

---

## **7️⃣ FLEXIBLE ARRAY MEMBER - PAYLOAD**

### **Tại sao dùng `char payload[]`?**

```c
typedef struct {
    PacketHeader header;
    char payload[];      // ← Flexible Array Member (C99)
} Packet;
```

**Lợi ích:**

```
❌ KHÔNG LINH HOẠT:
struct Packet_v1 {
    PacketHeader header;
    char payload[1024];  // ← Fixed 1024 bytes
};
// Lãng phí bộ nhớ cho small messages
// Không đủ cho large messages

✅ LINH HỌP:
struct Packet_v2 {
    PacketHeader header;
    char payload[];      // ← Size determined at runtime
};
// Chỉ allocate bao nhiêu cần thiết:
char *buf = malloc(8 + payload_size);
Packet *p = (Packet*)buf;
// payload[0..payload_size-1] accessible
```

**Cách sử dụng:**

```c
// Server receives:
uint32_t len = ntohl(hdr.length);  // e.g., 37 bytes

char *buf = malloc(sizeof(PacketHeader) + len);
// buf points to: [8 bytes header][37 bytes payload]

PacketHeader *hdr = (PacketHeader*)buf;
// hdr->payload points to buf + 8 (offset by header size)

// Access payload:
char *payload = buf + sizeof(PacketHeader);
// or
const char *payload_from_packet = hdr->payload;  // Same memory location
```

---

## **8️⃣ PROTOCOL VERSION & FUTURE EXTENSIBILITY**

### **Current (Simple)**

```
PacketHeader (8 bytes):
- cmd (2 bytes)
- user_id (2 bytes)
- length (4 bytes)
```

### **Future (With Version & Flags)**

```c
typedef struct {
    uint8_t version;     // Protocol version (e.g., 1, 2, 3)
    uint8_t flags;       // Compression, encryption, etc.
    uint16_t cmd;        // Command type
    uint16_t user_id;    // User ID or reserved
    uint32_t length;     // Payload length
} PacketHeader_v2;  // 10 bytes

// Benefit: Can add features without breaking old clients
// Old client: uses version 1
// New client: uses version 2 with compression
```

---

## **9️⃣ SECURITY IMPLICATIONS**

### **Validation Checks**

```c
// server.c should validate:

if (len > MAX_PAYLOAD_SIZE) {
    // Reject oversized packet (prevent DoS)
    close(fd);
    return;
}

if (len > 0 && !payload) {
    // Reject if header says payload but allocation failed
    close(fd);
    return;
}

// Packet struct helps maintain these invariants
```

### **Null Termination**

```c
payload[len] = '\0';  // Safe to use as C string
// Important for JSON parsing with string functions
```

---

## **🔟 SUMMARY TABLE**

| Aspect | Role | Example |
|--------|------|---------|
| **Definition** | Defines message format | 8-byte header + variable payload |
| **Standardization** | Both client & server follow same protocol | All messages are Packets |
| **Network Byte Order** | Handles endianness conversion | htons(), htonl(), ntohs(), ntohl() |
| **Flexibility** | Supports variable-size payloads | 37 bytes for login, 10KB for leaderboard |
| **Parsing** | Tells server how much data to read | `length` field specifies payload size |
| **Type Safety** | Compiler ensures correct layout | sizeof(PacketHeader) = 8 bytes |
| **Extensibility** | Can add fields for future features | Version/flags for future versions |
| **Error Handling** | Validates packet integrity | Check len > 0, allocate correctly |

---

## **📊 PACKET LIFECYCLE**

```
CLIENT SIDE
┌────────────────────────────────────┐
│ 1. Build Packet                    │
│    - Set cmd, user_id, length      │
│    - Create JSON payload           │
└─────────────┬──────────────────────┘
              │
              ▼
┌────────────────────────────────────┐
│ 2. Convert to Network Byte Order   │
│    - htons(cmd)                    │
│    - htonl(length)                 │
└─────────────┬──────────────────────┘
              │
              ▼
┌────────────────────────────────────┐
│ 3. Send over TCP Socket            │
│    - Send header (8 bytes)         │
│    - Send payload (length bytes)   │
└─────────────┬──────────────────────┘
              │
         [Network]
              │
              ▼
SERVER SIDE
┌────────────────────────────────────┐
│ 1. Receive PacketHeader (8 bytes)  │
│    - MSG_WAITALL ensures 8 bytes   │
└─────────────┬──────────────────────┘
              │
              ▼
┌────────────────────────────────────┐
│ 2. Convert from Network Byte Order │
│    - ntohs(cmd)                    │
│    - ntohl(length)                 │
└─────────────┬──────────────────────┘
              │
              ▼
┌────────────────────────────────────┐
│ 3. Allocate Payload Buffer         │
│    - malloc(length + 1)            │
│    - recv(fd, payload, length)     │
│    - Null terminate                │
└─────────────┬──────────────────────┘
              │
              ▼
┌────────────────────────────────────┐
│ 4. Dispatch Handler                │
│    - dispatcher_handle_packet()    │
│    - Route by cmd & payload        │
└─────────────┬──────────────────────┘
              │
              ▼
┌────────────────────────────────────┐
│ 5. Business Logic                  │
│    - Parse JSON                    │
│    - Process request               │
│    - Generate response             │
└─────────────┬──────────────────────┘
              │
              ▼
┌────────────────────────────────────┐
│ 6. Send Response Packet            │
│    - protocol_send_response()      │
│    - Build response Packet header  │
│    - Send to client                │
└────────────────────────────────────┘
```

