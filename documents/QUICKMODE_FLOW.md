# 🎮 **QUICKMODE USECASE FLOW**

## **1️⃣ OVERVIEW - QUICKMODE GAME MECHANICS**

### **Game Rules**
- **15 questions total**: 5 Easy, 5 Medium, 5 Hard
- **15 seconds per question**: Auto-end if timeout
- **1 wrong answer = Game Over**: Instant elimination
- **Win condition**: Answer all 15 correctly
- **Score tracked**: Total correct answers + response time

---

## **2️⃣ QUICKMODE START FLOW (Horizontal)**

```
┌──────────────┐  REQ_START_  ┌──────────────┐  cmd=0x05  ┌──────────────────┐  Extract JSON  ┌────────────────┐  dao_qm_create_  ┌─────────────────┐
│  Qt Client   │  QUICKMODE   │ server.c     │───────────→ │dispatcher.c      │────────────────→ │(empty payload) │ session()        │ PostgreSQL      │
│(user_id=1)   │─────────────→ │(Listener)    │            │ Quickmode(0x05)  │                │                │                  │                 │
│ Lobby Page   │(empty payload│              │            │                  │                └────────────────┘   INSERT INTO      │ quickmode_      │
│ Click:       │ or room_id)  │ - Receive    │            │ - Route to       │                       │                quickmode_       │ sessions table  │
│ "Quick Match"│              │   packet     │            │   quickmode      │                       ▼                sessions        │                 │
└──────────────┘              │ - Parse      │            │   dispatch()     │            ✓ Session created        (user_id, created_at)
                              │ - Dispatch   │            │                  │            │                     Returns: session_id
                              └──────────────┘            └──────────────────┘            ▼                     └─────────────────┘
                                                                                   protocol_send_response()
                                                                                   {"session_id": 100}

┌──────────────┐                                          ┌───────────────────────────────────────┐
│  Qt Client   │◄──────────────────────────────────────── │ TCP Response (cmd=0x0501?)            │
│              │ cmd=NOTIFY_GAME_START (0x0501)           │ - Success: {"session_id": 100}        │
│ Store        │ payload: {"session_id": 100}             │ - Error: {"error": "..."}             │
│ session_id   │                                          └───────────────────────────────────────┘
│ Start Game   │
└──────────────┘
```

---

## **3️⃣ QUESTION DELIVERY FLOW (Per Round)**

```
┌──────────────────────┐  REQ_GET_QUESTION  ┌──────────────┐  cmd=0x05  ┌──────────────────┐  Extract JSON  ┌────────────────┐  dao_question_    ┌──────────────┐
│ Qt Client            │  (Round N)         │ server.c     │───────────→ │dispatcher.c      │────────────────→ │round=N         │ get_random()     │ PostgreSQL   │
│(session_id=100)      │────────────────────→ │(Listener)    │            │ Quickmode(0x05)  │                │                │                  │              │
│ Game Screen          │payload:{session_id  │              │            │                  │                └────────────────┘  SELECT * FROM    │ questions    │
│ "Next question"      │ round=N}            │ - Receive    │            │ - Route to       │                       │                questions WHERE  │ table        │
└──────────────────────┘                     │   packet     │            │   quickmode      │                       ▼                difficulty=      │              │
                                             │ - Parse      │            │   dispatch()     │            ✓ Question fetched        (EASY|MEDIUM|HARD)
                                             │ - Dispatch   │            │                  │            │                     Random selection  │              │
                                             └──────────────┘            └──────────────────┘            │                     └──────────────┘
                                                                                                         ▼
                                                                                         dao_qm_create_round()
                                                                                         INSERT INTO quickmode_rounds
                                                                                         (session_id, round_number, question_id)
                                                                                         │
                                                                                         ▼
                                                                                         protocol_send_response()
                                                                                         Sends question JSON:
                                                                                         {
                                                                                           "round": 1,
                                                                                           "question": "What is AI?",
                                                                                           "options": ["A", "B", "C", "D"]
                                                                                         }

┌──────────────────────┐                                  ┌────────────────────────────────────────┐
│ Qt Client            │◄───────────────────────────────── │ TCP Response (cmd=NOTIFY_QUESTION?)   │
│                      │ payload: {question JSON}          │ - Round 1: Question + options        │
│ Display Question     │                                   │ - Question text                      │
│ Options: A B C D     │                                   │ - 4 answer choices                   │
│ Timer: 15 seconds    │                                   │ - Difficulty level (EASY|MEDIUM|...) │
│ waiting for input    │                                   └────────────────────────────────────────┘
└──────────────────────┘
```

---

## **4️⃣ ANSWER SUBMISSION FLOW (Per Round)**

```
┌──────────────────────┐  REQ_SUBMIT_ANSWER  ┌──────────────┐  cmd=0x05  ┌──────────────────┐  Extract JSON  ┌────────────────┐  Validate Answer   ┌──────────────┐
│ Qt Client            │  (User selected A)  │ server.c     │───────────→ │dispatcher.c      │────────────────→ │session_id=100  │                   │ PostgreSQL   │
│(session_id=100)      │────────────────────→ │(Listener)    │            │ Quickmode(0x05)  │                │ round=1        │ Compare with      │              │
│ Selected: A          │payload:{session_id  │              │            │                  │                │ answer="A"     │ correct_op from   │ quickmode_   │
│ Submitted at 12.5s   │ round=1             │ - Receive    │            │ REQ_SUBMIT_      │                │                │ question table    │ rounds       │
└──────────────────────┘  answer="A"}        │   packet     │            │ ANSWER handler   │                └────────────────┘ │                  │ table        │
                                             │ - Parse      │            │                  │                       │                UPDATE round      │              │
                                             │ - Dispatch   │            │                  │                       ▼                - Set answer      │              │
                                             └──────────────┘            └──────────────────┘            Comparison:             - Set response_time
                                                                                                         │                       │              │
                                                                              ┌────────────┬─────────────┴─┐
                                                                              ▼            ▼               ▼
                                                                        ✓ CORRECT    ✗ WRONG      ✗ TIMEOUT
                                                                        │            │            │
                                                    ┌─────────────────┼────────────┼────────────┤
                                                    │                 │            │
                                                    ▼                 ▼            ▼
                                    dao_qm_update_round()  dao_qm_end_session()  dao_qm_end_session()
                                    - answer="A"                                  
                                    - status="CORRECT"      UPDATE sessions      UPDATE sessions
                                    - Update stats          - status="FINISHED"  - status="FINISHED"
                                    - Increment score       - total_correct=3    - total_correct=1
                                    │                       - end_time=NOW()     - end_time=NOW()
                                    ▼                       │                    │
                                    protocol_send_response()protocol_send_error()protocol_send_error()
                                    RES_SUBMIT_ANSWER       GAME_OVER            TIMEOUT

┌──────────────────────┐                                  ┌────────────────────────────────────────┐
│ Qt Client            │◄───────────────────────────────── │ TCP Response                           │
│                      │ RES_SUBMIT_ANSWER                 │ - Success: {"correct": true, ...}     │
│ Show result          │                                   │ - Error: {"error": "WRONG_ANSWER"}     │
│ - ✓ Correct!         │                                   │ - Error: {"error": "TIMEOUT"}          │
│   Next question...   │                                   └────────────────────────────────────────┘
│ - ✗ Wrong! Game Over!│
└──────────────────────┘
```

---

## **5️⃣ GAME END & RESULTS FLOW**

```
┌──────────────┐  REQ_GET_RESULTS  ┌──────────────┐  cmd=0x05  ┌──────────────────┐  Extract JSON  ┌────────────────┐  dao_qm_get_session  ┌──────────────┐
│ Qt Client    │  (After 15 Qs     │ server.c     │───────────→ │dispatcher.c      │────────────────→ │session_id=100  │ _results()           │ PostgreSQL   │
│ Game Over    │  or wrong answer) │(Listener)    │            │ Quickmode(0x05)  │                │                │                      │              │
│ Click:       │────────────────────→ │              │            │                  │                └────────────────┘  SELECT:            │ quickmode_   │
│ "View        │(empty or          │ - Receive    │            │ - Route to       │                       │                - Total questions  │ sessions     │
│  Results"    │ session_id)        │   packet     │            │   quickmode      │                       ▼                - Correct answers   │ & rounds     │
└──────────────┘                    │ - Parse      │            │   dispatch()     │            ✓ Results fetched          - Wrong answer#     │ tables       │
                                    │ - Dispatch   │            │                  │            │                         - Response times   │              │
                                    └──────────────┘            └──────────────────┘            │                         └──────────────┘
                                                                                                ▼
                                                                                    dao_stats_record_match()
                                                                                    - INSERT INTO matches
                                                                                    - UPDATE user_stats
                                                                                    - Increment total_matches
                                                                                    - Update win_rate
                                                                                    - Update highest_score
                                                                                    │
                                                                                    ▼
                                                                                    protocol_send_response()
                                                                                    Results JSON:
                                                                                    {
                                                                                      "session_id": 100,
                                                                                      "total_questions": 15,
                                                                                      "correct_answers": 12,
                                                                                      "score": 85000,
                                                                                      "failed_at_round": 13,
                                                                                      "avg_response_time": 3.5,
                                                                                      "difficulty_progression": "EASY→MEDIUM→HARD"
                                                                                    }

┌──────────────┐                                  ┌────────────────────────────────────────┐
│ Qt Client    │◄───────────────────────────────── │ TCP Response (cmd=NOTIFY_GAME_OVER?)  │
│              │ payload: {results JSON}           │ - Game Over: {"correct": 12, ...}     │
│ Display:     │                                   │ - Final score calculated              │
│ - Score: 12/15 ✓ 80%                           │ - Achievements unlocked               │
│ - Leaderboard position updated                 │                                       │
│ - Option: Replay / Main Menu                   │                                       │
└──────────────┘                                  └────────────────────────────────────────┘
```

---

## **6️⃣ COMPLETE GAME SESSION SEQUENCE**

```
START
  │
  ├─→ Client requests quickmode start
  │   REQ_START_QUICKMODE (cmd=0x0501)
  │
  ├─→ Server creates session
  │   dao_qm_create_session() → session_id=100
  │
  └─→ Server sends NOTIFY_GAME_START
      RES_START_QUICKMODE (cmd=0x0501?)
      payload: {"session_id": 100, "total_rounds": 15}

ROUND 1
  │
  ├─→ Client requests question for round 1
  │   REQ_GET_QUESTION (cmd=0x0502?)
  │   payload: {"session_id": 100, "round": 1}
  │
  ├─→ Server fetches question
  │   dao_question_get_random("EASY")
  │   dao_qm_create_round()
  │
  ├─→ Server sends NOTIFY_QUESTION
  │   RES_GET_QUESTION (cmd=0x0502?)
  │   payload: {question details, options A-D}
  │
  ├─→ Client displays question + starts 15s timer
  │   Display: "Q: What is AI?" with options
  │   Timer: [15] [14] [13] ... [1] [TIMEOUT]
  │
  ├─→ Client user selects answer "A" at 12.5s
  │   REQ_SUBMIT_ANSWER (cmd=0x0503)
  │   payload: {"session_id": 100, "round": 1, "answer": "A"}
  │
  ├─→ Server validates answer
  │   SELECT correct_op FROM questions WHERE ...
  │   If "A" == correct_op → ✓ CORRECT
  │   Else → ✗ WRONG
  │
  ├─→ Server updates database
  │   UPDATE quickmode_rounds SET answer='A', correct=true, ...
  │   UPDATE user_stats SET ...
  │
  └─→ Server sends RES_SUBMIT_ANSWER
      payload: {"correct": true, "next_round": 2}

ROUNDS 2-15 (repeat pattern)
  │
  └─→ Same as Round 1, for each of 15 rounds
      - Difficulty progression: EASY (1-5) → MEDIUM (6-10) → HARD (11-15)
      - Timer resets for each question
      - Game continues until:
        * 15 questions answered correctly (WIN)
        * Wrong answer submitted (LOSE)
        * Timeout without submission (LOSE)

GAME END
  │
  ├─→ Server calls dao_qm_end_session()
  │   UPDATE quickmode_sessions SET status='FINISHED', end_time=NOW()
  │
  ├─→ Server records match in user_stats
  │   dao_stats_record_match()
  │   - total_matches += 1
  │   - correct_answers += N
  │   - Update win_rate
  │   - Update highest_score
  │
  ├─→ Server sends NOTIFY_GAME_OVER
  │   RES_GAME_END (cmd=0x0509)
  │   payload: {
  │     session_id: 100,
  │     total_questions: 15,
  │     correct_answers: 12,
  │     score: 85000,
  │     failed_at_round: 13,
  │     rank: "A",
  │     time_taken: 145
  │   }
  │
  └─→ Client displays results
      - Score breakdown
      - Difficulty progression
      - Option: Replay / View Stats / Leaderboard

END
```

---

## **7️⃣ DATABASE CHANGES**

### **Session Creation**
```sql
INSERT INTO quickmode_sessions (user_id, created_at, status)
VALUES (1, NOW(), 'STARTED');
-- Returns session_id = 100
```

### **Round Creation (Per Question)**
```sql
INSERT INTO quickmode_rounds (session_id, round_number, question_id, difficulty)
VALUES (100, 1, 42, 'EASY');
-- Stores question reference for this round
```

### **Round Update (Answer Submission)**
```sql
UPDATE quickmode_rounds 
SET answer='A', 
    correct=true, 
    response_time_ms=2500,
    answered_at=NOW()
WHERE session_id=100 AND round_number=1;
```

### **Session End**
```sql
UPDATE quickmode_sessions 
SET status='FINISHED', 
    end_time=NOW(),
    total_correct=12,
    total_score=85000
WHERE session_id=100;
```

### **Stats Update**
```sql
UPDATE user_stats 
SET total_matches = total_matches + 1,
    correct_answers = correct_answers + 12,
    wins = wins + (CASE WHEN total_correct=15 THEN 1 ELSE 0 END),
    losses = losses + (CASE WHEN total_correct<15 THEN 1 ELSE 0 END),
    highest_score = GREATEST(highest_score, 85000),
    avg_response_time_ms = (avg_response_time_ms + 3500) / 2
WHERE user_id=1;
```

---

## **8️⃣ COMMAND DEFINITIONS (0x05xx - BASIC MODE)**

| Command | Hex | Direction | Name | Purpose |
|---------|-----|-----------|------|---------|
| 1 | 0x0501 | S→C | NOTIFY_GAME_START | Server tells client game started |
| 2 | 0x0502 | S→C | NOTIFY_QUESTION | Server sends question for this round |
| 3 | 0x0503 | C→S | REQ_SUBMIT_ANSWER | Client submits answer |
| 4 | 0x0504 | S→C | RES_SUBMIT_ANSWER | Server confirms answer received |
| 5 | 0x0505 | S→C | NOTIFY_ANSWER_RESULT | Server tells if correct/wrong |
| 6 | 0x0506 | C→S | REQ_USE_LIFELINE | Client uses 50:50 lifeline (future) |
| 7 | 0x0507 | S→C | RES_USE_LIFELINE | Server confirms lifeline used |
| 8 | 0x0508 | S→C | NOTIFY_ROUND_END | Round ended (correct/wrong/timeout) |
| 9 | 0x0509 | S→C | NOTIFY_GAME_OVER | Game ended, results available |

---

## **9️⃣ ERROR HANDLING**

### **Timeout**
```
Client: 15-second timer expires
Client: No input from user
Client: Sends TIMEOUT notification
Server: dao_qm_end_session() with status='TIMEOUT'
Response: Game Over, correct_answers = N
```

### **Wrong Answer**
```
Client: Sends REQ_SUBMIT_ANSWER with incorrect answer
Server: Compares with correct_op
Response: RES_SUBMIT_ANSWER with correct=false
Server: dao_qm_end_session() immediately
Game Over, failed_at_round=N
```

### **Disconnect Mid-Game**
```
Server: Socket closes abruptly
Server: client_session_free() called
Database: Session status remains STARTED (incomplete)
Action: Can mark as ABANDONED or allow resume
```

---

## **🔟 TIMING DIAGRAM**

```
Time (seconds)
0            5            10           15
├────────────┼────────────┼────────────┤
│ Question   │ User       │ TIMEOUT    │
│ Displayed  │ Responds   │ Auto-end   │
│ Timer: 15s │ at 3.5s    │ Game if    │
│            │ Server     │ no answer  │
│            │ validates  │            │
│            │ Advances   │            │
│            │ to Round 2 │            │
└────────────┴────────────┴────────────┘

Round 1-5:   EASY    (avg 3-4 seconds per question)
Round 6-10:  MEDIUM  (avg 4-6 seconds per question)
Round 11-15: HARD    (avg 5-8 seconds per question)

Total game time (if all correct):
5 rounds × 3.5s (EASY) = 17.5s
5 rounds × 5s (MEDIUM) = 25s
5 rounds × 6.5s (HARD) = 32.5s
Total: ~75 seconds (1 minute 15 seconds)
+ Network latency & UI rendering: ~2-3 minutes per game
```

---

## **1️⃣1️⃣ SCORE CALCULATION**

```
Base Score = 1000 per correct answer
Difficulty Multiplier:
  - EASY:   1.0x
  - MEDIUM: 1.5x
  - HARD:   2.0x

Response Time Bonus:
  - < 2 seconds: +500 points
  - 2-5 seconds: +250 points
  - 5-10 seconds: +100 points
  - > 10 seconds: 0 points

Example:
Round 1-5 (EASY):
  5 × 1000 × 1.0 = 5000
  5 × 250 (avg response bonus) = 1250
  Subtotal: 6250

Round 6-10 (MEDIUM):
  5 × 1000 × 1.5 = 7500
  5 × 100 = 500
  Subtotal: 8000

Round 11-13 (HARD):
  3 × 1000 × 2.0 = 6000
  3 × 100 = 300
  Subtotal: 6300

Total: 20,550 points (if stop at round 13)
All 15 correct: ~30,000+ points
```

---

## **1️⃣2️⃣ IMPLEMENTATION STATUS**

| Component | Status | Notes |
|-----------|--------|-------|
| Session creation | ✅ Complete | `dao_qm_create_session()` |
| Question fetching | ✅ Complete | `dao_question_get_random()` |
| Round tracking | ✅ Complete | `dao_qm_create_round()` |
| Timer logic | ✅ Complete | `timer.h/c` with 15s countdown |
| Answer validation | ⚠️ Partial | Need `quickmode_dispatch()` implementation |
| Score calculation | ❌ Not started | Need multiplier logic |
| Results display | ⚠️ Partial | `test_quickmode.c` shows basic version |
| Network protocol | ❌ Not started | Commands defined but handlers empty |

---

## **NEXT STEPS TO COMPLETE**

```
1. Implement quickmode_dispatch() handler
   └─ Handle: 0x0501-0x0509 commands
   └─ Call appropriate DAO functions
   └─ Send responses via protocol_send_response()

2. Add score calculation logic
   └─ Base score × difficulty multiplier
   └─ Response time bonus
   └─ Leaderboard ranking

3. Implement lifeline system (50:50)
   └─ REQ_USE_LIFELINE handler
   └─ Remove 2 wrong options
   └─ Track lifeline usage per session

4. Add WebSocket or polling for real-time updates
   └─ Broadcast results to other players
   └─ Update leaderboard live
   └─ Show live match in progress

5. Implement 1vN competitive mode (0x06xx)
   └─ Multi-player synchronization
   └─ Simultaneous question delivery
   └─ Elimination tracking
```

