cd SAE-Bataille-Navale
gcc -o main.exe main.c battleship.c win/server_win.c win/client_win.c -lws2_32
move main.exe ../main.exe