-- =========================================================
-- Schema: Millionaire Game (PostgreSQL) - Updated Version
-- =========================================================

DROP DATABASE IF EXISTS ltm_group04;
CREATE DATABASE ltm_group04;
\c ltm_group04;

-- =========================
-- USERS (gộp stats vào đây)
-- =========================
CREATE TABLE users (
  user_id         BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  username        VARCHAR(32)  NOT NULL UNIQUE,
  password        VARCHAR(255) NOT NULL,
  avatar_img      VARCHAR(512) NULL,
  quickmode_games INT NOT NULL DEFAULT 0,
  quickmode_wins  INT NOT NULL DEFAULT 0,
  onevn_games     INT NOT NULL DEFAULT 0,
  onevn_wins      INT NOT NULL DEFAULT 0,
  created_at      TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

-- =========================
-- USER SESSIONS
-- =========================
CREATE TABLE user_sessions (
  id              BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  user_id         BIGINT NOT NULL,
  access_token    CHAR(64) NOT NULL UNIQUE,
  last_heartbeat  TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  expires_at      TIMESTAMPTZ NOT NULL,
  CONSTRAINT fk_user_sessions_user
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

CREATE INDEX idx_user_sessions_user ON user_sessions(user_id);
CREATE INDEX idx_user_sessions_exp  ON user_sessions(expires_at);
CREATE INDEX idx_user_sessions_heartbeat ON user_sessions(last_heartbeat);

-- =========================
-- MESSAGES
-- =========================
CREATE TABLE messages (
  id            BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  sender_id     BIGINT NOT NULL,
  receiver_id   BIGINT NULL,  -- NULL nếu là tin nhắn phòng
  room_id       BIGINT NULL,  -- NULL nếu là chat 1-1
  message       TEXT NOT NULL,
  created_at    TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  is_delivered  BOOLEAN NOT NULL DEFAULT FALSE,
  delivered_at  TIMESTAMPTZ NULL,
  is_read       BOOLEAN NOT NULL DEFAULT FALSE,
  read_at       TIMESTAMPTZ NULL,
  CONSTRAINT fk_messages_sender   FOREIGN KEY (sender_id)   REFERENCES users(user_id) ON DELETE CASCADE,
  CONSTRAINT fk_messages_receiver FOREIGN KEY (receiver_id) REFERENCES users(user_id) ON DELETE CASCADE,
  CONSTRAINT chk_messages_type CHECK (
    (receiver_id IS NOT NULL AND room_id IS NULL) OR
    (receiver_id IS NULL AND room_id IS NOT NULL)
  )
);

CREATE INDEX idx_messages_pair ON messages (sender_id, receiver_id, created_at);
CREATE INDEX idx_messages_receiver ON messages (receiver_id, is_delivered, is_read);
CREATE INDEX idx_messages_room ON messages (room_id, created_at);

-- =========================
-- FRIEND RELATIONSHIPS
-- =========================
CREATE TABLE friend_relationships (
  id            BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  user_id       BIGINT NOT NULL,
  peer_user_id  BIGINT NOT NULL,
  status        TEXT NOT NULL CHECK (status IN ('PENDING','ACCEPTED','DECLINED','BLOCKED')),
  created_at    TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  responded_at  TIMESTAMPTZ NULL,
  CONSTRAINT fk_fr_user FOREIGN KEY (user_id)      REFERENCES users(user_id) ON DELETE CASCADE,
  CONSTRAINT fk_fr_peer FOREIGN KEY (peer_user_id) REFERENCES users(user_id) ON DELETE CASCADE,
  CONSTRAINT chk_fr_not_self CHECK (user_id <> peer_user_id)
);

CREATE INDEX idx_fr_user ON friend_relationships(user_id, status);
CREATE INDEX idx_fr_peer ON friend_relationships(peer_user_id, status);
CREATE UNIQUE INDEX uq_fr_pair ON friend_relationships(user_id, peer_user_id);

-- =========================
-- QUESTIONS
-- =========================
CREATE TABLE question (
  question_id      BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  difficulty_level TEXT NOT NULL CHECK (difficulty_level IN ('EASY','MEDIUM','HARD')),
  content          TEXT NOT NULL,
  opA              VARCHAR(512) NOT NULL,
  opB              VARCHAR(512) NOT NULL,
  opC              VARCHAR(512) NOT NULL,
  opD              VARCHAR(512) NOT NULL,
  correct_op       CHAR(1) NOT NULL CHECK (correct_op IN ('A','B','C','D')),
  explanation      TEXT NULL
);

CREATE INDEX idx_question_diff ON question(difficulty_level);

-- =========================
-- ROOMS
-- =========================
CREATE TABLE room (
  room_id                BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  owner_id               BIGINT NOT NULL,
  status                 TEXT NOT NULL CHECK (status IN ('WAITING','STARTING','IN_PROGRESS','FINISHED')),
  max_number_players     INT  NOT NULL DEFAULT 8,
  created_at             TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  started_at             TIMESTAMPTZ NULL,
  ended_at               TIMESTAMPTZ NULL,
  CONSTRAINT fk_room_owner FOREIGN KEY (owner_id) REFERENCES users(user_id)
);

CREATE INDEX idx_room_status ON room(status);
CREATE OR REPLACE VIEW rooms AS SELECT * FROM room;

CREATE TABLE room_members (
  room_id   BIGINT NOT NULL,
  user_id   BIGINT NOT NULL,
  joined_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  PRIMARY KEY (room_id, user_id),
  CONSTRAINT fk_rmem_room FOREIGN KEY (room_id) REFERENCES room(room_id) ON DELETE CASCADE,
  CONSTRAINT fk_rmem_user FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

-- Bổ sung FK cho messages.room_id
ALTER TABLE messages
  ADD CONSTRAINT fk_messages_room
  FOREIGN KEY (room_id) REFERENCES room(room_id) ON DELETE CASCADE;

-- =========================
-- 1vN SESSIONS (chỉ lưu kết quả cuối)
-- =========================
CREATE TABLE onevn_sessions (
  session_id   BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  room_id      BIGINT NOT NULL,
  status       TEXT NOT NULL CHECK (status IN ('IN_PROGRESS','FINISHED','ABORTED')),
  winner_id    BIGINT NULL,
  started_at   TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  ended_at     TIMESTAMPTZ NULL,
  players      JSONB NOT NULL DEFAULT '[]'::jsonb,  -- [{ user_id, score, final_rank, eliminated }]
  CONSTRAINT fk_1s_room   FOREIGN KEY (room_id)   REFERENCES room(room_id)   ON DELETE CASCADE,
  CONSTRAINT fk_1s_winner FOREIGN KEY (winner_id) REFERENCES users(user_id)
);

CREATE UNIQUE INDEX uq_onevn_room_active
  ON onevn_sessions(room_id, status)
  WHERE status = 'IN_PROGRESS';

CREATE INDEX idx_onevn_players_jsonb ON onevn_sessions USING GIN (players);

-- =========================
-- SAMPLE DATA
-- =========================

-- Test Users (cho test 1vN)
INSERT INTO users (username, password) VALUES
('Owner', 'pass123'),
('Player2', 'pass123'),
('Player3', 'pass123'),
('Player4', 'pass123'),
('Player5', 'pass123'),
('Player6', 'pass123'),
('Player7', 'pass123'),
('Player8', 'pass123'),
('Alice', 'pass123'),
('Bob', 'pass123'),
('Charlie', 'pass123'),
('Diana', 'pass123')
ON CONFLICT (username) DO NOTHING;

-- Test Questions
INSERT INTO question (difficulty_level, content, opA, opB, opC, opD, correct_op, explanation)
VALUES
-- EASY Questions (10 câu)
('EASY', 'Thủ đô của Việt Nam là thành phố nào?', 'Hồ Chí Minh', 'Hà Nội', 'Đà Nẵng', 'Huế', 'B', 'Thủ đô của Việt Nam hiện nay là Hà Nội.'),
('EASY', '"AI" trong "Trí tuệ nhân tạo (AI)" là viết tắt của cụm từ tiếng Anh nào?', 'Artificial Intelligence', 'Advanced Internet', 'Automatic Interaction', 'Algorithm Integration', 'A', '"AI" là viết tắt của "Artificial Intelligence" (Trí tuệ nhân tạo).'),
('EASY', 'Số nguyên tố nhỏ nhất là số nào?', '0', '1', '2', '3', 'C', 'Số 2 là số nguyên tố nhỏ nhất và là số nguyên tố chẵn duy nhất.'),
('EASY', 'Hành tinh nào gần Mặt Trời nhất?', 'Venus', 'Mercury', 'Earth', 'Mars', 'B', 'Mercury (Sao Thủy) là hành tinh gần Mặt Trời nhất.'),
('EASY', 'Ngôn ngữ lập trình nào được tạo bởi Guido van Rossum?', 'Java', 'Python', 'C++', 'JavaScript', 'B', 'Python được tạo bởi Guido van Rossum vào năm 1991.'),
('EASY', 'Tổng số châu lục trên Trái Đất là bao nhiêu?', '5', '6', '7', '8', 'C', 'Có 7 châu lục: Á, Âu, Phi, Bắc Mỹ, Nam Mỹ, Úc, Nam Cực.'),
('EASY', 'Thành phố nào là thủ đô của Pháp?', 'Lyon', 'Marseille', 'Paris', 'Nice', 'C', 'Paris là thủ đô và thành phố lớn nhất của Pháp.'),
('EASY', 'Số Pi (π) có giá trị xấp xỉ bằng bao nhiêu?', '2.14', '3.14', '4.14', '5.14', 'B', 'Số Pi có giá trị xấp xỉ 3.14159...'),
('EASY', 'Hệ điều hành nào được phát triển bởi Microsoft?', 'Linux', 'macOS', 'Windows', 'Android', 'C', 'Windows là hệ điều hành được phát triển bởi Microsoft.'),
('EASY', 'Ngôn ngữ nào được nói nhiều nhất trên thế giới?', 'Tiếng Anh', 'Tiếng Trung', 'Tiếng Tây Ban Nha', 'Tiếng Ả Rập', 'B', 'Tiếng Trung (Mandarin) là ngôn ngữ được nói nhiều nhất với hơn 1 tỷ người.'),

-- MEDIUM Questions (10 câu)
('MEDIUM', 'Ngôn ngữ lập trình nào sau đây thường được dùng để lập trình hệ điều hành Linux?', 'Python', 'Java', 'C', 'HTML', 'C', 'Nhân Linux được viết chủ yếu bằng ngôn ngữ C.'),
('MEDIUM', 'Giao thức nào sau đây hoạt động ở tầng Transport trong mô hình TCP/IP?', 'IP', 'TCP', 'Ethernet', 'ARP', 'B', 'TCP (Transmission Control Protocol) là giao thức tầng Transport.'),
('MEDIUM', 'Trong lập trình, "OOP" là viết tắt của gì?', 'Object-Oriented Programming', 'Online Operating Protocol', 'Optimized Output Process', 'Ordered Operation Procedure', 'A', 'OOP là Object-Oriented Programming (Lập trình hướng đối tượng).'),
('MEDIUM', 'Cấu trúc dữ liệu nào hoạt động theo nguyên tắc LIFO (Last In First Out)?', 'Queue', 'Stack', 'Array', 'Linked List', 'B', 'Stack (Ngăn xếp) hoạt động theo nguyên tắc LIFO.'),
('MEDIUM', 'Port mặc định của giao thức HTTP là gì?', '21', '25', '80', '443', 'C', 'Port 80 là port mặc định cho HTTP, port 443 cho HTTPS.'),
('MEDIUM', 'Trong cơ sở dữ liệu, lệnh nào dùng để lấy dữ liệu từ bảng?', 'INSERT', 'UPDATE', 'SELECT', 'DELETE', 'C', 'SELECT là lệnh SQL dùng để truy vấn (query) dữ liệu.'),
('MEDIUM', 'Thuật toán sắp xếp nào có độ phức tạp thời gian tốt nhất là O(n log n)?', 'Bubble Sort', 'Insertion Sort', 'Quick Sort', 'Selection Sort', 'C', 'Quick Sort có độ phức tạp trung bình O(n log n).'),
('MEDIUM', 'Trong mạng máy tính, địa chỉ IP version 4 có bao nhiêu bit?', '16', '32', '64', '128', 'B', 'IPv4 có 32 bit (4 bytes), IPv6 có 128 bit.'),
('MEDIUM', 'Ngôn ngữ nào được dùng để tạo style cho trang web?', 'HTML', 'CSS', 'JavaScript', 'SQL', 'B', 'CSS (Cascading Style Sheets) dùng để định dạng và tạo style cho HTML.'),
('MEDIUM', 'Trong Git, lệnh nào dùng để tạo một branch mới?', 'git checkout', 'git branch', 'git merge', 'git clone', 'B', 'git branch <name> dùng để tạo branch mới, git checkout để chuyển branch.'),

-- HARD Questions (10 câu)
('HARD', 'Trong cơ sở dữ liệu quan hệ, ràng buộc đảm bảo giá trị một cột là duy nhất và không NULL được gọi là gì?', 'FOREIGN KEY', 'PRIMARY KEY', 'UNIQUE KEY', 'CHECK', 'B', 'PRIMARY KEY yêu cầu giá trị duy nhất và không được NULL.'),
('HARD', 'Trong mô hình OSI, giao thức HTTP chủ yếu tương ứng với tầng nào?', 'Tầng Mạng (Network)', 'Tầng Vật lý (Physical)', 'Tầng Giao vận (Transport)', 'Tầng Ứng dụng (Application)', 'D', 'HTTP là giao thức tầng Ứng dụng trong mô hình OSI.'),
('HARD', 'Trong lập trình, "Dijkstra" là thuật toán dùng để giải quyết vấn đề gì?', 'Sắp xếp mảng', 'Tìm đường đi ngắn nhất', 'Tìm kiếm nhị phân', 'Sắp xếp topo', 'B', 'Thuật toán Dijkstra dùng để tìm đường đi ngắn nhất trong đồ thị có trọng số.'),
('HARD', 'Trong cơ sở dữ liệu, "ACID" là viết tắt của gì?', 'Atomicity, Consistency, Isolation, Durability', 'Access, Control, Integrity, Data', 'Analysis, Computation, Integration, Design', 'Application, Code, Interface, Database', 'A', 'ACID là 4 tính chất: Atomicity, Consistency, Isolation, Durability.'),
('HARD', 'Trong mạng máy tính, giao thức nào dùng để chuyển đổi địa chỉ IP sang địa chỉ MAC?', 'TCP', 'UDP', 'ARP', 'ICMP', 'C', 'ARP (Address Resolution Protocol) dùng để tìm địa chỉ MAC từ địa chỉ IP.'),
('HARD', 'Trong lập trình, "Singleton Pattern" đảm bảo điều gì?', 'Chỉ có một instance của class', 'Class có nhiều instances', 'Class không thể khởi tạo', 'Class chỉ có static methods', 'A', 'Singleton Pattern đảm bảo chỉ có một instance duy nhất của class.'),
('HARD', 'Trong cơ sở dữ liệu, "Normalization" nhằm mục đích gì?', 'Tăng tốc độ truy vấn', 'Giảm redundancy và dependency', 'Tăng kích thước database', 'Giảm số lượng bảng', 'B', 'Normalization giúp giảm dữ liệu trùng lặp và phụ thuộc không cần thiết.'),
('HARD', 'Trong lập trình, "Deadlock" xảy ra khi nào?', 'Khi chương trình chạy quá chậm', 'Khi hai process chờ nhau vô hạn', 'Khi bộ nhớ đầy', 'Khi CPU quá tải', 'B', 'Deadlock xảy ra khi các process chờ đợi tài nguyên của nhau một cách vòng tròn.'),
('HARD', 'Trong mạng máy tính, "NAT" là viết tắt của gì?', 'Network Address Translation', 'Network Access Technology', 'Network Application Transport', 'Network Authentication Token', 'A', 'NAT (Network Address Translation) dùng để chuyển đổi địa chỉ IP.'),
('HARD', 'Trong lập trình, "Big O notation" dùng để mô tả gì?', 'Kích thước của biến', 'Độ phức tạp thuật toán', 'Số lượng biến', 'Tốc độ CPU', 'B', 'Big O notation mô tả độ phức tạp thời gian và không gian của thuật toán.');
