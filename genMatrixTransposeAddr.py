#!/usr/bin/python3.8

import sys

f = open('test.cus',"w")

rowA = int(sys.argv[1])
colA = int(sys.argv[2])
rowB = colA
colB = rowA
rowR = rowA
colR = colB

sizeElement = 8

startAddrA = 0
startAddrB = rowA * colA * sizeElement
startAddrR = startAddrB + (rowB * colB * sizeElement)

# iterate through rows of A
for i in range(rowA):
	# iterate through columns of A
	for j in range(colA):
		f.write("R 0x" + format(startAddrA + (i * colA * sizeElement) + (sizeElement * j), "08x") + "\n")
		f.write("W 0x" + format(startAddrR + (j * colR * sizeElement) + (sizeElement * i), "08x") + "\n")
f.close();
