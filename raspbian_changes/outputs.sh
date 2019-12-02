#!/bin/sh

out1=26
out2=25
out3=24
out4=23
out5=22

cd /sys/class/gpio/

echo $out1 > export
echo $out2 > export
echo $out3 > export
echo $out4 > export
echo $out5 > export

echo out > gpio${out1}/direction
echo out > gpio${out2}/direction
echo out > gpio${out3}/direction
echo out > gpio${out4}/direction
echo out > gpio${out5}/direction

