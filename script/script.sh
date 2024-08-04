#!/bin/bash

read -p "Enter maxima cantidad de hilos: " int1

for (( i=3; i<=$int1; i++ ))
do
    echo -n "cantidad de hilos $i, ejecucion nro $j"
    echo ""
    ./build/src/server/server $i
    sleep 2
    echo ""
done
echo "finalizacion exitosa"
