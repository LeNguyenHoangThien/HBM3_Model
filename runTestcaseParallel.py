#!/usr/bin/python3

import subprocess
import concurrent.futures
import os
import datetime
import sys

# Generate testcase
subprocess.call(["python3", "gentestcase.py"])

# Define function to run command and wait for it to finish
def run_command(cmd):
	subprocess.run(cmd, shell=True)

# Create executor with max_workers equal to number of CPUs
futures = list()
with concurrent.futures.ProcessPoolExecutor() as executor:
	with open('getTLBCoalescingReport.bash') as f:
		for cmd in f:
			if cmd.startswith("#"):
				continue
			future = executor.submit(run_command, cmd)
			futures.append(future)
	# Wait for all futures to complete
	executor.shutdown()
	concurrent.futures.wait(futures)

###########################################################################
############################################################################

# Create log file
current_date = datetime.datetime.today()
date_str = current_date.strftime("%Y_%m_%d_%H_%S")
logName = "log_" + date_str
logFile =  open(logName, "w")

# Get current directory
cwd = os.getcwd()

# Loop through files in directory to collect report
for filename in os.listdir(cwd):
	# Check if file starts with "check_"
	if filename.startswith("check_"):
		# Open file and print its contents
		with open(filename, 'r') as f:
			nCycle = ""
			TLBHit = ""
			nPTW   = ""
			for line in f:
				lineStrip = line.strip()
				if lineStrip.startswith("[Cycle"):
					lineSlipt = lineStrip.split()
					nCycle    = lineSlipt[1].strip("]")
				if lineStrip.startswith("Total Avg TLB hit rate"):
					lineSlipt = lineStrip.split("=")
					TLBHit    = str(round(float(lineSlipt[1].strip()) * 100, 2))
				if lineStrip.startswith("Total PTW"):
					lineSlipt = lineStrip.split("=")
					nPTW      = lineSlipt[1].strip()
			logFile.write(f'{filename:70}' + f'{nCycle:20}' + f'{TLBHit:10}' + f'{nPTW:20}' + "\n")

logFile.close()

## Generate xlsx
xlsxName = "report_" + date_str + ".xlsx"
subprocess.call(["python3", "./sortTLBReport.py", logName, xlsxName, sys.argv[1]])
