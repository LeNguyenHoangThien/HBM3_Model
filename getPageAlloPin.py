#!/usr/bin/python3.8
import sys

addrList = []

file = open(sys.argv[1], "r")
for line in file:
	fields = line.split()
	intNum = int(fields[1], 16)
	hexNum = hex(intNum >> 12)
	if(hexNum not in addrList):
		addrList.append(hexNum)
file.close()

addrList.sort()

print(addrList)
