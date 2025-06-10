# SAE-Bataille-Navale
Un bataille navale en LAN

If you get an error like this : 
```bash
gcc -o main.exe main.c battleship.c win/server_win.c win/client_win.c -lws2_32
win/client_win.c: In function 'connect_to_server':
win/client_win.c:35:9: warning: implicit declaration of function 'inet_pton' [-Wimplicit-function-declaration]
     if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) != 1) {
         ^~~~~~~~~
C:\Users\XXX\AppData\Local\Temp\ccMLGspN.o:client_win.c:(.text+0x13c): undefined reference to `inet_pton'
```

Please update your MinGW version to a newer one (the lastest if possible).