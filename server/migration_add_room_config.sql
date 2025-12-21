-- Migration: Add room configuration for 1vN mode
-- Adds fields to store question count per difficulty level

ALTER TABLE room
ADD COLUMN IF NOT EXISTS easy_count INT NOT NULL DEFAULT 0,
ADD COLUMN IF NOT EXISTS medium_count INT NOT NULL DEFAULT 0,
ADD COLUMN IF NOT EXISTS hard_count INT NOT NULL DEFAULT 0,
ADD CONSTRAINT chk_room_question_total CHECK (easy_count + medium_count + hard_count <= 30);

-- Update existing rooms to have default values
UPDATE room SET easy_count = 10, medium_count = 10, hard_count = 10 WHERE easy_count = 0 AND medium_count = 0 AND hard_count = 0;

