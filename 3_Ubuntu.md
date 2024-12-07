# state
cat /sys/devices/system/cpu/smt/active


# close 
sudu -i
echo off > /sys/devices/system/cpu/smt/control

# open
sudu -i
echo on > /sys/devices/system/cpu/smt/control