# Script to test 1vN mode with 2 clients on same machine
# Run from PowerShell: .\test_1vn_2clients.ps1

Write-Host "=== Test 1vN Mode with 2 Clients ===" -ForegroundColor Cyan
Write-Host ""

$exePath = ".\build\Desktop_Qt_6_10_1_MinGW_64_bit-Debug\MillionaireClient.exe"

# Check if exe exists
if (-not (Test-Path $exePath)) {
    Write-Host "[ERROR] File not found: $exePath" -ForegroundColor Red
    Write-Host "Please build the project first." -ForegroundColor Yellow
    exit 1
}

Write-Host "Testing Instructions:" -ForegroundColor Yellow
Write-Host "1. Two client windows will open" -ForegroundColor White
Write-Host "2. Window 1 - Login with: testuser1 / testpass1" -ForegroundColor White
Write-Host "3. Window 2 - Login with: testuser2 / testpass2" -ForegroundColor White
Write-Host "4. Client 1 creates room and selects question count" -ForegroundColor White
Write-Host "5. Client 2 joins room (enter room ID from client 1)" -ForegroundColor White
Write-Host "6. When both are in room, client 1 clicks Start Game" -ForegroundColor White
Write-Host ""
Write-Host "Note: Server must be running on WSL (localhost:9000)" -ForegroundColor Green
Write-Host ""

# Wait for user to read instructions
Read-Host "Press Enter to start..."

Write-Host ""
Write-Host "Starting Client 1..." -ForegroundColor Cyan
Start-Process -FilePath $exePath

# Wait 2 seconds for client 1 to start
Start-Sleep -Seconds 2

Write-Host "Starting Client 2..." -ForegroundColor Cyan
Start-Process -FilePath $exePath

Write-Host ""
Write-Host "Successfully launched 2 clients!" -ForegroundColor Green
Write-Host "Login accounts:" -ForegroundColor Yellow
Write-Host ""
Write-Host "  Client 1: testuser1 / testpass1" -ForegroundColor White
Write-Host "  Client 2: testuser2 / testpass2" -ForegroundColor White
Write-Host ""
Write-Host "For 3+ players, run the exe again:" -ForegroundColor Cyan
Write-Host "  $exePath" -ForegroundColor Gray
Write-Host "  Login with: testuser3 / testpass3" -ForegroundColor Gray
Write-Host ""
