#!/bin/bash

if [ "$1" = "debug" ]; then
    # Debug ex
    gnome-terminal --title="server - gdb" -- bash -c "gdb ./build/src/server/server -ex 'set follow-fork-mode child' -ex 'br server.c:217' -ex 'run'; exec bash" &
    # gnome-terminal --title="server - gdb" -- bash -c "gdb ./build/src/server/server -ex 'run' -ex 'set follow-fork-mode child'; exec bash" &
    sleep 1
    # gnome-terminal --title="client6 - gdb" -- bash -c "gdb ./build/src/client/client -ex 'set follow-fork-mode child' -ex 'br client.c:48' -ex 'run localhost 1'; exec bash" &
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
    gnome-terminal --title="server" -- ./build/src/server/server
    sleep 1
    gnome-terminal --title="client4" -- ./build/src/client/client localhost 1
    sleep 1
    gnome-terminal --title="client6" -- ./build/src/client/client localhost 0
fi