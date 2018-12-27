#!/bin/bash

echo "Start GT domain..."

cd ../build/bin/
./gt 192.168.1.102 5555

if [ $? -ne 0 ] ; then
    echo "Failed to start GT. " $?
fi