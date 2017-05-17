#!/bin/bash

echo "insert module"
sudo insmod task6.ko &&

echo "change read/write permissions" &&
sudo chmod 666 /dev/kerneltask

echo "test read operation"
cat /dev/kerneltask

echo "test write operation - wrong value"
echo "echo omerdaagn > /dev/kerneltask"
echo omerdaagn > /dev/kerneltask

echo "test write operation - correct value"
echo "echo omerdagan > /dev/kerneltask"
echo acef8c84aaa6 > /dev/kerneltask

echo "show the kernel log to prove operation"
dmesg

echo "remove the module"
sudo rmmod task6
