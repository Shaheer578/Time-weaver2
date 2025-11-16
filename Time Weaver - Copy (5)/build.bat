@echo off
echo Building Time Weaver ...
gcc -c sqlite3.c -o sqlite3.o
g++ -std=c++11 -O2 server.cpp database.cpp sqlite3.o -o TimeWeaver.exe -I. -lws2_32
if %errorlevel% equ 0 (
    echo Build successful!
    echo DSA: Trie, Priority Queue, LRU Cache, Recurrence Engine
    echo Run TimeWeaver.exe to start the server.
) else (
    echo Build failed!
)
pause
