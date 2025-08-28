#!/usr/bin/python3.8

import sys

f = open('test.cus',"w")

rowA = int(sys.argv[1])
colA = int(sys.argv[2])
rowB = colA
colB = rowB

sizeElement = 8

startAddrA = 0
startAddrB = rowA * colA * sizeElement
startAddrR = startAddrB + (rowB * colB * sizeElement)

# iterate through rows of A
for i in range(rowA):
	# iterate through columns of B
	for j in range(colB):
		# iterate through rows of B
		for k in range(rowB):
			f.write("R 0x" + format(startAddrA + (i * colA * sizeElement) + (sizeElement * k), "08x") + "\n")
			f.write("R 0x" + format(startAddrB + (k * colB * sizeElement) + (sizeElement * j), "08x") + "\n")
			#f.write("R 0x" + format(startAddrR + (i * colB * sizeElement) + (sizeElement * j), "08x") + "\n")
		f.write("W 0x" + format(startAddrR + (i * colB * sizeElement) + (sizeElement * j), "08x") + "\n")
f.close();
