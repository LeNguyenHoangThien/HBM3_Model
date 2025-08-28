#!/usr/bin/python3

import xlsxwriter
import sys

class testcaseClass:
    def __init__(self, buddyType, blockSize, requestSize, alloTime, dealloTime, nFreeNodes, nAlloNodes, avgAddrCovrPTE):
        self.buddyType   = buddyType
        self.blockSize   = blockSize
        self.requestSize = requestSize
        self.alloTime    = alloTime
        self.dealloTime  = dealloTime
        self.nFreeNodes  = nFreeNodes
        self.nAlloNodes  = nAlloNodes
        self.avgAddrCovrPTE = avgAddrCovrPTE
        
    def printTestcase(self):
        print(self.buddyType, self.blockSize, self.requestSize, self.alloTime, self.dealloTime, self.nFreeNodes, self.nAlloNodes, self.avgAddrCovrPTE)
        
testcaseList = []


# load testcases from file to testcaseList
file = open(sys.argv[1], "r")
for line in file:
    fields = line.split()
    
    buddyFields = fields[0].split("_")
    buddyType   = buddyFields[2]
    blockSize   = int(buddyFields[3].lstrip('B'))
    requestSize = int(buddyFields[4].lstrip('R'))
    
    alloTime    = float(fields[1])
    dealloTime  = float(fields[2])
    nFreeNodes  = int(fields[3])
    nAlloNodes  = int(fields[4])
    avgAddrCovrPTE  = int(fields[5])
    
    testcaseObj = testcaseClass(buddyType, blockSize, requestSize, alloTime, dealloTime, nFreeNodes, nAlloNodes, avgAddrCovrPTE)
    testcaseList.append(testcaseObj)
file.close()
# END: load testcases from file to testcaseList
    
testcaseList.sort(key=lambda testcaseClass : (testcaseClass.blockSize, testcaseClass.buddyType, testcaseClass.requestSize))

for e in testcaseList:
	print(e.buddyType, e.blockSize)

workbook = xlsxwriter.Workbook(sys.argv[2])
worksheet = workbook.add_worksheet()

caseList = ["BBS_TRAD", "BBS_NAPOT", "BBS_CAMB", "BBS_RCPT", "BBS_cRCPT"]
numberOfField = 2
numberOfCriteria = 5  #AlloTime, DealloTime, FreeNodes, AlloNodes, AvgAddrCovrPTE
numberOfCase  = len(caseList)
for i in range(numberOfCase):
    worksheet.merge_range(0, numberOfField + i * numberOfCriteria, 0, numberOfField + i * numberOfCriteria + (numberOfCriteria - 1), caseList[i],  workbook.add_format({'align' : 'center'}))

#worksheet.merge_range(0,  2, 0,  5, "TRAD", workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0,  6, 0,  9, "NAPOT", workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, 10, 0, 13, "CAMB", workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, 14, 0, 17, "RCPT", workbook.add_format({'align' : 'center'}))
#worksheet.merge_range(0, 18, 0, 21, "cRCPT", workbook.add_format({'align' : 'center'}))

worksheet.write(1, 0, "BlockSize")
worksheet.write(1, 1, "ReqSize")

#for i in [0, 4, 8, 12, 16]:
for i in range(numberOfField, numberOfField + numberOfCase * numberOfCriteria, numberOfCriteria):
	worksheet.write(1, 0+i, "AlloTime")
	worksheet.write(1, 1+i, "DealloTime")
	worksheet.write(1, 2+i, "FreeNodes")
	worksheet.write(1, 3+i, "AlloNodes")
	worksheet.write(1, 4+i, "AvgAddrCovrPTE")

row = 2
shift = 0
for e in testcaseList:
	if(e.buddyType == "TRAD"):
		shift = 0
		worksheet.write(row, shift + 0, e.blockSize)
		worksheet.write(row, shift + 1, e.requestSize)
	elif(e.buddyType == "NAPOT"):
		shift = 5
	elif(e.buddyType == "CAMB"):
		shift = 9
	elif(e.buddyType == "RCPT"):
		shift = 13
	elif(e.buddyType == "cRCPT"):
		shift = 17
	else:
		print("ERROR!!!!!!!!!!\n");

	worksheet.write(row, shift + 2, e.alloTime)
	worksheet.write(row, shift + 3, e.dealloTime)
	worksheet.write(row, shift + 4, e.nFreeNodes)
	worksheet.write(row, shift + 5, e.nAlloNodes)
	worksheet.write(row, shift + 6, e.avgAddrCovrPTE)
	if(e.buddyType == "cRCPT"):
		row += 1



# Draw chart
criteria  = "Allocation time (us)"
caseName  = ['TRAD', 'NAPOT', 'CAMB', 'RCPT', 'cRCPT']
positionR = [x for x in range(15, 15 + 7 * 16 , 16)]
positionC = [x for x in range( 1,      7 *  8 ,  8)]


stepCol  = 5
stepRow  = 8

startCol = 2
startRow = 2


chart = workbook.add_chart({'type': 'column'})

for i in range(len(caseName)):
  chart.add_series({
    'name'       : caseName[i],
    'categories' : ['Sheet1', 2, 0, 9, 0],
    'values'     : ['Sheet1', startRow, startCol + stepCol * i, startRow + stepRow - 1, startCol + stepCol * i],
    'gap': 75
  })


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

chart.set_y_axis({
  'name'     : criteria,
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

worksheet.insert_chart(positionR[0], positionC[0], chart)


workbook.close()
