#!/bin/bash

echo "Stop GT domain..."

cd ../build/bin/

kill -9 `pidof gt` 