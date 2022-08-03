cd ~/.arduino15/packages/arduino/hardware/sam
VERSION=$(ls | grep "1." | head -n1)
if [ -z $VERSION ]
then
	echo "[ERROR] package version 1.x of arduino sam not found"
	exit 1
fi
cd $VERSION/cores/arduino

LINE_POS=$(cat RingBuffer.h | grep -n "#define SERIAL_BUFFER_SIZE" | cut -d":" -f1)
split RingBuffer.h --lines=$LINE_POS
mv xaa header
mv xab tailer
split header --lines=$((LINE_POS-1))
mv xaa header
mv xab center
echo "#define SERIAL_BUFFER_SIZE 512" > center
cat header > newRingBuffer.h
cat center >> newRingBuffer.h
cat tailer >> newRingBuffer.h
rm header center tailer

DIF=$(diff RingBuffer.h newRingBuffer.h | wc -l)
if [ "$DIF" -le "4" ]
then
	rm RingBuffer.h
	mv newRingBuffer.h RingBuffer.h
	exit 0
else
	rm newRingBuffer.h
	echo "[ERROR] file define update could not be done"
	exit 1
fi
