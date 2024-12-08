# Hyperthreading

## Check state
cat /sys/devices/system/cpu/smt/active

## close hyperthreading
sudu -i
echo off > /sys/devices/system/cpu/smt/control

## open hyperthreading
sudu -i
echo on > /sys/devices/system/cpu/smt/control