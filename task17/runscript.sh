#!/bin/bash

echo "insert module"
sudo insmod task6.ko &&

echo "change read/write permissions" &&
sudo chmod 666 /dev/eudyptula

echo "test read operation"
cat /dev/eudyptula

echo "test write operation - wrong value"
echo "echo omerdaagn > /dev/eudyptula"
echo omerdaagn > /dev/eudyptula

echo "test write operation - correct value"
echo "echo omerdagan > /dev/eudyptula"
echo acef8c84aaa6 > /dev/eudyptula

echo "show the kernel log to prove operation"
dmesg

echo "remove the module"
sudo rmmod task6