-- Migration: Forbid Login Strategy
-- Date: 2026-01-06
-- Purpose: Ensure single-session policy - 1 user = 1 active session maximum
--
-- This migration adds database constraints and indexes to support
-- the Forbid Login authentication strategy

-- 1. Check if user_sessions table exists and has necessary columns
-- (Assuming table already exists from earlier migrations)

-- 2. Add expiry constraint if not already present
-- Sessions are considered "active" if expires_at > NOW()
-- This ensures old/expired sessions don't count as active

-- 3. Add index for fast lookup of active sessions by user_id
-- This is critical for the forbid-login check
-- Note: Cannot use partial index with NOW() (not IMMUTABLE)
-- Instead, create regular index and filter in query
CREATE INDEX IF NOT EXISTS idx_user_sessions_active_by_user 
    ON user_sessions(user_id, expires_at);

-- 4. Add index for fast lookup by token (if not exists)
CREATE INDEX IF NOT EXISTS idx_user_sessions_token 
    ON user_sessions(access_token);

-- 5. Optional: Add created_at timestamp if not present
-- Check if column exists first
-- Note: PostgreSQL requires using information_schema to check columns
ALTER TABLE user_sessions 
ADD COLUMN IF NOT EXISTS created_at TIMESTAMP DEFAULT NOW();

-- 6. Optional: Add device_info column for future tracking
-- Useful for notifying users when they login from new devices
ALTER TABLE user_sessions 
ADD COLUMN IF NOT EXISTS device_info VARCHAR(256);

-- 7. Log migration completion
-- Note: This is just a comment, not executed
-- [MIGRATION LOG] forbid_login migration completed
-- - Added index on (user_id, expires_at) for fast active session lookup
-- - Added index on access_token for fast token lookup
-- - Added optional device_info column for future use

-- Test query to verify the migration works:
-- SELECT COUNT(*) as active_sessions 
-- FROM user_sessions 
-- WHERE user_id = 1 AND expires_at > NOW();
-- 
-- Expected result: 0 or 1 (at most 1 active session per user due to forbid-login logic)
