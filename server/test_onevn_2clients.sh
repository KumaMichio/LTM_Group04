#!/bin/bash
# Script to test 1vN game with 2 clients
# Usage: ./test_onevn_2clients.sh [host] [port]

HOST=${1:-localhost}
PORT=${2:-9000}

echo "=== 1vN Game Test with 2 Clients ==="
echo "Host: $HOST:$PORT"
echo ""
echo "Instructions:"
echo "1. Client 1 (testuser1) will create a room and print the Room ID"
echo "2. Client 2 (testuser2) should join that room"
echo "3. After client 2 joins, client 1 will start the game"
echo ""

# Start client 1 in background, capture output
./build/test_onevn_network "$HOST" "$PORT" testuser1 testpass1 > /tmp/client1.log 2>&1 &
CLIENT1_PID=$!

# Wait for client 1 to create room (it prints room_id) 
sleep 2

# Extract room_id from client 1 output
ROOM_ID=$(grep "Room ID:" /tmp/client1.log 2>/dev/null | tail -1 | sed 's/.*Room ID: \([0-9]*\).*/\1/')

if [ -z "$ROOM_ID" ] || [ "$ROOM_ID" = "0" ]; then
    echo "[ERROR] Could not determine room_id from client 1 output"
    echo "[ERROR] Client 1 output:"
    cat /tmp/client1.log
    kill $CLIENT1_PID 2>/dev/null
    exit 1
fi

echo "[MAIN] Detected Room ID: $ROOM_ID"
echo "[MAIN] Starting client 2 (testuser2) to join room..."

# Start client 2 with room_id
echo "$ROOM_ID" | ./build/test_onevn_join "$HOST" "$PORT" testuser2 testpass2 &
CLIENT2_PID=$!

# Wait for both clients
wait $CLIENT1_PID
CLIENT1_EXIT=$?
wait $CLIENT2_PID
CLIENT2_EXIT=$?

echo ""
echo "=== Test Results ==="
echo "Client 1 exit code: $CLIENT1_EXIT"
echo "Client 2 exit code: $CLIENT2_EXIT"

if [ $CLIENT1_EXIT -eq 0 ] && [ $CLIENT2_EXIT -eq 0 ]; then
    echo "✓ Test completed successfully!"
    exit 0
else
    echo "✗ Test failed"
    echo ""
    echo "Client 1 output:"
    cat /tmp/client1.log
    exit 1
fi

