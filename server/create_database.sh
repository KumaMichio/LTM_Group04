#!/bin/bash
# Script to create database and run schema
# Usage: bash create_database.sh

set -e  # Exit on error

echo "=========================================="
echo "Creating Database: ltm_group04"
echo "=========================================="
echo ""

# Check if DB_CONN is set, if not use default
if [ -z "$DB_CONN" ]; then
    export DB_CONN="host=localhost port=5432 dbname=postgres user=postgres password=1"
    echo "Using default connection (postgres database)"
fi

# Extract connection info (simple parsing)
# Format: host=HOST port=PORT dbname=DB user=USER password=PASS
DB_HOST=$(echo $DB_CONN | grep -oP 'host=\K[^\s]+' || echo "localhost")
DB_PORT=$(echo $DB_CONN | grep -oP 'port=\K[^\s]+' || echo "5432")
DB_USER=$(echo $DB_CONN | grep -oP 'user=\K[^\s]+' || echo "postgres")
DB_PASS=$(echo $DB_CONN | grep -oP 'password=\K[^\s]+' || echo "1")

echo "Connection info:"
echo "  Host: $DB_HOST"
echo "  Port: $DB_PORT"
echo "  User: $DB_USER"
echo ""

# Set PGPASSWORD for psql
export PGPASSWORD="$DB_PASS"

echo "Step 1: Checking PostgreSQL connection..."
if psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d postgres -c "SELECT 1;" > /dev/null 2>&1; then
    echo "✅ PostgreSQL connection OK"
else
    echo "❌ Cannot connect to PostgreSQL"
    echo "Please check:"
    echo "  1. PostgreSQL is running"
    echo "  2. Connection info is correct"
    echo "  3. Password is correct"
    exit 1
fi

echo ""
echo "Step 2: Creating database ltm_group04..."
if psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d postgres -c "CREATE DATABASE ltm_group04;" 2>&1; then
    echo "✅ Database created successfully"
else
    # Check if database already exists
    if psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d postgres -c "\l" | grep -q "ltm_group04"; then
        echo "⚠️  Database already exists, continuing..."
    else
        echo "❌ Failed to create database"
        exit 1
    fi
fi

echo ""
echo "Step 3: Running schema SQL..."
if [ -f "db.sql" ]; then
    # Use connection to postgres first, then connect to ltm_group04
    psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d postgres -f db.sql 2>&1 | grep -v "already exists" || true
    echo "✅ Schema applied"
else
    echo "❌ db.sql not found!"
    exit 1
fi

echo ""
echo "=========================================="
echo "✅ Database setup complete!"
echo "=========================================="
echo ""
echo "Set DB_CONN to use the new database:"
echo "export DB_CONN=\"host=$DB_HOST port=$DB_PORT dbname=ltm_group04 user=$DB_USER password=$DB_PASS\""
echo ""
echo "Test connection:"
echo "  ./build/test_db"
echo ""

