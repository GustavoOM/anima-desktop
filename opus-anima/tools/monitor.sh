#!/bin/bash

bash ~/Documents/opus-anima/kiosk.sh &

echo "lavg1m lavg5m lavg15m" > cpu.txt
free -m | grep "total" | tr -s " " | cut -d" " -f2-7 > memory.txt
free -m | grep "total" | tr -s " " | cut -d" " -f2-4 > swap.txt

end=$((SECONDS+1728000))
while [ $SECONDS -lt $end ]; do
uptime | awk -F 'load average: ' '{print $2}' >> cpu.txt
free -m | grep "Mem:" | tr -s " " | cut -d" " -f2-7 >> memory.txt
free -m | grep "Swap:" | tr -s " " | cut -d" " -f2-4 >> swap.txt
sleep 60
done
