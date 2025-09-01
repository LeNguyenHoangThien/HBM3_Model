#!/usr/bin/python3

import math
import random

SEED_INIT = 493

f = open("getTLBCoalescingReport.bash", "w")

#coalescingList = ["BBS_CAMB", "BBS_TRAD", "BBS_ANCH", "BBS_RCPT"]
#coalescingList = ["BBS_BCT", "BBS_CAMB", "BBS_TRAD", "BBS_ANCH", "BBS_PCAD", "BBS_NAPOT", "BBS_RCPT"]
#coalescingList = ["BBS_RCPT", "BBS_CAMB", "BBS_TRAD", "BBS_ANCH", "BBS_PCAD", "PBS_PCAD", "RBS_PCAD", "BBS_NAPOT", "BBS_cRCPT", "BBS_BCT"]
#coalescingList = ["BBS_RCPT", "BBS_CAMB", "BBS_TRAD", "BBS_ANCH", "BBS_PCAD", "BBS_NAPOT", "BBS_cRCPT", "BBS_BCT"]
#coalescingList = ["BBS_TRAD", "BBS_ANCH", "BBS_PCAD", "RBS_PCAD"]
coalescingList = ["BBS_TRAD", "BBS_NAPOT", "BBS_CAMB", "BBS_RCPT", "BBS_cRCPT"]

#blockList = [2, 4, 8, 16, 32, 64, 128, 256]
blockList = [16]

tileList = [4]
#tileList = [4, 8, 16]

#nTotalPages = 131072	# 512 MB

################################## IMAGE #############################################################
#
mainfileList = ["mainImageDisplay.cpp", "mainImageBlending.cpp", "mainImagePreview.cpp", "mainImageScaling.cpp", "mainImageDetection.cpp"]
#mainfileList = ["mainImageDetection.cpp"]
#
#
##requestList = [(1280, 720), (1920, 1080), (2048, 1080), (4096, 2160)]
#requestList = [(1280, 720), (1920, 1080), (2560, 1080), (2560, 1440), (4096, 2160), (7680, 4320)]
requestList = [(1280, 720)]

scenarioList = ["ROTATION", "RASTER_SCAN", "MATRIX_SOBEL"]
#scenarioList = ["ROTATION"]
#scenarioList = ["MATRIX_SOBEL"]

for mainfile in mainfileList:
	for coalescingType in coalescingList:
		for scenarioType in scenarioList:
			if((mainfile == "mainImageBlending.cpp" or mainfile == "mainImageScaling.cpp" or  mainfile == "mainImageDetection.cpp") and scenarioType == "ROTATION"):
				continue
			if(mainfile == "mainImageDetection.cpp" and scenarioType == "RASTER_SCAN"):
				continue
			if(mainfile != "mainImageDetection.cpp" and scenarioType == "MATRIX_SOBEL"):
				continue
			for blockSize in blockList:
				for requestSize in requestList:
					nTotalPages = pow(2, int(math.ceil(math.log2(requestSize[0] * requestSize[1] * 4 / 4096))))
					if(mainfile == "mainImageBlending.cpp" or mainfile == "mainImageScaling.cpp"):
						nTotalPages *= 2
					if(mainfile == "mainImageDetection.cpp"):
						nTotalPages *= 3
					outFile = coalescingType + "_" + (mainfile.replace(".cpp", "_")).replace("main", "") + scenarioType.replace("_", "") + "_" + str(blockSize) + "_" + str(requestSize[0]) + "_" + str(requestSize[1]) + "_4"
					testcaseLine = "make" + "\t" + coalescingType + "\t" + "SCENARIO=" + scenarioType + "\t" + "BLOCKSIZE_INIT=" + str(blockSize) + "\t" + "IMG_HORIZONTAL_SIZE=" + str(requestSize[0]) + "\t" + "IMG_VERTICAL_SIZE=" + str(requestSize[1]) + "\t" + "MAINFILE=" + mainfile + "\t" + "NUM_TOTAL_PAGES=" + str(nTotalPages) +  "\t" + "TILE_SIZE=4" + "\t" + "SEED_INIT=" + str(SEED_INIT) + "\t" +  "; ./" + outFile + "\t" + "> check_" + outFile + "\t ; rm -f " + outFile + "\n"
					f.write(testcaseLine)
#
#################################### MATRIX  #########################################################

#mainfile = "mainMatrixc4m4.cpp"
mainfile = "mainMatrix.cpp"


#requestList = [(512, 512), (640, 640), (720, 720)]
requestList = [(640, 640)]
#requestList = [(720, 720)]

#scenarioList = ["MATRIX_TRANSPOSE", "MATRIX_SOBEL", "MATRIX_CONVOLUTION", "MATRIX_MULTIPLICATION"]
#scenarioList = ["MATRIX_SOBEL"]
#scenarioList = ["MATRIX_TRANSPOSE", "MATRIX_SOBEL", "MATRIX_CONVOLUTION"]
#scenarioList = ["MATRIX_MULTIPLICATION"]
#scenarioList = ["MATRIX_TRANSPOSE"]
scenarioList = ["MATRIX_CONVOLUTION", "MATRIX_TRANSPOSE", "MATRIX_MULTIPLICATION"]

for scenarioType in scenarioList:
	for coalescingType in coalescingList:
		for blockSize in blockList:
			for requestSize in requestList:
				nTotalPages = pow(2, int(math.ceil(math.log2(requestSize[0] * requestSize[1] * 4 / 4096)))) * 3
				if scenarioType == "MATRIX_MULTIPLICATION":
					for tileSize in tileList:
						outFile = coalescingType + "_" + (mainfile.replace(".cpp", "_")).replace("main", "") + scenarioType.replace("_", "") + "_" + str(blockSize) + "_" + str(requestSize[0]) + "_" + str(requestSize[1]) + "_" + str(tileSize)
						testcaseLine = "make" + "\t" + coalescingType + "\t" + "SCENARIO=" + scenarioType + "\t" + "BLOCKSIZE_INIT=" + str(blockSize) + "\t" + "IMG_HORIZONTAL_SIZE=" + str(requestSize[0]) + "\t" + "IMG_VERTICAL_SIZE=" + str(requestSize[1]) + "\t" + "MAINFILE=" + mainfile + "\t" + "NUM_TOTAL_PAGES=" + str(nTotalPages) + "\t" + "TILE_SIZE=" + str(tileSize) +  "\t" + "SEED_INIT=" + str(SEED_INIT) + "\t" + "; ./" + outFile + "\t" + "> check_" + outFile + "\t ; rm -f " + outFile + "\n"
						f.write(testcaseLine)
				else:
					outFile = coalescingType + "_" + (mainfile.replace(".cpp", "_")).replace("main", "") + scenarioType.replace("_", "") + "_" + str(blockSize) + "_" + str(requestSize[0]) + "_" + str(requestSize[1]) + "_" + "4"
					testcaseLine = "make" + "\t" + coalescingType + "\t" + "SCENARIO=" + scenarioType + "\t" + "BLOCKSIZE_INIT=" + str(blockSize) + "\t" + "IMG_HORIZONTAL_SIZE=" + str(requestSize[0]) + "\t" + "IMG_VERTICAL_SIZE=" + str(requestSize[1]) + "\t" + "MAINFILE=" + mainfile + "\t" + "NUM_TOTAL_PAGES=" + str(nTotalPages) + "\t" + "TILE_SIZE=4" + "\t" + "SEED_INIT=" + str(SEED_INIT) + "\t" + "; ./" + outFile + "\t" + "> check_" + outFile + "\t ; rm -f " + outFile + "\n"
					f.write(testcaseLine)


f.close()
