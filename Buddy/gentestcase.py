#!/usr/bin/python3

f = open("getBuddyStandAloneReport.bash", "w")

#coalescingList = ["BBS_TRAD", "BBS_NAPOT", "BBS_ANCH", "BBS_BCT", "BBS_CAMB", "BBS_RCPT", "BBS_cRCPT"]
coalescingList = ["BBS_TRAD", "BBS_NAPOT", "BBS_CAMB", "BBS_RCPT", "BBS_cRCPT"]

blockList = [2, 4, 8, 16, 32, 64, 128, 256]

#requestList = [900, 2025, 2700, 3600, 8640, 32400]
requestList = [507]
memoryList  = [65536]

################################ IMAGE #############################################################
f.write("#!/bin/bash -f\n\n")

for coalescingType in coalescingList:
	for memorySize in memoryList:
		for requestSize in requestList:
			if(requestSize > memorySize):
				continue
			for blockSize in blockList:
				outFile = coalescingType + "_" + str(blockSize) + "_" + str(requestSize) + "_" + str(memorySize)
				testcaseLine = "./generateBuddyReport.bash" + "\t" + coalescingType + "\t" + str(blockSize) + "\t" + str(requestSize) + "\t" + str(memorySize) + "\t" + "> " + "check_" + outFile + "\n"
				f.write(testcaseLine)

f.write("\n\n")
f.write("FILES=`ls | grep check_`\n")
f.write("for file in $FILES\n")
f.write("do\n")
f.write("\tprintf \"%-40s\" $file\n")
f.write("\tgrep Allocation $file | awk '{printf \"%-10.2f \", $2}'\n")
f.write("\tgrep De-allocation $file | awk '{printf \"%-10.2f \", $2}'\n")
f.write("\tgrep \"Free Nodes\" $file | awk '{printf \"%-10d \", $NF}'\n")
f.write("\tgrep \"allocated node\" $file | awk '{printf \"%-10d \", $NF}'\n")
f.write("\tgrep AvgAddrCovrPTE $file | awk '{printf \"%-10.2f \\n\", $NF}'\n")
f.write("done")

f.close()
