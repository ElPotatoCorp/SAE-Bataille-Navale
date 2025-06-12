# Go to project folder
Set-Location -Path "SAE-Bataille-Navale"

# Compile
gcc -o main.exe main.c battleship.c win/server_win.c win/client_win.c -lws2_32

# Move and overwrite main.exe in parent directory
try {
    Move-Item -Path "main.exe" -Destination "..\main.exe" -Force -ErrorAction Stop
    Write-Host "[OK] Le deplacement de main.exe s'est bien passe."
} catch {
    Remove-Item "main.exe" -ErrorAction SilentlyContinue
    Write-Host "[ERREUR] Le deplacement de main.exe a echoue (droits refuses ou fichier en cours d'utilisation)."
}

Set-Location ..

Write-Host "Appuyez sur Entree pour quitter..."
[void][System.Console]::ReadLine()