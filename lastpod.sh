#!/bin/bash

sleep 1

cd "$(dirname "${BASH_SOURCE[0]}"
./lastpod $1 &> log.txt

systemd-umount $1
