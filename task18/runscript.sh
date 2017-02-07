#!/bin/bash

echo "insert module"
sudo insmod task18.ko &&

echo Alice > /dev/eudyptula
echo Bob > /dev/eudyptula
sleep 15
echo Dave > /dev/eudyptula
echo Gena > /dev/eudyptula

echo "remove the module"
sudo rmmod task18
