- - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - -
-------------------------1. Instrukcja kompilacji---------------------------
- - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - -
Program należy skompilować uruchamiając plik Makefile, lub w przypadku jego
błędu należy skompilować pliki klienta i serwera ręcznie, przy użyciu 
instrukcji:
gcc client.c -o client
gcc server.c -o server
- - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - -
-----------------------2. Instrukcja uruchomienia---------------------------
- - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - -
Aby uruchomić program należy po skompilowaniu, najpierw uruchomić program
serwera, a następnie można uruchamiać program klienta/klientów. 
Menu programu jest intuicyjne, prowadzi do poszukiwanych funkcji, poprzez
wpisywanie numerów poleceń. Aby cokolwiek uczynić należy najpierw się
zalogować.
- - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - -
---------------------3. Opis poszczególnych plików--------------------------
- - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - -
-client.c - plik z kodem programu klienta
-server.c - plik z kodem programu serwera
-structures.ch - plik z kodem struktur używanych w programie
