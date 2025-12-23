# 📊 **USECASE FLOWS - HORIZONTAL VERSION**

## **1️⃣ USER REGISTRATION FLOW**

```
┌──────────────┐   REQ_REGISTER    ┌──────────────┐   cmd=0x01    ┌───────────────┐   auth_signup  ┌──────────────┐   dao_users_create  ┌──────────────────┐
│  Qt Client   │────────────────→  │ server.c     │────────────→  │dispatcher.c   │──────────────→ │auth_service  │─────────────────→  │ dao_users.c      │
│              │ payload:{username │ (Listener)   │               │(Dispatcher)   │               │              │ Extract JSON:      │                  │
│ Register     │  password}        │              │               │               │               │              │ - username="alice" │ - Hash password  │
│ Form         │                   │ - Receive    │               │ - Route by    │               │ - Call       │ - password="xxx"   │ - INSERT users   │
└──────────────┘                   │   packet     │               │   category    │               │   auth_      │                    │ - Returns user_id│
                                   │ - Parse hdr  │               │   (0x01)      │               │   signup()   │                    └──────────────────┘
                                   │ - Dispatch   │               │ - Route to    │               │              │
                                   └──────────────┘               │   auth_svc    │               │              │
                                                                  └───────────────┘               └──────────────┘
                                                                                                         │
                                                         ┌────────────────────────────────────────────┼───────────────────────────┐
                                                         │                                            │                           │
                                                         ▼                                            ▼                           ▼
                                               ✓ User created               ✗ Username exists   ✗ Database error
                                               │                           │                    │
                                               └─→ protocol_send_simple_ok(cmd=0x0102)
                                                                           └─→ protocol_send_error("USERNAME_EXISTS")
                                                                                          │
                                                                                          ▼
┌──────────────┐                               ┌──────────────────────────────────────────────┐
│  Qt Client   │◄────────────────────────────  │ TCP Response (cmd=0x0102)                    │
│              │ cmd=0x0102 RES_REGISTER       │ - Success: empty payload                     │
│ Show Result  │                               │ - Error: {"error": "USERNAME_EXISTS"}         │
└──────────────┘                               └──────────────────────────────────────────────┘
```

---

## **2️⃣ USER LOGIN FLOW**

```
┌──────────────┐   REQ_LOGIN    ┌──────────────┐   cmd=0x01    ┌───────────────┐   auth_login    ┌──────────────┐   dao_users   ┌──────────────────┐
│  Qt Client   │───────────────→ │ server.c     │──────────────→ │dispatcher.c   │───────────────→ │auth_service  │──────────→  │ dao_users.c      │
│              │ payload:{u,p}   │ (Listener)   │               │(Dispatcher)   │                │              │  password()  │                  │
│ Login Form   │                 │              │               │               │                │              │             │ - SELECT user    │
│              │                 │ - Receive    │               │ - Route by    │                │ - Extract    │             │ - Verify hash    │
└──────────────┘                 │   packet     │               │   category    │                │   JSON:u,p   │             │ - Return user_id │
                                 │ - Parse hdr  │               │   (0x01)      │                │              │             └──────────────────┘
                                 │ - Dispatch   │               │ - Route to    │                │              │
                                 └──────────────┘               │   auth_svc    │                │              │
                                                                └───────────────┘                └──────────────┘
                                                                                                        │
                                                        ┌─────────────────────────────────────────────┼──────────┐
                                                        │                                             │          │
                                                        ▼                                             ▼          ▼
                                                  ✓ Verify OK        ✗ Wrong password        ✗ DB error
                                                  │                  │                       │
                                                  └──→ dao_sessions_create()
                                                       - Generate token
                                                       - INSERT session
                                                       - Return token
                                                              │
                                                              ▼
                                                  Update sess->user_id
                                                  protocol_send_response()
                                                  {"token": "abc123xyz..."}
                                                              │
                                                              └─→ protocol_send_error()
                                                                  "INVALID_CREDENTIALS"

┌──────────────┐                                ┌────────────────────────────────────────┐
│  Qt Client   │◄───────────────────────────── │ TCP Response (cmd=0x0104)              │
│              │  cmd=0x0104 RES_LOGIN         │ - Success: {"token": "..."}            │
│ Store token  │  payload:{token}              │ - Error: {"error": "INVALID_..."}      │
│ Go to Lobby  │                               └────────────────────────────────────────┘
└──────────────┘
```

---

## **3️⃣ ADD FRIEND FLOW**

```
┌──────────────┐  REQ_ADD_FRIEND  ┌──────────────┐   cmd=0x02    ┌───────────────┐   Extract JSON  ┌────────────────────┐   dao_friends_      ┌─────────────┐
│  Qt Client   │─────────────────→ │ server.c     │──────────────→ │dispatcher.c   │────────────────→ │friend_id = 2       │ send_request()    │ PostgreSQL  │
│(user_id=1)   │payload:{friend_id│ (Listener)   │               │ Friends(0x02) │                │ (bob's id)         │                    │             │
│ Search "bob" │  2}              │              │               │               │                └────────────────────┘   INSERT INTO       │ friend_    │
│ Add Friend   │                  │ - Receive    │               │ - Route to    │                        │                   friend_requests  │ requests  │
└──────────────┘                  │   packet     │               │   REQ_ADD_    │                        ▼                   (1, 2, pending)   │           │
                                  │ - Parse      │               │   FRIEND hdlr │            ✓ Request sent               └─────────────────┘
                                  │ - Dispatch   │               │               │            │
                                  └──────────────┘               └───────────────┘            ▼
                                                                                    ✗ Error (user not found)
                                                                                    │
                                                                                    └─→ protocol_send_error()

┌──────────────┐                  ┌────────────────────────────────────────┐
│  Qt Client   │◄───────────────── │ TCP Response (cmd=0x0202)              │
│              │  cmd=0x0202       │ - Success: empty payload               │
│ Show Result  │  RES_ADD_FRIEND   │ - Error: {"error": "..."}              │
└──────────────┘                  └────────────────────────────────────────┘
```

---

## **4️⃣ RESPOND TO FRIEND REQUEST FLOW**

```
┌──────────────┐  REQ_RESPOND     ┌──────────────┐   cmd=0x02    ┌───────────────┐   Extract JSON  ┌────────────────────┐   dao_friends_       ┌─────────────┐
│  Qt Client   │─────────────────→ │ server.c     │──────────────→ │dispatcher.c   │────────────────→ │from_user=1         │ respond_request()  │ PostgreSQL  │
│(user_id=2)   │payload:{from_user│ (Listener)   │               │ Friends(0x02) │                │ accept=1           │                    │             │
│ Accept Req   │ accept}          │              │               │               │                └────────────────────┘   UPDATE request    │ friends    │
│ from alice   │                  │ - Receive    │               │ - Route to    │                        │                   status='accepted' │ table      │
└──────────────┘                  │   packet     │               │   REQ_RESPOND │                        ▼                   INSERT friends     │            │
                                  │ - Parse      │               │   hdlr        │            ✓ Request accepted               (1,2) and (2,1)   │            │
                                  │ - Dispatch   │               │               │            │                       └─────────────────┘
                                  └──────────────┘               └───────────────┘            ▼
                                                                                    ✗ Error (invalid request)
                                                                                    │
                                                                                    └─→ protocol_send_error()

┌──────────────┐                  ┌────────────────────────────────────────┐
│  Qt Client   │◄───────────────── │ TCP Response (cmd=0x0205)              │
│              │  cmd=0x0205       │ - Success: empty payload               │
│ Update UI    │  RES_RESPOND      │ - Error: {"error": "..."}              │
└──────────────┘                  └────────────────────────────────────────┘
```

---

## **5️⃣ LIST FRIENDS FLOW**

```
┌──────────────┐  REQ_LIST_      ┌──────────────┐   cmd=0x02    ┌───────────────┐   Route to      ┌────────────────────┐   dao_friends_    ┌─────────────┐
│  Qt Client   │  FRIENDS        │ server.c     │──────────────→ │dispatcher.c   │────────────────→ │handler             │ list(user_id=1)  │ PostgreSQL  │
│(user_id=1)   │────────────────→ │ (Listener)   │               │ Friends(0x02) │                └────────────────────┘                    │             │
│ View Friends │ (empty payload) │              │               │               │                        │                   SELECT * FROM    │ friends    │
│              │                 │ - Receive    │               │ - Route to    │                        ▼                   friends WHERE    │ table      │
└──────────────┘                 │   packet     │               │   REQ_LIST_   │            Build JSON array:             user_id_1=1      │            │
                                 │ - Parse      │               │   FRIENDS hdlr│            [{friend1}, {friend2}, ...]  JOIN users table  │            │
                                 │ - Dispatch   │               │               │                                         └─────────────────┘
                                 └──────────────┘               └───────────────┘
                                                                                                    │
                                                                                                    ▼
                                                                                    protocol_send_response()
                                                                                    Sends JSON friend list

┌──────────────┐                 ┌────────────────────────────────────────┐
│  Qt Client   │◄───────────────── │ TCP Response (cmd=0x0207)              │
│              │  cmd=0x0207       │ - Success: [friend JSON array]         │
│ Display      │  RES_LIST_        │ - Error: {"error": "..."}              │
│ Friend List  │  FRIENDS          └────────────────────────────────────────┘
└──────────────┘
```

---

## **6️⃣ SEND DIRECT MESSAGE FLOW**

```
┌──────────────┐  REQ_SEND_DM    ┌──────────────┐   cmd=0x03    ┌───────────────┐   Extract JSON  ┌────────────────────┐   dao_chat_      ┌─────────────┐
│  Qt Client   │─────────────────→ │ server.c     │──────────────→ │dispatcher.c   │────────────────→ │to_user=2           │ send_dm()        │ PostgreSQL  │
│(user_id=1)   │payload:{to_user   │ (Listener)   │               │  Chat(0x03)   │                │ content="Hello"    │                  │             │
│ Chat with    │ content}          │              │               │               │                └────────────────────┘   INSERT INTO    │ messages  │
│ bob (id=2)   │                   │ - Receive    │               │ - Route to    │                        │                   messages table  │ table      │
└──────────────┘                   │   packet     │               │   REQ_SEND_DM │                        ▼                   (1, 2, content) │            │
                                   │ - Parse      │               │   handler     │            ✓ Message stored              │ read=false      │            │
                                   │ - Dispatch   │               │               │            │                       └─────────────────┘
                                   └──────────────┘               └───────────────┘            ├→ TODO: NOTIFY_DM
                                                                                               │     if bob online
                                                                                               ▼
                                                                                    ✗ Error (bob not found)

┌──────────────┐                  ┌────────────────────────────────────────┐
│  Qt Client   │◄───────────────── │ TCP Response (cmd=0x0302)              │
│              │  cmd=0x0302       │ - Success: empty payload               │
│ Message sent │  RES_SEND_DM      │ - Error: {"error": "..."}              │
└──────────────┘                  └────────────────────────────────────────┘
```

---

## **7️⃣ CREATE GAME ROOM FLOW**

```
┌──────────────┐  REQ_CREATE_   ┌──────────────┐   cmd=0x04    ┌───────────────┐   Route to      ┌────────────────────┐   dao_rooms_    ┌──────────────┐
│  Qt Client   │  ROOM          │ server.c     │──────────────→ │dispatcher.c   │────────────────→ │handler             │ create()        │ PostgreSQL  │
│(user_id=1)   │───────────────→ │ (Listener)   │               │  Room(0x04)   │                └────────────────────┘                 │             │
│ Lobby Page   │ (empty payload)│              │               │               │                        │                   INSERT INTO    │ rooms table │
│ Click:       │                 │ - Receive    │               │ - Route to    │                        ▼                   rooms          │             │
│ "Create Room"│                 │   packet     │               │   REQ_CREATE_ │            ✓ Room created              (host_id=1)    │             │
└──────────────┘                 │ - Parse      │               │   ROOM hdlr   │            │                       INSERT INTO    │             │
                                 │ - Dispatch   │               │               │            │                       room_members   │             │
                                 └──────────────┘               └───────────────┘            │                       (room_id, user_1) 
                                                                                             ▼                       └──────────────┘
                                                                                    Build response JSON:
                                                                                    {"room_id": 10}
                                                                                    │
                                                                                    ▼
                                                                                    protocol_send_response()

┌──────────────┐                 ┌────────────────────────────────────────┐
│  Qt Client   │◄───────────────── │ TCP Response (cmd=0x0402)              │
│              │  cmd=0x0402       │ - Success: {"room_id": 10}             │
│ Store room   │  RES_CREATE_      │ - Error: {"error": "..."}              │
│ Go to Room   │  ROOM             └────────────────────────────────────────┘
└──────────────┘
```

---

## **8️⃣ JOIN GAME ROOM FLOW**

```
┌──────────────┐  REQ_JOIN_    ┌──────────────┐   cmd=0x04    ┌───────────────┐   Extract JSON  ┌────────────────────┐   dao_rooms_   ┌──────────────┐
│  Qt Client   │  ROOM         │ server.c     │──────────────→ │dispatcher.c   │────────────────→ │room_id=10          │ join()         │ PostgreSQL  │
│(user_id=2)   │──────────────→ │ (Listener)   │               │  Room(0x04)   │                └────────────────────┘                │             │
│ Lobby Page   │payload:{room_id│              │               │               │                        │                   INSERT INTO    │ room_members
│ See Room #10 │ 10}           │ - Receive    │               │ - Route to    │                        ▼                   room_members   │ table       │
│ Click: "Join"│                 │   packet     │               │   REQ_JOIN_   │            ✓ Joined room              (10, user_2)   │             │
└──────────────┘                 │ - Parse      │               │   ROOM hdlr   │            │                       TODO: Broadcast │             │
                                 │ - Dispatch   │               │               │            │                       NOTIFY_ROOM_   │             │
                                 └──────────────┘               └───────────────┘            │                       UPDATE          │             │
                                                                                             ▼                       └──────────────┘
                                                                                    ✗ Error (room full)

┌──────────────┐                 ┌────────────────────────────────────────┐
│  Qt Client   │◄───────────────── │ TCP Response (cmd=0x0404)              │
│              │  cmd=0x0404       │ - Success: empty payload               │
│ Join Room    │  RES_JOIN_ROOM    │ - Error: {"error": "ROOM_FULL"}        │
└──────────────┘                  └────────────────────────────────────────┘
```

---

## **9️⃣ GET USER PROFILE FLOW**

```
┌──────────────┐  REQ_GET_    ┌──────────────┐   cmd=0x07    ┌───────────────┐   Route to      ┌────────────────────┐   dao_stats_   ┌──────────────┐
│  Qt Client   │  PROFILE     │ server.c     │──────────────→ │dispatcher.c   │────────────────→ │handler             │ get_profile()  │ PostgreSQL  │
│              │─────────────→ │ (Listener)   │               │  Stats(0x07)  │                └────────────────────┘                │             │
│ Click user   │ (empty payload│              │               │               │                        │                   SELECT from    │ users +    │
│ "alice"      │ or user_id)  │ - Receive    │               │ - Route to    │                        ▼                   users,         │ stats      │
└──────────────┘                 │   packet     │               │   REQ_GET_    │            Build JSON profile:             user_stats  │ tables     │
                                 │ - Parse      │               │   PROFILE hdlr│            {username, wins,             JOIN         │             │
                                 │ - Dispatch   │               │               │             losses, score...}             └──────────────┘
                                 └──────────────┘               └───────────────┘
                                                                                                    │
                                                                                                    ▼
                                                                                    protocol_send_response()
                                                                                    Sends profile JSON

┌──────────────┐                 ┌────────────────────────────────────────┐
│  Qt Client   │◄───────────────── │ TCP Response (cmd=0x0702)              │
│              │  cmd=0x0702       │ - Success: {profile JSON}              │
│ Display      │  RES_GET_         │ - Error: {"error": "..."}              │
│ User Stats   │  PROFILE          └────────────────────────────────────────┘
└──────────────┘
```

---

## **🔟 GET LEADERBOARD FLOW**

```
┌──────────────┐  REQ_         ┌──────────────┐   cmd=0x07    ┌───────────────┐   Route to      ┌────────────────────┐   dao_stats_     ┌──────────────┐
│  Qt Client   │  LEADERBOARD  │ server.c     │──────────────→ │dispatcher.c   │────────────────→ │handler             │ get_leaderboard()│ PostgreSQL  │
│              │──────────────→ │ (Listener)   │               │  Stats(0x07)  │                └────────────────────┘                 │             │
│ Click:       │ (empty payload│              │               │               │                        │                   SELECT TOP 20  │ user_stats │
│ "Leaderboard"│ or limit=20)  │ - Receive    │               │ - Route to    │                        ▼                   ORDER BY wins  │ table      │
└──────────────┘                 │   packet     │               │   REQ_LEADER  │            Build JSON array:             DESC            │             │
                                 │ - Parse      │               │   BOARD hdlr  │            [{rank1}, {rank2}, ...]      JOIN users     │             │
                                 │ - Dispatch   │               │               │                                         └──────────────┘
                                 └──────────────┘               └───────────────┘
                                                                                                    │
                                                                                                    ▼
                                                                                    protocol_send_response()
                                                                                    Sends leaderboard JSON

┌──────────────┐                 ┌────────────────────────────────────────┐
│  Qt Client   │◄───────────────── │ TCP Response (cmd=0x0704)              │
│              │  cmd=0x0704       │ - Success: [rank JSON array]           │
│ Display Top  │  RES_LEADERBOARD  │ - Error: {"error": "..."}              │
│ 20 Rankings  │                   └────────────────────────────────────────┘
└──────────────┘
```

---

## **📋 SUMMARY TABLE**

| Usecase | Cmd | Status | Key Components |
|---------|-----|--------|-----------------|
| 1. Register | 0x0101/02 | ✅ | `auth_dispatch()` → `auth_signup()` → `dao_users_create()` |
| 2. Login | 0x0103/04 | ✅ | `auth_dispatch()` → `auth_login()` → `dao_users_check_password()` → `dao_sessions_create()` |
| 3. Add Friend | 0x0201/02 | ✅ | `dispatcher` (0x02) → `dao_friends_send_request()` |
| 4. Respond Friend | 0x0204/05 | ✅ | `dispatcher` (0x02) → `dao_friends_respond_request()` |
| 5. List Friends | 0x0206/07 | ✅ | `dispatcher` (0x02) → `dao_friends_list()` |
| 6. Send DM | 0x0301/02 | ✅ | `dispatcher` (0x03) → `dao_chat_send_dm()` |
| 7. Create Room | 0x0401/02 | ✅ | `dispatcher` (0x04) → `dao_rooms_create()` |
| 8. Join Room | 0x0403/04 | ✅ | `dispatcher` (0x04) → `dao_rooms_join()` |
| 9. Get Profile | 0x0701/02 | ✅ | `stats_handle_get_profile()` → `dao_stats_get_profile()` |
| 10. Leaderboard | 0x0703/04 | ✅ | `stats_handle_leaderboard()` → `dao_stats_get_leaderboard()` |

---

## **🎯 KEY POINTS**

1. **Horizontal Flow**: Client (left) → Server (middle) → Database (right)
2. **All flows follow same pattern**:
   - Client sends REQ with JSON payload
   - Dispatcher routes by command category
   - Service/DAO handles business logic
   - Protocol sends RES or error back
3. **Error handling**: Each flow can branch to error response
4. **Database updates**: Shown in each usecase's table changes

