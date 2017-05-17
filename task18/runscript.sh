#!/bin/bash

echo "insert module"
sudo insmod task18.ko &&

echo Alice > /dev/kerneltask
echo Bob > /dev/kerneltask
sleep 15
echo Dave > /dev/kerneltask
echo Gena > /dev/kerneltask

echo "remove the module"
sudo rmmod task18
