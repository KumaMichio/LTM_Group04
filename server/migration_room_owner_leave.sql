-- Migration: Auto Close Room When Owner Leaves
-- Date: 2026-01-07
-- Purpose: Automatically close room and kick all members when owner leaves
-- This ensures consistent room state and prevents orphaned rooms

-- =========================================================
-- STEP 1: Allow owner_id to be NULL
-- =========================================================
-- When a room is closed, owner_id will be set to NULL
DO $$
BEGIN
    IF EXISTS (
        SELECT 1 FROM information_schema.table_constraints 
        WHERE table_name = 'room' 
        AND constraint_type = 'FOREIGN KEY'
        AND constraint_name = 'fk_room_owner'
    ) THEN
        -- Drop existing constraint first
        ALTER TABLE room DROP CONSTRAINT fk_room_owner;
        
        -- Re-add constraint allowing NULL
        ALTER TABLE room
            ADD CONSTRAINT fk_room_owner 
            FOREIGN KEY (owner_id) REFERENCES users(user_id);
        
        RAISE NOTICE 'Removed NOT NULL constraint from owner_id';
    END IF;
END $$;

-- =========================================================
-- STEP 2: Create function to handle room closing
-- =========================================================
CREATE OR REPLACE FUNCTION auto_close_room_on_owner_leave()
RETURNS TRIGGER AS $$
DECLARE
  v_owner_id BIGINT;
  v_room_status TEXT;
BEGIN
  -- Get current room state BEFORE it was deleted
  -- (OLD.room_id still exists)
  SELECT owner_id, status
  INTO v_owner_id, v_room_status
  FROM room
  WHERE room_id = OLD.room_id;
  
  -- Check: user being deleted is the owner, and room is active
  IF v_owner_id = OLD.user_id 
     AND v_room_status IN ('WAITING', 'STARTING') THEN
    
    -- Just update room status - don't delete members yet
    -- (Server/dao_rooms_delete will handle that)
    UPDATE room 
    SET 
      status = 'FINISHED',
      ended_at = NOW(),
      owner_id = NULL
    WHERE room_id = OLD.room_id;
  END IF;
  
  RETURN OLD;
END;
$$ LANGUAGE plpgsql;

-- =========================================================
-- STEP 3: Create trigger
-- =========================================================
-- Drop trigger if it exists (for idempotency)
DROP TRIGGER IF EXISTS trg_auto_close_room_on_owner_leave ON room_members;

-- Create the trigger - use AFTER to avoid conflict with DELETE operation
CREATE TRIGGER trg_auto_close_room_on_owner_leave
  AFTER DELETE ON room_members
  FOR EACH ROW
  EXECUTE FUNCTION auto_close_room_on_owner_leave();

-- =========================================================
-- STEP 4: Verification
-- =========================================================
-- Verify the trigger exists
SELECT 
  trigger_name,
  event_manipulation,
  event_object_table,
  action_statement
FROM information_schema.triggers
WHERE trigger_name = 'trg_auto_close_room_on_owner_leave';

-- Log migration success
DO $$
BEGIN
    RAISE NOTICE '[MIGRATION SUCCESS] Auto close room on owner leave';
    RAISE NOTICE '- Function: auto_close_room_on_owner_leave()';
    RAISE NOTICE '- Trigger: trg_auto_close_room_on_owner_leave';
    RAISE NOTICE '- When owner leaves room_members, trigger will:';
    RAISE NOTICE '  1. Check if user is room owner';
    RAISE NOTICE '  2. Close room (status=FINISHED, owner_id=NULL)';
    RAISE NOTICE '  3. Kick all members';
    RAISE NOTICE '  4. Log system message';
END $$;
