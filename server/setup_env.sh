#!/bin/bash
# Setup environment variables for database connection
# Usage: source setup_env.sh
# Or: . setup_env.sh

# Default PostgreSQL connection string
# Format: "host=localhost port=5432 dbname=ltm_group04 user=postgres password=YOUR_PASSWORD"
export DB_CONN="host=localhost port=5432 dbname=ltm_group04 user=postgres password=1"

# Optional: Server mode
export SERVER_MODE="0"  # Set to "1" to run as server
export SERVER_PORT="9000"

echo "Environment variables set:"
echo "  DB_CONN=$DB_CONN"
echo "  SERVER_MODE=$SERVER_MODE"
echo "  SERVER_PORT=$SERVER_PORT"
echo ""
echo "To use, run: source setup_env.sh"
echo "Or modify the values above and export manually"

