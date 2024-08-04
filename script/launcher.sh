#!/bin/bash

if [ "$1" = "debug" ]; then
    # Debug ex
    gnome-terminal --title="server - gdb" -- bash -c "gdb ./build/src/server/server -ex 'set follow-fork-mode child' -ex 'br server.c:217' -ex 'run'; exec bash" &
    sleep 1
    gnome-terminal --title="client6 - gdb" -- bash -c "gdb ./build/src/client/client -ex 'run localhost 1'; exec bash" &
elif [ "$1" = "valgrind" ]; then
    # Valgrind ex
    gnome-terminal --title="server - valgrind" -- bash -c "valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./build/src/server/server; exec bash" &
    sleep 1
    gnome-terminal --title="client4 - valgrind" -- bash -c "valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./build/src/client/client localhost 1; exec bash" &
    sleep 1
    gnome-terminal --title="client6 - valgrind" -- bash -c "valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./build/src/client/client localhost 0; exec bash" &
else
    # Normal ex
    gnome-terminal --title="server" -- bash -c "./build/src/server/server; exec bash" &
    sleep 1
    gnome-terminal --title="client4" -- bash -c "./build/src/client/client "127.0.0.1" 1; exec bash" &
    sleep 1
    gnome-terminal --title="client6" -- bash -c "./build/src/client/client "::1" 0; exec bash" &
fi
