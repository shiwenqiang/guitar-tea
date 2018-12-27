#!/bin/bash

echo "Stop GT domain..."

cd ../build/bin/

kill -9 `pidof gt` > /dev/null 2>&1
result=$?

if [ $result -ne 0 ]; then
    echo "Failed to stop GT. ret:" $result
fi