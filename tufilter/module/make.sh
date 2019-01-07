#!/bin/bash 
sudo make -C /usr/src/linux-headers-4.15.0-43-generic M=$PWD modules
#sudo make -C /usr/src/$(shell uname -r) M=$PWD modules

