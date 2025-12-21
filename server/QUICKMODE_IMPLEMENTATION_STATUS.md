# QuickMode Implementation Status

## ✅ Phase 2: Session Management - COMPLETED

### [x] Implement session linked list
- **Location**: `server/src/service/quickmode_service.c:18`
- **Implementation**: Static linked list `active_sessions` to manage all active QuickMode sessions
```c
static QuickModeSession *active_sessions = NULL;
```

### [x] Implement quickmode_session_new()
- **Location**: `server/src/service/quickmode_service.c:24-52`
- **Features**:
  - Creates new QuickModeSession
  - Generates unique session_id using timestamp + random
  - Initializes all game state (round, score, lifeline counters)
  - Adds session to linked list
  - Returns NULL on failure

### [x] Implement quickmode_session_free()
- **Location**: `server/src/service/quickmode_service.c:54-71`
- **Features**:
  - Removes session from linked list
  - Handles edge cases (first node, middle node)
  - Frees session memory

### [x] Implement quickmode_session_find()
- **Location**: `server/src/service/quickmode_service.c:73-80`
- **Features**:
  - Finds session by user_id
  - Returns NULL if not found
  - Also implemented `quickmode_session_find_by_id()` for session_id lookup

---

## ✅ Phase 3: Game Logic - COMPLETED

### [x] Implement quickmode_handle_start()
- **Location**: `server/src/service/quickmode_service.c:129-166`
- **Features**:
  - Validates user is logged in
  - Checks for existing active session
  - Creates new session
  - Loads 15 questions (5 EASY, 5 MEDIUM, 5 HARD)
  - Sends `CMD_NOTIFY_GAME_START` with session_id

### [x] Implement question loading (5 EASY, 5 MEDIUM, 5 HARD)
- **Location**: `server/src/service/quickmode_service.c:95-123`
- **Function**: `quickmode_load_questions()`
- **Features**:
  - Loads 5 EASY questions (indices 0-4)
  - Loads 5 MEDIUM questions (indices 5-9)
  - Loads 5 HARD questions (indices 10-14)
  - Uses `dao_question_get_random()` for each question
  - Returns -1 on failure

### [x] Implement quickmode_handle_get_question()
- **Location**: `server/src/service/quickmode_service.c:168-251`
- **Features**:
  - Validates session_id and round (1-15)
  - Finds session by session_id
  - Validates round progression
  - Retrieves question for specified round
  - Escapes JSON strings properly
  - Sends `CMD_NOTIFY_QUESTION` with full question data

### [x] Implement quickmode_handle_submit_answer()
- **Location**: `server/src/service/quickmode_service.c:253-350`
- **Features**:
  - Validates session_id, round, and answer (A/B/C/D)
  - Finds session and validates ownership
  - Checks if round already answered
  - Compares answer with correct option
  - Updates score and game status
  - Sends `CMD_RES_SUBMIT_ANSWER` with result
  - Sends `CMD_NOTIFY_ANSWER_RESULT` notification
  - Handles game over condition

### [x] Implement quickmode_handle_game_over()
- **Location**: `server/src/service/quickmode_service.c:445-470`
- **Features**:
  - Calculates final score and status
  - Sends `CMD_NOTIFY_GAME_OVER` with final stats
  - Frees session (cleanup)

### [x] Implement quickmode_handle_use_lifeline() (BONUS)
- **Location**: `server/src/service/quickmode_service.c:352-443`
- **Features**:
  - Validates lifeline usage (max 2 times per session)
  - Validates round not already answered
  - Removes 2 wrong options randomly
  - Keeps 1 correct + 1 random wrong option
  - Sends `CMD_RES_USE_LIFELINE` with remaining/removed options

---

## ✅ Phase 4: Integration - COMPLETED

### [x] Implement quickmode_dispatch()
- **Location**: `server/src/service/quickmode_service.c:487-516`
- **Features**:
  - Routes QuickMode commands to appropriate handlers
  - Handles:
    - `CMD_REQ_START_QUICKMODE` → `quickmode_handle_start()`
    - `CMD_REQ_GET_QUESTION` → `quickmode_handle_get_question()`
    - `CMD_REQ_SUBMIT_ANSWER` → `quickmode_handle_submit_answer()`
    - `CMD_REQ_USE_LIFELINE` → `quickmode_handle_use_lifeline()`
  - Sends error for unknown commands

### [x] Update dispatcher.c - Add QuickMode routing
- **Location**: `server/src/service/dispatcher.c:153-156`
- **Implementation**:
```c
case 0x05: // Basic Mode – QuickMode
case 0x06: // 1vN MODE – sau này bạn mở rộng thêm
    quickmode_dispatch(sess, cmd, payload, payload_len);
    break;
```

### [x] Cleanup on disconnect
- **Location**: `server/src/service/server.c:176-178`
- **Implementation**:
```c
if (sess && sess->user_id > 0) {
    quickmode_cleanup_user(sess->user_id);
}
```

---

## ✅ Phase 5: Testing - COMPLETED

### [x] Test start game
- **Test File**: `server/src/test/test_quickmode_network.c`
- **Function**: `test_start_game()`
- **Tests**:
  - Sends `CMD_REQ_START_QUICKMODE`
  - Receives `CMD_NOTIFY_GAME_START`
  - Validates session_id in response

### [x] Test get question
- **Test File**: `server/src/test/test_quickmode_network.c`
- **Function**: `test_get_question()`
- **Tests**:
  - Sends `CMD_REQ_GET_QUESTION` with session_id and round
  - Receives `CMD_NOTIFY_QUESTION`
  - Validates question data in response

### [x] Test submit answer (correct/wrong)
- **Test File**: `server/src/test/test_quickmode_network.c`
- **Function**: `test_submit_answer()`
- **Tests**:
  - Sends `CMD_REQ_SUBMIT_ANSWER` with session_id, round, and answer
  - Receives `CMD_RES_SUBMIT_ANSWER`
  - Receives `CMD_NOTIFY_ANSWER_RESULT`
  - Validates game over condition

### [x] Test game over
- **Test File**: `server/src/test/test_quickmode_network.c`
- **Function**: `test_game_over()`
- **Tests**:
  - Receives `CMD_NOTIFY_GAME_OVER` when game ends
  - Validates final stats in response

### [x] Test multiple clients
- **Test File**: `server/src/test/test_multiclient.c`
- **Features**:
  - Tests multiple concurrent connections
  - Each client can start and play QuickMode independently
  - Server handles multiple sessions simultaneously

---

## Additional Features Implemented

### Random Seed Initialization
- **Location**: `server/src/main.c:13`
- **Implementation**: `srand((unsigned int)time(NULL));`
- **Purpose**: Ensures proper randomness for session_id generation and lifeline selection

### Error Handling
- All handlers validate inputs
- Proper error responses sent via `protocol_send_error()`
- Session cleanup on errors

### JSON Escaping
- All user-facing strings are properly escaped using `util_json_escape()`
- Prevents JSON injection and parsing errors

---

## File Structure

```
server/
├── include/service/
│   ├── quickmode_service.h      # QuickModeSession struct and function declarations
│   └── commands.h               # Command definitions (updated with QuickMode commands)
├── src/service/
│   ├── quickmode_service.c       # Full QuickMode implementation
│   ├── dispatcher.c              # Routes 0x05xx commands to quickmode_dispatch()
│   └── server.c                  # Cleanup on disconnect
└── src/test/
    ├── test_quickmode.c          # DAO-level test (existing)
    └── test_quickmode_network.c  # Network protocol test (NEW)
```

---

## How to Test

### 1. Start Server
```bash
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"
export SERVER_MODE=1
./build/server
```

### 2. Test Network Protocol
```bash
# Build test
make build/test_quickmode_network

# Run test
./build/test_quickmode_network localhost 9000 testuser testpass
```

### 3. Test Multiple Clients
```bash
# Build test
make build/test_multiclient

# Run test (5 concurrent clients)
./build/test_multiclient 5 localhost 9000
```

---

## Command Flow

### Start Game
1. Client → Server: `CMD_REQ_START_QUICKMODE` (payload: `{}`)
2. Server → Client: `CMD_NOTIFY_GAME_START` (payload: `{"session_id": 123, "total_rounds": 15}`)

### Get Question
1. Client → Server: `CMD_REQ_GET_QUESTION` (payload: `{"session_id": 123, "round": 1}`)
2. Server → Client: `CMD_NOTIFY_QUESTION` (payload: `{"session_id": 123, "round": 1, "question_id": 456, "content": "...", "options": {...}}`)

### Submit Answer
1. Client → Server: `CMD_REQ_SUBMIT_ANSWER` (payload: `{"session_id": 123, "round": 1, "answer": "A"}`)
2. Server → Client: `CMD_RES_SUBMIT_ANSWER` (payload: `{"session_id": 123, "round": 1, "correct": true, "score": 1}`)
3. Server → Client: `CMD_NOTIFY_ANSWER_RESULT` (payload: `{"round": 1, "correct": true, "correct_answer": "A"}`)

### Game Over
1. Server → Client: `CMD_NOTIFY_GAME_OVER` (payload: `{"session_id": 123, "final_score": 15, "status": "won"}`)

### Use Lifeline (50-50)
1. Client → Server: `CMD_REQ_USE_LIFELINE` (payload: `{"session_id": 123, "round": 1}`)
2. Server → Client: `CMD_RES_USE_LIFELINE` (payload: `{"session_id": 123, "round": 1, "remaining_options": ["A", "C"], "removed_options": ["B", "D"]}`)

---

## Status: ✅ ALL PHASES COMPLETED

All phases have been successfully implemented and tested. The QuickMode game service is fully functional and ready for production use.
