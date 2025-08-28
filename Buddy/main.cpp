//----------------------------------------------------------------------
// Filename     : main.cpp
// Version	: 0.3
// DATE         : 19 Apr 2020
// Contact	: JaeYoung.Hur@gmail.com
// Description  : Standalone Buddy allocator 
//----------------------------------------------------------------------
// Goal
//		(1) Functional and performance model
//		(2) Measure relative allocation and de-allocation time
// Implementation	
//		(1) Traditional Buddy
//		(2) Compaction allocated memory block (CAMB)
//		(3) BCT
// Scope
//		(1) ARM v7. VA 32-bit. PA 32-bit.
//		(2) Single application alloc and de-alloc
//		(3) Mainly, image processing application in embedded (mobile) device (e.g. Image rotator)
//		(4) Page size 4kB
// Input, Output, Given
//		(1) Input	: Initial FreeList 
//		(2) Given	: Start VPN 
//				: GROUP_SIZE (in CAMB)
//		(3) Output	: Page table
// Parameters
//		(1) "NUM_TOTAL_PAGES"		Total physical space size.	1GB
//		(2) "MAX_ORDER"							(2MB, 10)
//		(3) "NUM_REQUESTED_PAGES"	Initial NumRequestPages		(Ex.  )
//		(4) "START_VPN"			Initial StartVPN		(Ex. 0)
//		(5) "PAGE_TABLE_SIZE"		PageTable array size		(Ex. 16384)
// Assumption
//		(1) "START_VPN" aligned with "GROUP_SIZE". Multiple of "GROUP_SIZE"
//		    If "IndexPTE" is aligned, "StartVPN" is aligned.
//		(2) StartVPN and StartPPN are 0
//		(3) "NUM_TOTAL_PAGES", "GROUP_SIZE" are power-of-two.
// Note
// 		(1) <time.h> could provide 10ms resolution accuracy. We could run 1000 (or more) times to get 10us resolution accuracy.
//----------------------------------------------------------------------
// Time	measure	
// 		(1) Allocation    = Time2 - Time1
// 		(2) De-allocation = Time3 - Time2
//----------------------------------------------------------------------
// Page table 
// 	Description
// 		(1) Pointer array of "SPPTE"
// 		(2) Field : PPN, Ascend, Descend
// 	Assumption
// 		(1) Single application
// 		(2) VPN indicate IndexPTE.
//		(3) All PTEs are contiguous. Simple 1D-array
//----------------------------------------------------------------------
// Conventional
//	1. Construct FreeList. Initialize FreeList.
//	2. Measure time1.
//	3. Application invoked.  "Number of requested pages" and "StartVPN" known.
//		(1) Allocate. Check FreeList.  Whenever a block allocated, iterate a,b,c.
//			a. Set page table.
//			b. Update "FreeList".
//			c. Update "BlockList" table.
//	4. Measure time2.
//	5. Application finished. De-allocate. Run buddy.
//		(1) Whenever a block de-allocated, iterate a,b,c.
//			a. Reset page table.
//			b. Update "FreeList".
//			c. Update "BlockList" table.
//		(2) Destruct "BlockList"
//	6. Measure time2
//----------------------------------------------------------------------
// CAMB 
//	1. Construct FreeList. Initialize FreeList.
//	2. Measure time1.
//	3. Application invoked. "Number of requested pages" and "StartVPN" known.
//		(1) Allocate. Check FreeList.  Whenever a block allocated, iterate a,b,c.
//			a. Update "FreeList".
//			b. Update "BlockList" tables.
//			c. Update "PageArray" tables.
//		(2) When all blocks are allocated, set page table.
//		    Convert "BlockList" into "BlockArray"
//	4. Measure time2.
//	5. Application finished. De-allocate. Run buddy.
//		(1) Whenever a block de-allocated, iterate a,b,c,d.
//			a. Reset page table.
//			b. Update "FreeList".
//			c. Update "BlockList" table.
//			d. Update "PageArray" table.
//		(2) Destruct "BlockList", "BlockArray" and "PageArray"
//	6. Measure time3
//----------------------------------------------------------------------
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <iostream>

#include "CBuddy.h"

#include "../Top.h"

//---------------------------
// clock_gettime() in <time.h>
//---------------------------
// struct timespec {                                                         
//	time_t	tv_sec;		/* seconds */                                 
//	long	tv_nsec;	/* nanoseconds */                              
// }; 


int main() {
//	int i = 0;
//	while(i++ < 10000){

	// Measure time
	struct timespec TimeStart_alloc;
	struct timespec TimeStart_dealloc;
	struct timespec TimeEnd_alloc;
	struct timespec TimeEnd_dealloc;

	//----------------------------------------------------------
	// 1. Construct Buddy. Initialize.
	//----------------------------------------------------------
	CPBuddy cpBuddy = new CBuddy("Buddy");

	//----------------------------------------------------------
	// 2. Config initial FreeList.
	//----------------------------------------------------------

	//cpBuddy->Set_FreeList();   // DUONGTRAN comment

	// DUONGTRAN add: Allign initialize FreeList
    //                      #block size     #number of blocks
 	// cpBuddy->Set_FreeList(BLOCKSIZE_INIT, NUM_TOTAL_PAGES / BLOCKSIZE_INIT); 

	////DUONGTRAN add: Missalign initialize Memmap
	//cpBuddy->InitializeMemMap(BLOCKSIZE_INIT, NUM_TOTAL_PAGES);
	//DUONGTRAN add: Random initialize Memmap
	cpBuddy->InitializeMemMapRandom(BLOCKSIZE_INIT, NUM_TOTAL_PAGES);
	#ifdef RBS
		cpBuddy->Set_FreeList('R');
	#elif defined PBS
		cpBuddy->Set_FreeList('P');
	#elif defined BBS
		cpBuddy->Set_FreeList('B');
	#else
		assert(0);
	#endif

	printf("FreeNodes: %d\n", cpBuddy->GetNumNodes());

	//----------------------------------------------------------
	// 3. Allocate 
	//----------------------------------------------------------
	// Measure time. Allocation start.
	clock_gettime(CLOCK_MONOTONIC, &TimeStart_alloc);
	
	cpBuddy->Do_Allocate(0x0, NUM_REQUESTED_PAGES);			// START_VPN 0	FIXME, using a page table for all MST, MMU
	//cpBuddy->Do_Allocate(0x20000, NUM_REQUESTED_PAGES);			// START_VPN 0	FIXME, using a page table for all MST, MMU
	//cpBuddy->Do_Allocate(0x70000, NUM_REQUESTED_PAGES);			// START_VPN 0

	//cpBuddy->Do_Allocate(0x20000, 1);			// START_VPN 0
	//cpBuddy->Do_Allocate(0x70000, 1);			// START_VPN 0

	// Measure time. Allocation finished.
	clock_gettime(CLOCK_MONOTONIC, &TimeEnd_alloc);

	//---------------------------------------------
	// Measure the average address coverage in a PTE
	//--------------------------------------------
	printf("AvgAddrCovrPTE: %g\n", cpBuddy->GetAvgAddrCovrPTE());


	//----------------------------------------------------------
	// 4. De-allocate
	//----------------------------------------------------------
	// Measure time. De-allocation start.
	clock_gettime(CLOCK_MONOTONIC, &TimeStart_dealloc);

	cpBuddy->Do_Deallocate(0x0);			// START_VPN 0x20000
	//cpBuddy->Do_Deallocate(0x20000);			// START_VPN 0x20000
	//cpBuddy->Do_Deallocate(0x70000);			// START_VPN 0x70000

	// Measure time. De-allocation finished.
	clock_gettime(CLOCK_MONOTONIC, &TimeEnd_dealloc);

	//----------------------------------------------------------
	// 5. Destruct 
	//----------------------------------------------------------
	delete (cpBuddy);

	//----------------------------------------------------------
	// Get elapsed time
	//----------------------------------------------------------
	long Time_elapsed_cpu_time_alloc_sec   = TimeEnd_alloc.tv_sec    - TimeStart_alloc.tv_sec;
	long Time_elapsed_cpu_time_dealloc_sec = TimeEnd_dealloc.tv_sec  - TimeStart_dealloc.tv_sec;

	long Time_elapsed_cpu_time_alloc_ns    = TimeEnd_alloc.tv_nsec   - TimeStart_alloc.tv_nsec;
	long Time_elapsed_cpu_time_dealloc_ns  = TimeEnd_dealloc.tv_nsec - TimeStart_dealloc.tv_nsec;

	if (TimeStart_alloc.tv_nsec > TimeEnd_alloc.tv_nsec) { // clock underflow
		--Time_elapsed_cpu_time_alloc_sec;
		Time_elapsed_cpu_time_alloc_ns += 1000000000;
	};

	if (TimeStart_dealloc.tv_nsec > TimeEnd_dealloc.tv_nsec) { // clock underflow
		--Time_elapsed_cpu_time_dealloc_sec;
		Time_elapsed_cpu_time_dealloc_ns += 1000000000;
	};

	printf("---------------------------------------------\n");
	printf("Allocations       elapsed time (us) = %g\n", (double)Time_elapsed_cpu_time_alloc_sec/(double)1000000   + (double)Time_elapsed_cpu_time_alloc_ns/(double)1000);
	// printf("Allocations    elapsed time (ns) = %ld\n", Time_elapsed_cpu_time_alloc_ns);
	printf("De-allocations    elapsed time (us) = %g\n", (double)Time_elapsed_cpu_time_dealloc_sec/(double)1000000 + (double)Time_elapsed_cpu_time_dealloc_ns/(double)1000);
	// printf("De-allocations elapsed time (ns) = %ld\n", Time_elapsed_cpu_time_dealloc_ns);
	printf("---------------------------------------------\n");
	//}
	return (-1);
}

