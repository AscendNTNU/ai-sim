#!/bin/bash
killall a.out
g++ random.cpp
if [ $? -eq 0 ]; then
    echo COMPILED random.cpp SUCCESS
    ./a.out

else
    echo COMPILED random.cpp ERROR
    $SHELL
fi



