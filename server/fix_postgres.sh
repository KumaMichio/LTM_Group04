#!/bin/bash
# Quick fix script for PostgreSQL authentication
# This script attempts to reset postgres password

echo "=========================================="
echo "PostgreSQL Password Reset Script"
echo "=========================================="
echo ""

# Check if running as root or with sudo
if [ "$EUID" -ne 0 ]; then 
    echo "⚠️  This script needs sudo privileges"
    echo "Please run: sudo bash fix_postgres.sh"
    exit 1
fi

echo "Step 1: Checking PostgreSQL status..."
if systemctl is-active --quiet postgresql; then
    echo "✅ PostgreSQL is running"
else
    echo "⚠️  PostgreSQL is not running. Starting..."
    systemctl start postgresql
    sleep 2
fi

echo ""
echo "Step 2: Attempting to reset postgres password..."
echo "Setting password to: 1"
echo ""

# Try to reset password
sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD '1';" 2>/dev/null

if [ $? -eq 0 ]; then
    echo "✅ Password reset successful!"
    echo ""
    echo "=========================================="
    echo "Next steps:"
    echo "=========================================="
    echo ""
    echo "1. Set DB_CONN environment variable:"
    echo "   export DB_CONN=\"host=localhost port=5432 dbname=ltm_group04 user=postgres password=1\""
    echo ""
    echo "2. Create database (if not exists):"
    echo "   psql -U postgres -f server/db.sql"
    echo ""
    echo "3. Test connection:"
    echo "   ./build/test_db"
    echo ""
else
    echo "❌ Failed to reset password automatically"
    echo ""
    echo "Try manual steps:"
    echo "1. sudo -u postgres psql"
    echo "2. ALTER USER postgres WITH PASSWORD '1';"
    echo "3. \\q"
    echo ""
    echo "Or see FIX_POSTGRES_AUTH.md for more options"
fi

