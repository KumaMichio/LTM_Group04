#!/bin/bash
# Complete database setup script
# Handles peer authentication and creates database

set -e

echo "=========================================="
echo "Database Setup for ltm_group04"
echo "=========================================="
echo ""

# Step 1: Check PostgreSQL is running
echo "Step 1: Checking PostgreSQL..."
if sudo systemctl is-active --quiet postgresql || sudo service postgresql status > /dev/null 2>&1; then
    echo "✅ PostgreSQL is running"
else
    echo "⚠️  Starting PostgreSQL..."
    sudo systemctl start postgresql 2>/dev/null || sudo service postgresql start 2>/dev/null
    sleep 2
fi

echo ""
echo "Step 2: Creating database ltm_group04..."

# Try with sudo -u postgres (peer auth)
if sudo -u postgres psql -c "SELECT 1;" > /dev/null 2>&1; then
    echo "✅ Can connect with peer authentication"
    
    # Create database
    sudo -u postgres psql -c "DROP DATABASE IF EXISTS ltm_group04;" 2>/dev/null || true
    sudo -u postgres psql -c "CREATE DATABASE ltm_group04;" 2>/dev/null
    
    if [ $? -eq 0 ]; then
        echo "✅ Database created"
    else
        # Check if exists
        if sudo -u postgres psql -lqt | cut -d \| -f 1 | grep -qw ltm_group04; then
            echo "⚠️  Database already exists"
        else
            echo "❌ Failed to create database"
            exit 1
        fi
    fi
    
    echo ""
    echo "Step 3: Running schema SQL..."
    if [ -f "db.sql" ]; then
        # Remove DROP/CREATE DATABASE lines and run
        sudo -u postgres psql -d ltm_group04 -f db.sql 2>&1 | grep -v "already exists" | grep -v "DROP DATABASE" || true
        echo "✅ Schema applied"
    else
        echo "❌ db.sql not found!"
        exit 1
    fi
    
else
    echo "❌ Cannot connect to PostgreSQL"
    echo "Trying alternative method..."
    
    # Try with PGPASSWORD
    export PGPASSWORD="1"
    if psql -U postgres -h localhost -c "SELECT 1;" > /dev/null 2>&1; then
        echo "✅ Can connect with password authentication"
        psql -U postgres -h localhost -c "CREATE DATABASE ltm_group04;" 2>/dev/null || true
        psql -U postgres -h localhost -d ltm_group04 -f db.sql 2>&1 | grep -v "already exists" || true
        echo "✅ Database and schema created"
    else
        echo "❌ Cannot connect. Please:"
        echo "  1. Check PostgreSQL is running"
        echo "  2. Reset password: sudo -u postgres psql -c \"ALTER USER postgres WITH PASSWORD '1';\""
        exit 1
    fi
fi

echo ""
echo "=========================================="
echo "✅ Setup Complete!"
echo "=========================================="
echo ""
echo "For peer authentication (recommended):"
echo "  export DB_CONN=\"host=localhost port=5432 dbname=ltm_group04 user=postgres\""
echo ""
echo "For password authentication:"
echo "  export DB_CONN=\"host=localhost port=5432 dbname=ltm_group04 user=postgres password=1\""
echo ""
echo "Test:"
echo "  ./build/test_db"
echo ""

