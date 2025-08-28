#!/usr/bin/python3

import xlsxwriter
import sys

# sys.argv[1] : Source
# sys.argv[2] : report
# sys.argv[3] : hostCoalescing

class testcaseClass:
	def __init__(self, buddyType, coalescing, app, scenario, blockSize, requestSize, tileSize, cycle, hitRatio, ptw):
		self.buddyType   = buddyType
		self.coalescing  = coalescing
		self.app		 = app
		self.scenario	 = scenario
		self.blockSize   = blockSize
		self.requestSize = requestSize
		self.tileSize    = tileSize
		self.cycle	     = cycle
		self.hitRatio	 = hitRatio
		self.ptw		 = ptw
		
	def printTestcase(self):
		print(self.buddyType, self.coalescing, self.app, self.scenario, self.blockSize, self.requestSize, self.tileSize, self.cycle, self.hitRatio, self.ptw)
 
testcaseList = []

# load testcases from file to testcaseList
file = open(sys.argv[1], "r")
for line in file:
	print(line)
	fields = line.split()
	
	buddyFields = fields[0].split("_")
	buddyType   = buddyFields[1]
	coalescing  = buddyFields[2]
	app			= buddyFields[3]
	scenario	= buddyFields[4]
	blockSize   = int(buddyFields[5])
	requestSize = int(int(buddyFields[6]) * int(buddyFields[7]) / 1024)
	tileSize    = int(buddyFields[8])

	cycle	    = int(fields[1])
	hitRatio	= float(fields[2])
	ptw			= int(fields[3])

	testcaseObj = testcaseClass(buddyType, coalescing, app, scenario, blockSize, requestSize, tileSize, cycle, hitRatio, ptw)
	testcaseList.append(testcaseObj)
file.close()
# END: load testcases from file to testcaseList
#testcaseList.sort(key=lambda testcaseClass : (testcaseClass.app, testcaseClass.scenario, testcaseClass.blockSize, testcaseClass.requestSize, testcaseClass.buddyType))
#testcaseList.sort(key=lambda testcaseClass : (testcaseClass.app, testcaseClass.scenario, testcaseClass.blockSize, testcaseClass.requestSize, testcaseClass.tileSize, testcaseClass.coalescing))
testcaseList.sort(key=lambda testcaseClass : (testcaseClass.app, testcaseClass.scenario, testcaseClass.requestSize, testcaseClass.blockSize, testcaseClass.tileSize, testcaseClass.coalescing))

workbook = xlsxwriter.Workbook(sys.argv[2])
worksheet = workbook.add_worksheet()

# Build xlsm report
numberOfField = 5
numberOfCriteria = 3	#(execution cycle, TLB hit, number of PTW)
#caseList = ["BBS_TRAD", "BBS_ANCH", "BBS_PCAD", "BBS_NAPOT", "BBS_BCT", "BBS_CAMB", "BBS_RCPT", "PBS_PCAD", "RBS_PCAD", "BBS_cRCPT"]
caseList = ["BBS_TRAD", "BBS_ANCH", "BBS_PCAD", "BBS_NAPOT", "BBS_BCT", "BBS_CAMB", "BBS_RCPT", "BBS_cRCPT"]
numberOfCase  = len(caseList)
for i in range(numberOfCase):
	worksheet.merge_range(0, numberOfField + i * numberOfCriteria, 0, numberOfField + i * numberOfCriteria + (numberOfCriteria - 1), caseList[i],  workbook.add_format({'align' : 'center'}))
	worksheet.merge_range(0, numberOfField + (numberOfCase + i) * numberOfCriteria, 0, numberOfField + (numberOfCase + i) * numberOfCriteria + (numberOfCriteria - 1), "cmp" + caseList[i],  workbook.add_format({'align' : 'center'}))
	

#worksheet.merge_range(0, numberOfField + 0 * numberOfCriteria, 0, numberOfField + 0 * numberOfCriteria + (numberOfCriteria - 1), "BBS_TRAD",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + 1 * numberOfCriteria, 0, numberOfField + 1 * numberOfCriteria + (numberOfCriteria - 1), "BBS_ANCH",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + 2 * numberOfCriteria, 0, numberOfField + 2 * numberOfCriteria + (numberOfCriteria - 1), "BBS_PCAD",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + 3 * numberOfCriteria, 0, numberOfField + 3 * numberOfCriteria + (numberOfCriteria - 1), "BBS_NAPOT", workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + 4 * numberOfCriteria, 0, numberOfField + 4 * numberOfCriteria + (numberOfCriteria - 1), "BBS_BCT",   workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + 5 * numberOfCriteria, 0, numberOfField + 5 * numberOfCriteria + (numberOfCriteria - 1), "BBS_CAMB",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + 6 * numberOfCriteria, 0, numberOfField + 6 * numberOfCriteria + (numberOfCriteria - 1), "BBS_RCPT",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + 7 * numberOfCriteria, 0, numberOfField + 7 * numberOfCriteria + (numberOfCriteria - 1), "PBS_PCAD",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + 8 * numberOfCriteria, 0, numberOfField + 8 * numberOfCriteria + (numberOfCriteria - 1), "RBS_PCAD",  workbook.add_format({'align' : 'center'}))

#worksheet.merge_range(0, numberOfField + (numberOfCase + 0) * numberOfCriteria, 0, numberOfField + (numberOfCase + 0) * numberOfCriteria + (numberOfCriteria - 1), "cmpBBS_TRAD",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + (numberOfCase + 1) * numberOfCriteria, 0, numberOfField + (numberOfCase + 1) * numberOfCriteria + (numberOfCriteria - 1), "cmpBBS_ANCH",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + (numberOfCase + 2) * numberOfCriteria, 0, numberOfField + (numberOfCase + 2) * numberOfCriteria + (numberOfCriteria - 1), "cmpBBS_PCAD",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + (numberOfCase + 3) * numberOfCriteria, 0, numberOfField + (numberOfCase + 3) * numberOfCriteria + (numberOfCriteria - 1), "cmpBBS_NAPOT", workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + (numberOfCase + 4) * numberOfCriteria, 0, numberOfField + (numberOfCase + 4) * numberOfCriteria + (numberOfCriteria - 1), "cmpBBS_BCT",   workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + (numberOfCase + 5) * numberOfCriteria, 0, numberOfField + (numberOfCase + 5) * numberOfCriteria + (numberOfCriteria - 1), "cmpBBS_CAMB",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + (numberOfCase + 6) * numberOfCriteria, 0, numberOfField + (numberOfCase + 6) * numberOfCriteria + (numberOfCriteria - 1), "cmpBBS_RCPT",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + (numberOfCase + 7) * numberOfCriteria, 0, numberOfField + (numberOfCase + 7) * numberOfCriteria + (numberOfCriteria - 1), "cmpPBS_PCAD",  workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, numberOfField + (numberOfCase + 8) * numberOfCriteria, 0, numberOfField + (numberOfCase + 8) * numberOfCriteria + (numberOfCriteria - 1), "cmpRBS_PCAD",  workbook.add_format({'align' : 'center'}))

worksheet.write(1, 0, "APPLICATION")
worksheet.write(1, 1, "SCENARIO")
worksheet.write(1, 2, "BlockSize")
worksheet.write(1, 3, "ReqSize")
worksheet.write(1, 4, "TileSize")

#for i in [4, 7, 10, 13, 16, 19, 22, 25]:
for i in range(numberOfField, numberOfField + numberOfCase * numberOfCriteria, numberOfCriteria):
	worksheet.write(1, i,   "Cycle")
	worksheet.write(1, i+1, "TLBHit%")
	worksheet.write(1, i+2, "PTW")

for i in range(numberOfField + numberOfCase * numberOfCriteria, numberOfField + numberOfCase * numberOfCriteria * 2, numberOfCriteria):
	worksheet.write(1, i,   "Cycle(%)")
	worksheet.write(1, i+1, "TLBHit(%)")
	worksheet.write(1, i+2, "PTW(%)")

myDataMatrixRow = list()
myDataMatrixCol = [0 for i in range(numberOfField + numberOfCase * numberOfCriteria * 2)]
print("length of myDataMatrixCol: ", len(myDataMatrixCol))

row = 2
shift = 0
hostCol = -1

for i in range(numberOfCase):
	if(sys.argv[3] == caseList[i]):
		hostCol = numberOfField + numberOfCriteria * i
		break
	if(i == numberOfCase - 1):
		print("ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		quit()

#if(sys.argv[3] == "BBS_ANCH"):
#	hostCol = numberOfField + numberOfCriteria * 1
#elif(sys.argv[3] == "BBS_PCAD"):
#	hostCol = numberOfField + numberOfCriteria * 2
#elif(sys.argv[3] == "BBS_NAPOT"):
#	hostCol = numberOfField + numberOfCriteria * 3
#elif(sys.argv[3] == "BBS_BCT"):
#	hostCol = numberOfField + numberOfCriteria * 4
#elif(sys.argv[3] == "BBS_CAMB"):
#	hostCol = numberOfField + numberOfCriteria * 5
#elif(sys.argv[3] == "BBS_RCPT"):
#	hostCol = numberOfField + numberOfCriteria * 6
#elif(sys.argv[3] == "PBS_PCAD"):
#	hostCol = numberOfField + numberOfCriteria * 7
#elif(sys.argv[3] == "RBS_PCAD"):
#	hostCol = numberOfField + numberOfCriteria * 8
#else:
#	print("ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
#	quit()

for e in testcaseList:
	e.printTestcase()

	worksheet.write(row, 0, e.app)
	worksheet.write(row, 1, e.scenario)
	worksheet.write(row, 2, e.blockSize)
	worksheet.write(row, 3, e.requestSize)
	worksheet.write(row, 4, e.tileSize)


	for i in range(numberOfCase):
		case = caseList[i]
		buddySys   = case[:3]
		coalesType = case[4:]
		if(e.buddyType == buddySys and e.coalescing == coalesType):
			shift = numberOfCriteria * i
			break
		if(i == numberOfCase - 1):
			print("ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			quit()

#	if(e.buddyType == "BBS" and e.coalescing == "TRAD"):
#		shift = numberOfCriteria * 0 
#	elif(e.buddyType == "BBS" and e.coalescing == "ANCH"):
#		shift = numberOfCriteria * 1
#	elif(e.buddyType == "BBS" and e.coalescing == "PCAD"):
#		shift = numberOfCriteria * 2
#	elif(e.buddyType == "BBS" and e.coalescing == "NAPOT"):
#		shift = numberOfCriteria * 3
#	elif(e.buddyType == "BBS" and e.coalescing == "BCT"):
#		shift = numberOfCriteria * 4
#	elif(e.buddyType == "BBS" and e.coalescing == "CAMB"):
#		shift = numberOfCriteria * 5
#	elif(e.buddyType == "BBS" and e.coalescing == "RCPT"):
#		shift = numberOfCriteria * 6
#	elif(e.buddyType == "PBS"):
#		shift = numberOfCriteria * 7
#	elif(e.buddyType == "RBS"):
#		shift = numberOfCriteria * 8
#	else:
#		shift = numberOfCriteria * 9

	worksheet.write(row, shift + numberOfField + 0, e.cycle)
	worksheet.write(row, shift + numberOfField + 1, e.hitRatio)
	worksheet.write(row, shift + numberOfField + 2, e.ptw)

	myDataMatrixCol[0] = e.app
	myDataMatrixCol[1] = e.scenario
	myDataMatrixCol[2] = e.blockSize
	myDataMatrixCol[3] = e.requestSize
	myDataMatrixCol[4] = e.tileSize
	myDataMatrixCol[shift + numberOfField + 0] = e.cycle
	myDataMatrixCol[shift + numberOfField + 1] = e.hitRatio
	myDataMatrixCol[shift + numberOfField + 2] = e.ptw

	#if(e.buddyType == "RBS"):	# Please carefull, must sort logfile before run this script
	#if(e.buddyType == "PBS"):	# Please carefull, must sort logfile before run this script
	#if(e.coalescing == "TRAD"):	# Please carefull, must sort logfile before run this script
	if(e.coalescing == "cRCPT"):	# Please carefull, must sort logfile before run this script
		myDataMatrixRow.append(myDataMatrixCol)

		# Compare result
		hostTime = myDataMatrixCol[hostCol + 0]
		hostHit  = myDataMatrixCol[hostCol + 1]
		hostPTW  = myDataMatrixCol[hostCol + 2]
		

		for col in range(numberOfField, numberOfField + numberOfCase * numberOfCriteria, numberOfCriteria):
			clientTime = myDataMatrixCol[col + 0]
			clientHit  = myDataMatrixCol[col + 1]
			clientPTW  = myDataMatrixCol[col + 2]
			if(clientTime == 0):
				clientTime = 1
			if(clientPTW == 0):
				clientPTW = 1

			myDataMatrixCol[col + numberOfCase * numberOfCriteria + 0] = round(100 * (clientTime - hostTime) / clientTime, 2)
			myDataMatrixCol[col + numberOfCase * numberOfCriteria + 1] = round(       hostHit    - clientHit             , 2)
			myDataMatrixCol[col + numberOfCase * numberOfCriteria + 2] = round(100 * (clientPTW  - hostPTW)  / clientPTW , 2)

			worksheet.write(row, col + numberOfCase * numberOfCriteria + 0, myDataMatrixCol[col + numberOfCase * numberOfCriteria + 0])
			worksheet.write(row, col + numberOfCase * numberOfCriteria + 1, myDataMatrixCol[col + numberOfCase * numberOfCriteria + 1])
			worksheet.write(row, col + numberOfCase * numberOfCriteria + 2, myDataMatrixCol[col + numberOfCase * numberOfCriteria + 2])

		myDataMatrixCol = [0 for i in range(numberOfField + numberOfCase * numberOfCriteria * 2)]
		row += 1

for line in range(row - 2):
	print(myDataMatrixRow[line])


# Draw chart
criteria  = ['Execution cycles (millions)', 'TLB hit rate (%)']
#caseName  = ['TRAD', 'ANCH', 'PCAD', 'NAPOT', 'BCT', 'CAMB', 'RCPT', 'PBS', 'RBS']
#["BBS_TRAD", "BBS_ANCH", "BBS_PCAD", "BBS_NAPOT", "BBS_CAMB", "BBS_RCPT", "BBS_cRCPT"]
caseName = []
for i in range(numberOfCase):
	case = caseList[i]
	buddySys   = case[:3]
	coalesType = case[4:]
	caseName.append(coalesType)


#The order in appName is very important, make sure to arrange in the correct order of application name, then scenario

# Image
#appName = ['Image Blending', 'Edge Detection', 'Image Display', 'Rotated Display', 'Image Preview', 'Rotated Preview', 'Image Scaling']
#imageSize = ['1280x720', '1920x1080', '2560x1080', '2560x1440', '4096x2160', '7680x4320']

# Matrix
appName   = ['Matrix Convolution', 'Matrix Multiplication', 'Matrix Transpose']
imageSize = ['512x512', '640x640', '720x720']

START_NEW_AREA = 115
positionR = [x for x in range(340, 340 + 7 * 16 , 16)]
positionC = [x for x in range(  1,       7 *  8 ,  8)]


stepCol  = numberOfCriteria
stepRow  = 8
for jCrit in range(len(criteria)):
	startCol = 5 + jCrit
	startRow = 2
	for chartOrder in range(len(appName)):
		for iSize in range(len(imageSize)):
			chart = workbook.add_chart({'type': 'column'})
			
			for i in range(len(caseName)):
				#modify chart here, ignore case based on caseName
			  if(i == 1 or i == 2 or i == 4): continue
			  chart.add_series({
			    'name'       : caseName[i],
			    'categories' : ['Sheet1', 2, 2, 9, 2],
			    'values'     : ['Sheet1', startRow, startCol + stepCol * i, startRow + stepRow - 1, startCol + stepCol * i],
			    'gap': 75
			  })
			#end for
	
			startRow += stepRow
			
			chart.set_x_axis({
			  'name'     : 'Block size (pages)',
			  'name_font': {
			    'size': 12,
			    'name': 'Times New Roman'
			  },
			  'num_font': {
			    'size': 12,
			    'name': 'Times New Roman'
			  },
			  'line': {'color': 'black'}
			})
			
			if(jCrit == 0):	# EXECUTION CYCLES
				chart.set_y_axis({
				  'name'     : criteria[jCrit],
				  'name_font': {
				    'size': 12,
				    'name': 'Times New Roman'
				  },
				  'num_font': {
				    'size': 12,
				    'name': 'Times New Roman'
				  },
				  'display_units': 'millions',
				  'display_units_visible': False,
				  'min': 0,
				  'major_gridlines': {
				      'visible': True,
				      'line': {
				        'width': 1,
				        'color': 'black',
				        'width': 0.25
				      }
				  },
				  'line': {'none': True}
				})
			elif(jCrit == 1): # TLB HIT RATE
				chart.set_y_axis({
				  'name'     : criteria[jCrit],
				  'name_font': {
				    'size': 12,
				    'name': 'Times New Roman'
				  },
				  'num_font': {
				    'size': 12,
				    'name': 'Times New Roman'
				  },
				  #'display_units': 'millions',
				  'display_units_visible': False,
				  'min': 0,
				  'max' : 100,
				  'major_gridlines': {
				      'visible': True,
				      'line': {
				        'width': 1,
				        'color': 'black',
				        'width': 0.25
				      }
				  },
				  'line': {'none': True}
				})
			else:
				chart.set_y_axis({
				  'name'     : criteria[jCrit],
				  'name_font': {
				    'size': 12,
				    'name': 'Times New Roman'
				  },
				  'num_font': {
				    'size': 12,
				    'name': 'Times New Roman'
				  },
				  #'display_units': 'millions',
				  'display_units_visible': False,
				  'min': 0,
				  #'max' : 100,
				  'major_gridlines': {
				      'visible': True,
				      'line': {
				        'width': 1,
				        'color': 'black',
				        'width': 0.25
				      }
				  },
				  'line': {'none': True}
				})
				

			chart.set_title({
			  'name'     : appName[chartOrder] + ' ' + imageSize[iSize],
			  'name_font': {
			    'size': 14.4,
			    'name': 'Times New Roman',
			    'bold': True
			  }
			})
			
			chart.set_legend({
			  'position': 'top',
			  'font': {
			    'size': 12,
			    'name': 'Times New Roman'
			  }
			})
			
			chart.set_chartarea({
			  'border': {'none': True},
			 # 'fill':   {'none': True}
			})
			
			worksheet.insert_chart(START_NEW_AREA * jCrit + positionR[chartOrder], positionC[iSize], chart)
		#END for iSize
	#END for chartOrder
#END for jCrit

workbook.close()
