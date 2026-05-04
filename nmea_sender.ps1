$file = "C:\Users\USER\nmea_feed.txt"
$client = New-Object System.Net.Sockets.TcpClient
$client.Connect("127.0.0.1", 4444)
$stream = $client.GetStream()
$writer = New-Object System.IO.StreamWriter($stream)
$writer.AutoFlush = $true

Write-Host "Connected to QEMU serial port. Sending NMEA sentences..."

$lines = Get-Content $file
while ($true) {
    foreach ($line in $lines) {
        $writer.WriteLine($line)
        Write-Host "Sent: $line"
        Start-Sleep -Milliseconds 250
    }
}