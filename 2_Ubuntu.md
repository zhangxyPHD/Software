# Hyperthreading

## Check state
cat /sys/devices/system/cpu/smt/active

## close hyperthreading
sudo -i
echo off > /sys/devices/system/cpu/smt/control

## open hyperthreading
sudo -i
echo on > /sys/devices/system/cpu/smt/control


# clearMem
alias clearMem='sudo bash -c "echo 1 > /proc/sys/vm/drop_caches"'

# install software from .deb file

# Check the size of folders and disc space


```
<!--0: Current directory-->
du -h --max-depth=0 
<!-- 1: Current and next level directory -->
du -h --max-depth=1

```
du -sh

du -h
du -h a.txt

du -h ftp
du -hs ftp

```
