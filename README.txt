-----------------------------------------------------------
Memory system simulator 
-----------------------------------------------------------
Version		: 0.75
Contact		: JaeYoung.Hur@gmail.com
Date		: 16 Nov 2019 
-----------------------------------------------------------
History
Feb 01 2023: Duong port to ARMv8 Address translation with 4-level
-----------------------------------------------------------
Descriptions 
	1. Simulation of memory subsystem in SoC
	2. Performance model to explore micro-architectures
	3. Cycle level simulation
	4. Transaction-level (ARM architecture, AXI protocol)
-----------------------------------------------------------
Notes 
	- The simulation is tested with g++ (gcc version 9.4.0) in Linux (5.15.90.1-microsoft-standard-WSL2) 
	- We did not experience warnings or errors. For any support, please contact JaeYoung.Hur@gmail.com
-----------------------------------------------------------
Scenarios
	- Image
		+ Display
		+ Blending
		+ Preview
		+ Scaling
	- Matrix
		+ Transpose
		+ Multiplition
		+ Convolution
		+ Sobel
-----------------------------------------------------------
How to run
	To run the simulation, type below command:
	step 1:	./runTestcaseParallel.py RBS_PCAD
	step 2: check log_<today time> and report_<today time>.xlsx
	
-----------------------------------------------------------
To do
	1. Use shared page table
		(1) Currently, fixed page table assumed 
	2. Use a single "main.cpp" 
		(1) Enable (or disable) each instance 
	3. Reduce "#ifdef"
		(1) Use "if" and "else"
	4. Reduce perf stat 
		(1) Every cycle, occupancy and waiting time. --> This takes time 
		    Perf. stat --> #define
	5. PCA BF
		(1) Contiguity 100% --> FillTLB_BF() no code?



