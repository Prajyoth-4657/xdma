#!/bin/bash

lspci | grep "9034"

sleep 2s

lsmod | grep "xdma"

sleep 2s

sudo rmmod xdma

echo "Removed the xdma driver	$?" 

cd /home/user1/stub_testing/dma_ip_drivers-2019.1/XDMA/linux-kernel/tests
sleep 3s
sudo ./load_driver.sh
 
