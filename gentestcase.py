#!/usr/bin/python3

import math
import random

SEED_INIT = 493

f = open("getTLBCoalescingReport_BGFAM_48.bash", "w")

coalescingList = ["BBS_TRAD", "BBS_TRAD_SELECTED"]

#blockList = [2, 4, 8, 16, 32, 64, 128, 256]
blockList = [16]

tileList = [4]
#tileList = [4, 8, 16]

#nTotalPages = 131072	# 512 MB

ARList = ["AR_LIAM", "AR_BFAM", "AR_BGFAM"]
AWList = ["AW_LIAM", "AW_BFAM", "AW_BGFAM"]
BankList = [48]


################################## IMAGE #############################################################
#
mainfileList = ["mainImageDisplay.cpp", "mainImageBlending.cpp", "mainImagePreview.cpp", "mainImageScaling.cpp"]
#mainfileList = ["mainImageDetection.cpp"]
#mainfileList = ["mainImagePreview.cpp", "mainImageDisplay.cpp"]
#
#requestList = [(128, 128), (256, 256), (384, 384), (512, 512), (640, 640), (768, 768), (896, 896), (1024, 1024), (1152, 1152), (1280, 1280), (1408, 1408), (1536, 1536), (1664, 1664), (1792, 1792), (1920, 1920), (2048, 2048), (2176, 2176), (2304, 2304), (2432, 2432), (2560, 2560), (2688, 2688), (2816, 2816), (2944, 2944), (3072, 3072), (3200, 3200), (3328, 3328), (3456, 3456), (3584, 3584), (3712, 3712), (3840, 3840), (3968, 3968), (4096, 4096)] # For 32-banks
#requestList = [(1024, 128), (2048, 256), (3072, 384), (4096, 512), (5120, 640), (6144, 768), (7168, 896), (8192, 1024), (9216, 1152), (10240, 1280), (11264, 1408), (12288, 1536), (13312, 1664), (14336, 1792), (15360, 1920), (16384, 2048), (17408, 2176), (18432, 2304), (19456, 2432), (20480, 2560), (21504, 2688), (22528, 2816), (23552, 2944), (24576, 3072), (25600, 3200), (26624, 3328), (27648, 3456), (28672, 3584), (29696, 3712), (30720, 3840), (31744, 3968), (32768, 4096)] # For 32-banks


#requestList = [(192, 192), (384, 384), (576, 576), (768, 768), (960, 960), (1152, 1152), (1344, 1344), (1536, 1536), (1728, 1728), (1920, 1920), (2112, 2112), (2304, 2304), (2496, 2496), (2688, 2688), (2880, 2880), (3072, 3072), (3264, 3264), (3456, 3456), (3648, 3648), (3840, 3840), (4032, 4032), (4224, 4224), (4416, 4416), (4608, 4608), (4800, 4800), (4992, 4992), (5184, 5184), (5376, 5376), (5568, 5568), (5760, 5760), (5952, 5952), (6144, 6144)] # For 48-banks
requestList = [(1536, 192), (3072, 384), (4608, 576), (6144, 768), (7680, 960), (9216, 1152), (10752, 1344), (12288, 1536), (13824, 1728), (15360, 1920), (16896, 2112), (18432, 2304), (19968, 2496), (21504, 2688), (23040, 2880), (24576, 3072), (26112, 3264), (27648, 3456), (29184, 3648), (30720, 3840), (32256, 4032), (33792, 4224), (35328, 4416), (36864, 4608), (38400, 4800), (39936, 4992), (41472, 5184), (43008, 5376), (44544, 5568), (46080, 5760), (47616, 5952), (49152, 6144)] # For 48-banks


#requestList = [(256, 256), (512, 512), (768, 768), (1024, 1024), (1280, 1280), (1536, 1536), (1792, 1792), (2048, 2048), (2304, 2304), (2560, 2560), (2816, 2816), (3072, 3072), (3328, 3328), (3584, 3584), (3840, 3840), (4096, 4096), (4352, 4352), (4608, 4608), (4864, 4864), (5120, 5120), (5376, 5376), (5632, 5632), (5888, 5888), (6144, 6144), (6400, 6400), (6656, 6656), (6912, 6912), (7168, 7168), (7424, 7424), (7680, 7680), (7936, 7936), (8192, 8192)] # For 64-banks
#requestList = [(2048, 256), (4096, 512), (6144, 768), (8192, 1024), (10240, 1280), (12288, 1536), (14336, 1792), (16384, 2048), (18432, 2304), (20480, 2560), (22528, 2816), (24576, 3072), (26624, 3328), (28672, 3584), (30720, 3840), (32768, 4096), (34816, 4352), (36864, 4608), (38912, 4864), (40960, 5120), (43008, 5376), (45056, 5632), (47104, 5888), (49152, 6144), (51200, 6400), (53248, 6656), (55296, 6912), (57344, 7168), (59392, 7424), (61440, 7680), (63488, 7936), (65536, 8192)] # For 64-banks

#requestList = [(512, 64), (1024, 128), (1536, 192), (2048, 256), (2560, 320), (3072, 384), (3584, 448), (4096, 512), (4608, 576), (5120, 640), (5632, 704), (6144, 768), (6656, 832), (7168, 896), (7680, 960), (8192, 1024), (8704, 1088), (9216, 1152), (9728, 1216), (10240, 1280), (10752, 1344), (11264, 1408), (11776, 1472), (12288, 1536), (12800, 1600), (13312, 1664), (13824, 1728), (14336, 1792), (14848, 1856), (15360, 1920), (15872, 1984), (16384, 2048)]	# For 16-banks

#scenarioList = ["ROTATION", "RASTER_SCAN", "MATRIX_SOBEL"]
#scenarioList = ["ROTATION", "RASTER_SCAN"]
scenarioList = ["ROTATION"]
#scenarioList = ["MATRIX_SOBEL"]

for mainfile in mainfileList:
	for coalescingType in coalescingList:
		for scenarioType in scenarioList:
				for ARType in ARList:
					for AWType in AWList:
						for BankNum in BankList:
							if((mainfile == "mainImageBlending.cpp" or mainfile == "mainImageScaling.cpp" or  mainfile == "mainImageDetection.cpp") and scenarioType == "ROTATION"):
								continue
							if(mainfile == "mainImageDetection.cpp" and scenarioType == "RASTER_SCAN"):
								continue
							if(mainfile != "mainImageDetection.cpp" and scenarioType == "MATRIX_SOBEL"):
								continue
							if(ARType == "AR_HBM_INTERLEAVE" and AWType != "AW_HBM_INTERLEAVE"):
								continue
							if(AWType == "AW_HBM_INTERLEAVE" and ARType != "AR_HBM_INTERLEAVE"):
								continue
							if (coalescingType == "BBS_TRAD_BANK_SHUFFLE_PLUS" and (ARType == "AR_HBM_INTERLEAVE" or AWType == "AW_HBM_INTERLEAVE")):
								continue
							if (coalescingType == "BBS_TRAD_BANK_SHUFFLE_MINUS" and (ARType == "AR_HBM_INTERLEAVE" or AWType == "AW_HBM_INTERLEAVE")):
								continue
							if (coalescingType == "BBS_TRAD_BANK_SHUFFLE_PLUS" and ARType[2:] != AWType[2:]):
								continue
							if (coalescingType == "BBS_TRAD_BANK_SHUFFLE_MINUS" and ARType[2:] != AWType[2:]):
								continue
							if (coalescingType == "BBS_TRAD_SELECTED" and (ARType != "AR_LIAM" or AWType != "AW_LIAM")):
								continue
							if (ARType == "AR_BFAM" and AWType == "AW_LIAM"):
								continue
							if (ARType[2:] != AWType[2:]):
								continue

							for blockSize in blockList:
								for requestSize in requestList:
									if (requestSize[0] > 2048 and (coalescingType == "BBS_TRAD_BANK_SHUFFLE_MINUS" or coalescingType == "BBS_TRAD_BANK_SHUFFLE_PLUS")):
										continue
									nTotalPages = pow(2, int(math.ceil(math.log2(requestSize[0] * requestSize[1] * 4 / 4096))))
									if(mainfile == "mainImageBlending.cpp" or mainfile == "mainImageScaling.cpp"):
										nTotalPages *= 2
									if(mainfile == "mainImageDetection.cpp"):
										nTotalPages *= 3
									outFile = coalescingType + "_" + (mainfile.replace(".cpp", "_")).replace("main", "") + scenarioType.replace("_", "") + "_" + str(ARType) + "_" + str(AWType) + "_" + str(BankNum) + "_" + str(blockSize) + "_" + str(requestSize[0]) + "_" + str(requestSize[1]) + "_4"
									testcaseLine = "make" + "\t" + coalescingType + "\t" + "AR=" + ARType + "\t" + "AW=" + AWType + "\t" + "BANK_NUM=" + str(BankNum) + "\t" + "SCENARIO=" + scenarioType + "\t" + "BLOCKSIZE_INIT=" + str(blockSize) + "\t" + "IMG_HORIZONTAL_SIZE=" + str(requestSize[0]) + "\t" + "IMG_VERTICAL_SIZE=" + str(requestSize[1]) + "\t" + "MAINFILE=" + mainfile + "\t" + "NUM_TOTAL_PAGES=" + str(nTotalPages) +  "\t" + "TILE_SIZE=4" + "\t" + "SEED_INIT=" + str(SEED_INIT) + "\t" +  "; nohup ./" + outFile + "\t" + "> check_" + outFile + " & \n"
									f.write(testcaseLine)
	#
#################################### MATRIX  #########################################################

#mainfile = "mainMatrixc4m4.cpp"
#mainfile = "mainMatrix.cpp"


#requestList = [(512, 512), (640, 640), (720, 720)]
#requestList = [(640, 640)]
#requestList = [(720, 720)]

#scenarioList = ["MATRIX_TRANSPOSE", "MATRIX_SOBEL", "MATRIX_CONVOLUTION", "MATRIX_MULTIPLICATION"]
#scenarioList = ["MATRIX_SOBEL"]
#scenarioList = ["MATRIX_TRANSPOSE", "MATRIX_SOBEL", "MATRIX_CONVOLUTION"]
#scenarioList = ["MATRIX_MULTIPLICATION"]
#scenarioList = ["MATRIX_TRANSPOSE"]
#scenarioList = ["MATRIX_CONVOLUTION", "MATRIX_TRANSPOSE", "MATRIX_MULTIPLICATION"]

#for scenarioType in scenarioList:
#	for coalescingType in coalescingList:
#		for blockSize in blockList:
#			for requestSize in requestList:
#				nTotalPages = pow(2, int(math.ceil(math.log2(requestSize[0] * requestSize[1] * 4 / 4096)))) * 3
#				if scenarioType == "MATRIX_MULTIPLICATION":
#					for tileSize in tileList:
#						outFile = coalescingType + "_" + (mainfile.replace(".cpp", "_")).replace("main", "") + scenarioType.replace("_", "") + "_" + str(blockSize) + "_" + str(requestSize[0]) + "_" + str(requestSize[1]) + "_" + str(tileSize)
#						testcaseLine = "make" + "\t" + coalescingType + "\t" + "SCENARIO=" + scenarioType + "\t" + "BLOCKSIZE_INIT=" + str(blockSize) + "\t" + "IMG_HORIZONTAL_SIZE=" + str(requestSize[0]) + "\t" + "IMG_VERTICAL_SIZE=" + str(requestSize[1]) + "\t" + "MAINFILE=" + mainfile + "\t" + "NUM_TOTAL_PAGES=" + str(nTotalPages) + "\t" + "TILE_SIZE=" + str(tileSize) +  "\t" + "SEED_INIT=" + str(SEED_INIT) + "\t" + "; ./" + outFile + "\t" + "> check_" + outFile + "\t ; rm -f " + outFile + "\n"
#						f.write(testcaseLine)
#				else:
#					outFile = coalescingType + "_" + (mainfile.replace(".cpp", "_")).replace("main", "") + scenarioType.replace("_", "") + "_" + str(blockSize) + "_" + str(requestSize[0]) + "_" + str(requestSize[1]) + "_" + "4"
#					testcaseLine = "make" + "\t" + coalescingType + "\t" + "SCENARIO=" + scenarioType + "\t" + "BLOCKSIZE_INIT=" + str(blockSize) + "\t" + "IMG_HORIZONTAL_SIZE=" + str(requestSize[0]) + "\t" + "IMG_VERTICAL_SIZE=" + str(requestSize[1]) + "\t" + "MAINFILE=" + mainfile + "\t" + "NUM_TOTAL_PAGES=" + str(nTotalPages) + "\t" + "TILE_SIZE=4" + "\t" + "SEED_INIT=" + str(SEED_INIT) + "\t" + "; ./" + outFile + "\t" + "> check_" + outFile + "\t ; rm -f " + outFile + "\n"
#					f.write(testcaseLine)


f.close()
