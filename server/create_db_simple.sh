#!/bin/bash
# Simple database creation - no sudo needed
# Assumes you can connect with password or peer auth

echo "Creating database ltm_group04..."

# Try with password first (if PGPASSWORD is set)
if [ -n "$PGPASSWORD" ]; then
    echo "Using password authentication..."
    psql -U postgres -h localhost -c "CREATE DATABASE ltm_group04;" 2>/dev/null || echo "Database may already exist"
    psql -U postgres -h localhost -d ltm_group04 -f db.sql 2>&1 | grep -v "already exists" | grep -v "DROP DATABASE" || true
    echo "✅ Done! Set DB_CONN:"
    echo "export DB_CONN=\"host=localhost port=5432 dbname=ltm_group04 user=postgres password=YOUR_PASSWORD\""
    exit 0
fi

# Try peer auth (if running as postgres user or with sudo)
if command -v sudo >/dev/null 2>&1; then
    echo "Trying with sudo (peer authentication)..."
    sudo -u postgres psql -c "CREATE DATABASE ltm_group04;" 2>/dev/null || echo "Database may already exist"
    sudo -u postgres psql -d ltm_group04 -f db.sql 2>&1 | grep -v "already exists" | grep -v "DROP DATABASE" || true
    echo "✅ Done! Set DB_CONN:"
    echo "export DB_CONN=\"host=localhost port=5432 dbname=ltm_group04 user=postgres\""
    exit 0
fi

echo "❌ Cannot create database automatically"
echo "Please run manually:"
echo "  sudo -u postgres psql -c \"CREATE DATABASE ltm_group04;\""
echo "  sudo -u postgres psql -d ltm_group04 -f db.sql"

