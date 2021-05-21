#!/usr/bin/env bash

echo "Flashing $1 via $2"
arm-none-eabi-objcopy -O binary $1 $1.bin
sudo python3 $ASB_PATH/asb.py --bin $1.bin --load-address-blob 0x20000 --magic-num 0xCB \
     -o $1.ota.bin --version 0x0 --load-address-wired 0xC000 -i 6 --options 0x1 -b 115200 -port $2 -r 2 -v
