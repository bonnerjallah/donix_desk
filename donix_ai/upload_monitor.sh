#!/bin/bash

echo "=============="
echo "Bulding and Uploading..."
echo "=============="

if pio run -t upload; then
	echo ""
	echo "================="
	echo "Firmware Upload Successful!"
	echo "================="
else 
	echo ""
	echo "❌ Firmware upload failed."
	exit 1
fi

echo ""
echo "Opening Serial Monitor..."
pio device monitor
