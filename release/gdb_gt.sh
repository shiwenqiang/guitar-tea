#!/bin/bash

CMD_PATH=$1

echo "Start GT domain..."

cd ../build/bin/

#if [ $CMD_PATH -e "" ] ; then
#    gdb -ex=r --args ./gt 192.168.1.102 5555 > /dev/null 2>&1
#else
    gdb -x $CMD_PATH -ex=r --args ./gt 192.168.1.102 5555 > /dev/null 2>&1 &
#fi

result=$?

if [ $result -ne 0 ] ; then
    echo "Failed to start GT. ret:" $result
fi
