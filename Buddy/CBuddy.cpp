///-----------------------------------------------------------
// Filename     : CBuddy.cpp 
// Version	: 0.3
// Date         : 19 Apr 2020
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Buddy handler definition
//-----------------------------------------------------------
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
//		(3) Page size 4kB
// Input, Output, Given
//		(1) Input       : Initial FreeList 
//		(2) Given       : Start VPN 
//				: GROUP_SIZE (in CAMB)
//		(3) Output      : Page table
// Parameters
//		(1) "NUM_TOTAL_PAGES"		Total physical space size.	1GB
//		(2) "MAX_ORDER"							(2MB, 10)
//		(3) "NUM_REQUESTED_PAGES"	Initial NumRequestPages		(Ex.  )
//		(4) "START_VPN"			Initial StartVPN		(Ex. 0)
//		(5) "PAGE_TABLE_SIZE"		PageTable array size		(Ex. 16384)
// Assumption
// 		(1) VPN is "IndexPTE"
//		(2) If "IndexPTE" is aligned, "StartVPN" is aligned.
//		(3) StartVPN and StartPPN are 0
//		(4) "NUM_TOTAL_PAGES", "GROUP_SIZE" are power-of-two.
//
//		(5) [CAMB] "START_VPN" aligned with "GROUP_SIZE". Multiple of "GROUP_SIZE"
//----------------------------------------------------------------------
// Page table 
//	Description
//		(1) Pointer array of "SPPTE"
//		(2) Field : PPN, Ascend, Descend
//		(3) Size  : "NUM_REQUESTED_PAGES"
//	Assumption
//		(1) VPN is IndexPTE.
//		(2) All PTEs are contiguous. Simple 1D-array
//----------------------------------------------------------------------
// Conventional
//	Allocation	
//		"Number of requested pages" and "StartVPN" known.
//		(1) Allocate. Check FreeList.  Whenever a block allocated, iterate a,b,c.
//			a. Set page table.
//			b. Update "FreeList".
//			c. Update "BlockList" table.
//	Deallocation
//		(1) Whenever a block de-allocated, iterate a,b,c.
//			a. Reset page table.
//			b. Update "FreeList".
//			c. Update "BlockList" table.
//		(2) Destruct "BlockList"
//----------------------------------------------------------------------
// CAMB 
//	Allocation	
//		"Number of requested pages" and "StartVPN" known.
//		(1) Allocate. Check FreeList.  Whenever a block allocated, iterate a,b,c.
//			a. Update "FreeList".
//			b. Update "BlockList" tables.
//			c. Update "PageArray" tables.
//		(2) When all blocks are allocated, set page table.
//		    Convert "BlockList" into "BlockArray"
//	Deallocation	
//		(1) Whenever a block de-allocated, iterate a,b,c,d.
//			a. Reset page table.
//			b. Update "FreeList".
//			c. Update "BlockList" table.
//			d. Update "PageArray" table.
//		(2) Destruct "BlockList", "BlockArray" and "PageArray"
//----------------------------------------------------------------------
// To-do
// 	CAMB
// 		(1) Check CAMB support multiple applications.
// 		(2) Check CAMB support BCT (BlockSize) page table 
// 	BlockList
// 		(1) When multiple applications run, "BlockList" should not be FIFO. 
// 		    "BlockList" should be out-of-order queue or CAM.
//----------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "CBuddy.h"
 
//-------------------------------------------------
// Constructor
// 	"MAX_ORDER", "NUM_TOTAL_PAGES" known
//-------------------------------------------------
CBuddy::CBuddy(string cName){

	// Construct. "MAX_ORDER", "NUM_TOTAL_PAGES" static known.
	this->cpFreeList = new CFreeList("FreeList");
	
	// Initially empty.
	this->cpBlockList = new CBlockList("BlockList");

	// Initiliaze memamp
	this->memmap = new bool[NUM_TOTAL_PAGES * 2];

	// #ifdef CAMB
	// // Construct PageArray. Array size known. 
	// int nNumElement_PageArray = 1 + NUM_REQUESTED_PAGES / GROUP_SIZE;
	// this->cpPageArray = new CPageArray("PageArray", nNumElement_PageArray);
	// #endif
	
	// // Generate page-table
	// this->spPageTable = new SPTE* [NUM_REQUESTED_PAGES];
	// for (int i=0; i<NUM_REQUESTED_PAGES; i++) {
	// 	this->spPageTable[i] = new SPTE;
	// };

	// Initialize	
	this->cName = cName;
	
	this->anchorDistance = 0;    //DUONGTRAN add
};


//-------------------------------------------------
// Destructor
//-------------------------------------------------
CBuddy::~CBuddy() {

	delete (this->cpFreeList); 
	this->cpFreeList = NULL;

	//DUONGTRAN add
	delete (this->cpBlockList); 
	this->cpBlockList = NULL;

	delete(this->memmap);
	this->memmap = NULL;

	// for (int i=0; i < NUM_REQUESTED_PAGES; i++) {
	// 	delete (this->spPageTable[i]); 
	// 	this->spPageTable[i] = NULL;
	// };
	
#ifdef CAMB_ENABLE
 	// delete (cpBlockArray);  // Delete only pointer FIXME
	delete (this->cpPageArray);
	// this->cpPageArray = NULL;
#endif
};

// DUONGTRAN add
//-------------------------------------------------
// initialize random memory
//-------------------------------------------------
EResultType CBuddy::InitializeMemMapRandom(int BlockSize, int TOTAL_PAGES) {
	for(int i = 0; i < TOTAL_PAGES * 2; i++){
		this->memmap[i] = false;
	}
	if(BlockSize == 1){
		for(int i = 0; i < TOTAL_PAGES * 2; i = i + 2){
			this->memmap[i] = true;
		}
		return (ERESULT_TYPE_SUCCESS);
	}
	int step = BlockSize << 1;
	for(int i = 0; i < TOTAL_PAGES * 2; i += step){
		int rand = random(0, BlockSize - 1);
		for(int j = 0; j < BlockSize; j++){
			this->memmap[i + j + rand] = true;
		}
	}
	return (ERESULT_TYPE_SUCCESS);
}

// DUONGTRAN add
//-------------------------------------------------
// initialize aligned memory
//-------------------------------------------------
EResultType CBuddy::InitializeMemMapAligned(int BlockSize, int TOTAL_PAGES) {
	for(int i = 0; i < TOTAL_PAGES * 2; i++){
		this->memmap[i] = false;
	}
	if(BlockSize == 1){
		for(int i = 0; i < TOTAL_PAGES * 2; i = i + 2){
			this->memmap[i] = true;
		}
		return (ERESULT_TYPE_SUCCESS);
	}
	int step = BlockSize << 1;
	for(int i = 0; i < TOTAL_PAGES * 2; i += step){
		for(int j = 0; j < BlockSize; j++){
			this->memmap[i + j] = true;
		}
	}
	return (ERESULT_TYPE_SUCCESS);
}

// DUONGTRAN add
//-------------------------------------------------
// initialize miss aligned memory
//-------------------------------------------------
EResultType CBuddy::InitializeMemMap(int BlockSize, int TOTAL_PAGES) {
	for(int i = 0; i < TOTAL_PAGES * 2; i++){
		this->memmap[i] = false;
	}
	if(BlockSize == 1){
		for(int i = 0; i < TOTAL_PAGES * 2; i = i + 2){
			this->memmap[i] = true;
		}
		return (ERESULT_TYPE_SUCCESS);
	}
	int i = 0;
	int j = 0;
	if(BlockSize == 16){
		for(i = 0; i < TOTAL_PAGES * 2; i = i + 32){
			for(j = 0; j < 16; j++){
				this->memmap[i + j] = true;
			}
		}
		return (ERESULT_TYPE_SUCCESS);
	}
	if(BlockSize == 256){
		for(int i = 0; i < TOTAL_PAGES * 2; i = i + 512){
			for(int j = 0; j < 256; j++){
				this->memmap[i + j] = true;
			}
		}
		return (ERESULT_TYPE_SUCCESS);
	}
	

	//int total_pages = 0;

	//int i = 0;
	//int trueRange = BlockSize;
	//while(i < TOTAL_PAGES * 2){
	//	i++;
	//	if(trueRange > 0){
	//		this->memmap[i] = true;
	//		trueRange--;
	//	} else {
	//		trueRange = BlockSize;
	//		total_pages += BlockSize;
	//		if(total_pages >= TOTAL_PAGES)
	//			return (ERESULT_TYPE_SUCCESS);
	//		if(((i + 1) % BlockSize == 0)){
	//			i++;
	//		}
	//	}
	//}

	if(BlockSize < 16){
		int i = 1;
		int j = 0;
		int totalPage = 0;
		while(totalPage < TOTAL_PAGES){
			j = 0;
			while(j < BlockSize){
				this->memmap[i + j] = true;
				j++;
			}
			totalPage += BlockSize;
			if(totalPage >= TOTAL_PAGES)
				return (ERESULT_TYPE_SUCCESS);
			i = i + j + 1;
			if(i % BlockSize == 0) i++;
		}
		return (ERESULT_TYPE_SUCCESS);
	}
	if(BlockSize < 256){
		int i = 16;
		int j = 0;
		int totalPage = 0;
		while(totalPage < TOTAL_PAGES){
			j = 0;
			while(j < BlockSize){
				this->memmap[i + j] = true;
				j++;
			}
			totalPage += BlockSize;
			if(totalPage >= TOTAL_PAGES)
				return (ERESULT_TYPE_SUCCESS);
			i = i + j + 16;
			if(i % BlockSize == 0) i += 16;
		}
		return (ERESULT_TYPE_SUCCESS);
	}
	if(BlockSize < 4096){
		int i = 256;
		int j = 0;
		int totalPage = 0;
		while(totalPage < TOTAL_PAGES){
			j = 0;
			while(j < BlockSize){
				this->memmap[i + j] = true;
				j++;
			}
			totalPage += BlockSize;
			if(totalPage >= TOTAL_PAGES)
				return (ERESULT_TYPE_SUCCESS);
			i = i + j + 256;
			if(i % BlockSize == 0) i += 256;
		}
		return (ERESULT_TYPE_SUCCESS);
	}
	assert(0);
};

//-------------------------------------------------
// Set freelist initialize
//-------------------------------------------------
EResultType CBuddy::Set_FreeList() {

	this->cpFreeList->SetFree_Initial(); 

	return (ERESULT_TYPE_SUCCESS);
};

// DUONGTRAN add
EResultType CBuddy::Set_FreeList(int reqBlockSize, int numBlock) {

	this->cpFreeList->SetFree_Initial(reqBlockSize, numBlock); 

	return (ERESULT_TYPE_SUCCESS);
};

// DUONGTRAN add
EResultType CBuddy::Set_FreeList(char typeBuddy){
	this->cpFreeList->SetFree_Initial(memmap, typeBuddy);
	return (ERESULT_TYPE_SUCCESS);
}

//-------------------------------------------------
// Allocate 
//-------------------------------------------------
EResultType CBuddy::Do_Allocate(int64_t ORIGINAL_VPN, int NUM_REQ_PAGES) {

	#ifdef DEBUG_BUDDY
	// Check allocatable
	//this->nInitial_NumFreePages = this->cpFreeList->GetNumFreePages();    //DUONGTRAN comment
	if (this->cpFreeList->nNumFreePage < NUM_REQ_PAGES) {
		printf("\nApplication requests %d pages. Memory has %d pages. \n", NUM_REQ_PAGES, this->cpFreeList->nNumFreePage);
		assert (0);
	};
	#endif
	
	#ifdef DEBUG_BUDDY
	printf("---------------------------------------------\n");
	printf("Initial memory setting \n");
	printf("---------------------------------------------\n");
	printf("\t MAX_ORDER              : %d\n", MAX_ORDER);
	printf("\t Memory space (in pages): %d\n", NUM_TOTAL_PAGES);
	printf("\t Memory available pages : %d\n", this->cpFreeList->nNumFreePage);
	printf("---------------------------------------------\n");
	#endif
	
	#ifdef DEBUG_BUDDY
//	this->cpFreeList->Display();
	#endif

	//----------------------------------------------------------
	// Application invoked.  "Number of requested pages" and "StartVPN" known
	//----------------------------------------------------------
	int nNumRequestedPages = NUM_REQ_PAGES;
	int64_t nStartVPN = ORIGINAL_VPN;
	
	#ifdef DEBUG_BUDDY
	printf("---------------------------------------------\n");
	printf("Allocation requested. \n");
	printf("---------------------------------------------\n");
	printf("\t Start VPN                 : 0x%lx\n", nStartVPN);
	printf("\t Requested number of pages :   %d\n",  nNumRequestedPages);
	printf("---------------------------------------------\n");
	#endif

	// Insert Dummy PTEs for CAMB, RCPT, and cRCPT
	#if defined CAMB_ENABLE || defined RCPT_ENABLE || defined cRCPT_ENABLE
		int NUM_REQ_PAGES_WITHOUT_DUMMY = NUM_REQ_PAGES & ~(GROUP_SIZE - 1);
		if(NUM_REQ_PAGES != NUM_REQ_PAGES_WITHOUT_DUMMY){
			NUM_REQ_PAGES = NUM_REQ_PAGES_WITHOUT_DUMMY + GROUP_SIZE;
		}
	#endif

	// Block number 0,1,2,3,...
	int nBlockNum = 0;
	// Generate page-table
	this->spPageTable = new SPTE* [NUM_REQ_PAGES];
	for (int i = 0; i < NUM_REQ_PAGES; i++) {
		this->spPageTable[i] = new SPTE;
	};
	// PTE index 0,1,2,3,...
	int nIndexPTE = 0;					// nStartVPN

	#ifdef AT_ENABLE
		int anchor_record_block[20] = {0};   // DUONGTRAN add
		int anchor_record_time[20] = {0};
		int anchor_distance[20] = {0};
		int jRecord = 0; // index of anchor_record_block[]
	#endif

	//----------------------------------------------------------
	// Allocate application
	//----------------------------------------------------------
	while(nNumRequestedPages > 0) {
	
		//----------------------------------------------------------
		// Allocate a block 
		//----------------------------------------------------------

		// Get targeted order 
		int iOrder = this->cpFreeList->GetTargetOrder(nNumRequestedPages);
		
		int nBlockSize = 0;	
		#ifdef PBS
			int multiple = 1 << iOrder;
		#endif
		//// Expand until FreeList[i] filled
		if (this->cpFreeList->IsEmpty(iOrder) == ERESULT_TYPE_YES) {
			#if defined RBS
				this->cpFreeList->Expand(iOrder+1, nNumRequestedPages);
			#elif defined PBS
				this->cpFreeList->Expand(iOrder + GRANULARITY, 1 << (iOrder + GRANULARITY));
			#elif defined BBS
				this->cpFreeList->Expand(iOrder+1);
			#endif //RBS
		};

		
		#ifdef DEBUG_BUDDY
		assert (this->cpFreeList->IsEmpty(iOrder) == ERESULT_TYPE_NO);
		#endif
		
		int nStartPPN  = this->cpFreeList->GetStartPPN_Head(iOrder);
		#ifdef RBS
			nBlockSize = this->cpFreeList->GetBlockSize_Head(iOrder);
			if(nBlockSize > nNumRequestedPages) {
				this->cpFreeList->SplitRange(iOrder, nStartPPN, nNumRequestedPages);
				nBlockSize = nNumRequestedPages;
				nNumRequestedPages = 0;
			} else {   // nBlockSize <= nNumRequestedPages
				nNumRequestedPages -= nBlockSize;
			}
			// Store block-info in "BlockList" table
			this->cpBlockList->Push(ORIGINAL_VPN, nStartVPN, nStartPPN, nBlockSize);
		#elif defined PBS
			nBlockSize = this->cpFreeList->GetBlockSize_Head(iOrder);
			if(nBlockSize > nNumRequestedPages) {
				//nBlockSize = nNumRequestedPages - nNumRequestedPages % multiple;
				nBlockSize = nNumRequestedPages & (~(multiple - 1));
				this->cpFreeList->SplitPage(iOrder, 0, nBlockSize); // 0 mean split
			}
			nNumRequestedPages -= nBlockSize;
			// Store block-info in "BlockList" table
			this->cpBlockList->Push(ORIGINAL_VPN, nStartVPN, nStartPPN, nBlockSize);
		#elif defined BBS
			nBlockSize = GetBlockSize(iOrder);
			nNumRequestedPages -= nBlockSize;
			// Store block-info in "BlockList" table
			this->cpBlockList->Push(ORIGINAL_VPN, nStartVPN, nStartPPN, nBlockSize);
		#else
			assert(0);
		#endif // RBS

		#ifdef DEBUG_BUDDY
		assert(nBlockSize > 0);
//		printf("[Block %2d] Allocated. StartVPN 0x%lx. StartPPN 0x%x. BlockSize %d.\n", nBlockNum, nStartVPN, nStartPPN, nBlockSize);
		#endif

		// Update block-number 
		nBlockNum++;

		#ifdef AT_ENABLE //DUONGTRAN add
			for(int i = 0; i <= jRecord; i++){
				if(anchor_record_block[i] == nBlockSize){
					anchor_record_time[i]++;
					break;
				} else {
					if(i == jRecord){
						anchor_record_block[jRecord] = nBlockSize;
						anchor_record_time[jRecord]  = 1;
						jRecord++;
						break;
					}
				}
			}
			if(nNumRequestedPages == 0){
				if(nBlockNum == 1) {
					this->anchorDistance = nBlockSize;
				} else {
					for(int i = 0; i < 16; i++){
						for(int j = 0; j < jRecord; j++){
							anchor_distance[i] += (anchor_record_block[j]/((2<<i)*(2<<i)) * anchor_record_time[j]);
						}
						if(anchor_distance[i] == 0){
							this->anchorDistance = 2<<i;
							break;
						}
					}
				}
			}

			#ifdef DEBUG_BUDDY
			assert(jRecord < 20);
			#endif

		#endif //AT



		// Update StartVPN
		nStartVPN += nBlockSize;

		#if !defined CAMB_ENABLE && !defined RCPT_ENABLE && !defined cRCPT_ENABLE
			// Update page table
			#ifdef NAPOT
				int NAPOT_SIZE = 1 << NAPOT_BITS;
				if(nBlockSize >= NAPOT_SIZE){
					int nStartPPN_NAPOT = nStartPPN;
					for(int j = 0; j < nBlockSize; j += NAPOT_SIZE){	// NAPOT PTE
						for(int k = 0; k < NAPOT_SIZE; k++){	// detail
							spPageTable[nIndexPTE]->N_flag = 1;
							spPageTable[nIndexPTE]->nPPN   = nStartPPN_NAPOT;
							nIndexPTE++;
						} // end for: detail
						nStartPPN_NAPOT += NAPOT_SIZE;
					} // end for: NAPOT PTE
				} else {
					for(int j = 0; j < nBlockSize; j++) {
						spPageTable[nIndexPTE]->N_flag     = 0;
						spPageTable[nIndexPTE]->nPPN       = nStartPPN + j;
						nIndexPTE++;
					} //endfor
				} // endif
			#else
				for(int j = 0; j < nBlockSize; j++) {
					// Set PTE
					spPageTable[nIndexPTE]->nPPN       = nStartPPN + j;					// FIXME Careful
					spPageTable[nIndexPTE]->nBlockSize = nBlockSize;					// FIXME Careful

					#ifdef PCAD
						#ifdef PBS
							//spPageTable[nIndexPTE]->nAscend = (nBlockSize - j - 1) / (1 << iOrder);
							spPageTable[nIndexPTE]->nAscend = (nBlockSize - j - 1) >> iOrder;
							//spPageTable[nIndexPTE]->nDescend = j / (1 << iOrder);
							spPageTable[nIndexPTE]->nDescend = j >> iOrder;
							spPageTable[nIndexPTE]->nCodedPageSize = iOrder >> 2;			//DUONGTRAN add for PBS
						#else
							spPageTable[nIndexPTE]->nAscend = nBlockSize - j - 1;
							spPageTable[nIndexPTE]->nDescend = j;
						#endif //PBS
					#endif	// PCAD

					#ifdef AT_ENABLE
						spPageTable[nIndexPTE]->nContiguity = nBlockSize - j;
					#endif

					// Update index
					nIndexPTE++;
				}
			#endif // NAPOT
		#endif // !defined CAMB_ENABLE && !defined RCPT_ENABLE && !defined cRCPT_ENABLE

		
		// Update "FreeList"
		this->cpFreeList->Remove(iOrder, nStartPPN);
		this->cpFreeList->nNumFreePage -= nBlockSize;
		#ifdef DEBUG_BUDDY	
//			this->cpBlockList->Display();
		#endif
		
		
		// Check all allocation finished 
		if(nNumRequestedPages == 0) {

			#ifdef AT_ENABLE
				for(int j = 0; j < nIndexPTE; j++){
					if(j % this->anchorDistance == 0){
						if(spPageTable[j]->nContiguity > this->anchorDistance){
							spPageTable[j]->nContiguity = this->anchorDistance;
						}
						j += (this->anchorDistance - 1);
					}
				}
			#endif
		
			#ifdef DEBUG_BUDDY
				printf("---------------------------------------------\n");
				printf("All allocations (%d block(s)) finished.      \n", nBlockNum);
				printf("---------------------------------------------\n");
			#endif
			
			#ifdef DEBUG_BUDDY
				#ifdef BBS
					EResultType IsExist = cpFreeList->Is_Buddy_Exist_Not_Merged();
					if (IsExist == ERESULT_TYPE_YES) {
						printf("In FreeList, buddy remains un-merged.\n");
						assert (0);
					};
				#endif
//				this->cpFreeList->Display();
//				this->cpBlockList->Display();
			#endif
			
			printf("nCurNum\t:\t%d\n", this->cpBlockList->GetCurNum());   // DUONGTRAN don't comment to get number of allocated nodes
			break;
		};
	};

	#ifdef CAMB_ENABLE

		#ifdef DEBUG_BUDDY
			assert(nIndexPTE == 0);
		#endif

		// Build CAMB page table
		SPLinkedBlock spLinkedBlockCurrent = this->cpBlockList->spLinkedBlock_head;
		SPLinkedBlock spLinkedBlockAlignNext = this->cpBlockList->spLinkedBlock_head;
		int64_t nStartAlignedVPN = spLinkedBlockCurrent->spBlock->nStartVPN & ~(GROUP_SIZE - 1);	// GROUP_SIZE must is power-of-2

		while(nIndexPTE < NUM_REQ_PAGES){	// build group
			if(spLinkedBlockCurrent != NULL){
				spPageTable[nIndexPTE]->nPPN       = spLinkedBlockCurrent->spBlock->nStartPPN;
				spPageTable[nIndexPTE]->nBlockSize = spLinkedBlockCurrent->spBlock->nBlockSize;

				if(spLinkedBlockCurrent->spBlock->nStartVPN == nStartAlignedVPN + GROUP_SIZE){
					spLinkedBlockAlignNext = spLinkedBlockCurrent;
				}

				spLinkedBlockCurrent = spLinkedBlockCurrent->spNext;
			}
			// Update index
			nIndexPTE++;

			//if(nIndexPTE % GROUP_SIZE == 0){
			if((nIndexPTE & (GROUP_SIZE - 1)) == 0){
				nStartAlignedVPN += GROUP_SIZE;
				if( (spLinkedBlockCurrent != NULL) && (spLinkedBlockCurrent->spBlock->nStartVPN == nStartAlignedVPN) ){		// in case full fragmentation
					continue;
				}
				spLinkedBlockCurrent = spLinkedBlockAlignNext;
			}
		}

	#endif  //CAMB_ENABLE

	#ifdef RCPT_ENABLE
		#ifdef DEBUG_BUDDY
			assert(nIndexPTE == 0);
		#endif

		// Build RCPT page table
		SPLinkedBlock spLinkedBlockCurrent = this->cpBlockList->spLinkedBlock_head;
		SPLinkedBlock spLinkedBlockAlignNext = this->cpBlockList->spLinkedBlock_head;
		int64_t nStartAlignedVPN = spLinkedBlockCurrent->spBlock->nStartVPN & ~(GROUP_SIZE - 1);	// GROUP_SIZE must is power-of-2

		/*
			| PPN0               | BlockSize0               |                Atttributes                    |
			| PPN2               | BlockSize2               | PPN1               | BlockSize1               |
			| PPN4               | BlockSize4               | PPN3               | BlockSize3               |
			....
			| PPN_NUM_PTE        | BlockSize_NUM_PTE        |                Atttributes                    |
			| PPN_NUM_PTE_PLUS_2 | BlockSize_NUM_PTE_PLUS_2 | PPN_NUM_PTE_PLUS_1 | BlockSize_NUM_PTE_PLUS_1 |
			| PPN_NUM_PTE_PLUS_4 | BlockSize_NUM_PTE_PLUS_4 | PPN_NUM_PTE_PLUS_3 | BlockSize_NUM_PTE_PLUS_3 |
			...
			
			| PPN_LAST_IF_EVEN   | BlockSize_IF_EVEN        | PPN_LAST_IF_ODD    | BlockSize_IF_ODD         |
		*/

		bool evenFlag = true;
		while(nIndexPTE < NUM_REQ_PAGES){	// build group
			if(spLinkedBlockCurrent != NULL){
				if(evenFlag == true){
					spPageTable[nIndexPTE]->nPPN         = spLinkedBlockCurrent->spBlock->nStartPPN;
					spPageTable[nIndexPTE]->nBlockSize   = spLinkedBlockCurrent->spBlock->nBlockSize;
					evenFlag = false;
					// Update index
					nIndexPTE++;
				} else {
					spPageTable[nIndexPTE]->nPPN_1       = spLinkedBlockCurrent->spBlock->nStartPPN;
					spPageTable[nIndexPTE]->nBlockSize_1 = spLinkedBlockCurrent->spBlock->nBlockSize;
					evenFlag = true;
				}

				if(spLinkedBlockCurrent->spBlock->nStartVPN == nStartAlignedVPN + GROUP_SIZE){
					spLinkedBlockAlignNext = spLinkedBlockCurrent;
				}

				spLinkedBlockCurrent = spLinkedBlockCurrent->spNext;
			} else {
			//if(spLinkedBlockCurrent == NULL){
				// Update index
				nIndexPTE++;
			}

			//if(nIndexPTE % GROUP_SIZE == 0){
			if((nIndexPTE & (GROUP_SIZE - 1)) == 0){
				nStartAlignedVPN += GROUP_SIZE;
				if( (spLinkedBlockCurrent != NULL) && (spLinkedBlockCurrent->spBlock->nStartVPN == nStartAlignedVPN) ){		// in case full fragmentation
					continue;
				}
				spLinkedBlockCurrent = spLinkedBlockAlignNext;
				evenFlag = true;
			}
		}

	#endif //RCPT_ENABLE

	#ifdef cRCPT_ENABLE
		#ifdef DEBUG_BUDDY
			assert(nIndexPTE == 0);
				#ifdef DEBUG_BUDDY	
				//this->cpBlockList->Display();
				#endif
		#endif

		// Build cRCPT page table

		SPLinkedBlock spLinkedBlockCurrent = this->cpBlockList->spLinkedBlock_head;
		SPLinkedBlock spLinkedBlockAlignNext = this->cpBlockList->spLinkedBlock_head;

		int64_t nStartAlignedVPN = spLinkedBlockCurrent->spBlock->nStartVPN & ~(GROUP_SIZE - 1);	// GROUP_SIZE must is power-of-2

		//while((nIndexPTE < NUM_REQ_PAGES) and (spLinkedBlockCurrent != NULL)){	// build group
		int32_t msb_aligned_PPN;
		int32_t msb_block1_PPN;
		int32_t msb_block2_PPN;
		//int32_t msb_block3_PPN;
		int32_t nStartPPN;
		while(nIndexPTE < NUM_REQ_PAGES){	// build group

				//if(nIndexPTE % GROUP_SIZE == 0){	// AlignedPTEs
				if((nIndexPTE & (GROUP_SIZE - 1)) == 0){	// AlignedPTEs
					if(spLinkedBlockCurrent != NULL){
						nStartPPN                        	 = spLinkedBlockCurrent->spBlock->nStartPPN;
						spPageTable[nIndexPTE]->nBlockSize   = spLinkedBlockCurrent->spBlock->nBlockSize;
						spPageTable[nIndexPTE]->nPPN         = nStartPPN;
						msb_aligned_PPN 					 = nStartPPN >> 16;
						spLinkedBlockCurrent 				 = spLinkedBlockCurrent->spNext;
					}
				} else { // Next PTEs

					if(spLinkedBlockCurrent != NULL){
						nStartPPN 		                     = spLinkedBlockCurrent->spBlock->nStartPPN;
						spPageTable[nIndexPTE]->nBlockSize_1 = spLinkedBlockCurrent->spBlock->nBlockSize;
						spPageTable[nIndexPTE]->nPPN_1       = nStartPPN;
						msb_block1_PPN 						 = nStartPPN >> 16;
						if(spLinkedBlockCurrent->spBlock->nStartVPN == nStartAlignedVPN + GROUP_SIZE){
							spLinkedBlockAlignNext = spLinkedBlockCurrent;
						}
						spLinkedBlockCurrent = spLinkedBlockCurrent->spNext;
					}
					if(spLinkedBlockCurrent != NULL){
						nStartPPN                        	 = spLinkedBlockCurrent->spBlock->nStartPPN;
						spPageTable[nIndexPTE]->nBlockSize   = spLinkedBlockCurrent->spBlock->nBlockSize;
						spPageTable[nIndexPTE]->nPPN         = nStartPPN;
						msb_block2_PPN 						 = nStartPPN >> 16;
						if(spLinkedBlockCurrent->spBlock->nStartVPN == nStartAlignedVPN + GROUP_SIZE){
							spLinkedBlockAlignNext = spLinkedBlockCurrent;
						}
						spLinkedBlockCurrent = spLinkedBlockCurrent->spNext;
					}
					//if((spLinkedBlockCurrent != NULL) and (msb_aligned_PPN == spLinkedBlockCurrent->spBlock->nStartPPN >> 16)){
					//if((spLinkedBlockCurrent != NULL) and (msb_aligned_PPN == spLinkedBlockCurrent->spBlock->nStartPPN >> 16)
					//								  and (msb_aligned_PPN == msb_block1_PPN)
					//								  and (msb_aligned_PPN == msb_block2_PPN)){
					if((spLinkedBlockCurrent != NULL) and ((msb_aligned_PPN & msb_block1_PPN & msb_block2_PPN) == (spLinkedBlockCurrent->spBlock->nStartPPN >> 16))){
						nStartPPN                            = spLinkedBlockCurrent->spBlock->nStartPPN;
						spPageTable[nIndexPTE]->nBlockSize_3 = spLinkedBlockCurrent->spBlock->nBlockSize;
						spPageTable[nIndexPTE]->nPPN_3       = nStartPPN;
						if(spLinkedBlockCurrent->spBlock->nStartVPN == nStartAlignedVPN + GROUP_SIZE){
							spLinkedBlockAlignNext = spLinkedBlockCurrent;
						}
						spLinkedBlockCurrent = spLinkedBlockCurrent->spNext;
						spPageTable[nIndexPTE]->nCluster     = true;
					} else {
						spPageTable[nIndexPTE]->nCluster     = false;
					}

				}

			//}
			// Update index
			nIndexPTE++;

			//if(nIndexPTE % GROUP_SIZE == 0){
			if((nIndexPTE & (GROUP_SIZE - 1)) == 0){
				nStartAlignedVPN += GROUP_SIZE;
				if( (spLinkedBlockCurrent != NULL) && (spLinkedBlockCurrent->spBlock->nStartVPN == nStartAlignedVPN) ){		// in case full fragmentation
					continue;
				}
				spLinkedBlockCurrent = spLinkedBlockAlignNext;
			}
		}
	#endif // cRCPT_ENABLE


	// Measure time. Allocation finished.
	// clock_gettime(CLOCK_MONOTONIC, &TimeEnd_alloc);
   	#ifdef DEBUG_BUDDY	
		this->PageTable_Display(NUM_REQ_PAGES);
	#endif
	return (ERESULT_TYPE_SUCCESS);
};

// ----------------------------------------------
// Measure the average address coverage in a PTE
// ----------------------------------------------
/*
float CBuddy::GetAvgAddrCovrPTE(){
	int32_t sum = 0;
	float avg = 0.0f;
	for (int i = 0; i < NUM_REQUESTED_PAGES; i++) {
		#ifdef NAPOT
			if(this->spPageTable[i]->N_flag == 1){
				sum += (1 << NAPOT_BITS);
			} else{
				sum += 1;
			}
		#elif defined CAMB_ENABLE
			sum += this->spPageTable[i]->nBlockSize;
		#elif defined RCPT_ENABLE
			sum += this->spPageTable[i]->nBlockSize;
			sum += this->spPageTable[i]->nBlockSize_1;
		#elif defined cRCPT_ENABLE
			sum += this->spPageTable[i]->nBlockSize;
			sum += this->spPageTable[i]->nBlockSize_1;
			sum += this->spPageTable[i]->nBlockSize_3;
		#else
			sum += 1;
		#endif
	}
	avg = (float)sum / NUM_REQUESTED_PAGES;
	return avg;
}
*/

//----------------------------------------------------------
// De-allocate
//----------------------------------------------------------
//EResultType CBuddy::Do_Deallocate(int64_t START_VPN, int NUM_REQ_PAGES) { //DUONGTRAN comment
EResultType CBuddy::Do_Deallocate(int64_t START_VPN) {

	// Measure time. De-allocation start.
	// clock_gettime(CLOCK_MONOTONIC, &TimeStart_dealloc);
	
	// Get number of allocated blocks
	// // int NumAllocatedBlock = nBlockNum;

	// DUONGTRAN comment
	 //int NumAllocatedBlock = this->cpBlockList->GetCurNum();			// FIXME FIXME Multiple app.
	//int NumAllocatedBlock = NUM_REQ_PAGES; 						// FIXME FIXME FIXME BlockSize > 0
		

	#ifdef DEBUG_BUDDY
	//// Check number free pages
	//int NumFreePages = cpFreeList->GetNumFreePages();
	//assert (NUM_REQ_PAGES == (this->nInitial_NumFreePages - NumFreePages));
	#endif
 
	// Number of de-allocated blocks
	//int NumDeallocatedBlock = 0;  DUONGTRAN comment
	
	// Reset
	// int nIndexPTE = 0;	// START_VPN;	// FIXME Check
 
 
	// Get block-info
	SPLinkedBlock spLinkedBlock = this->cpBlockList->spLinkedBlock_head;
	#ifdef DEBUG_BUDDY
	assert (spLinkedBlock->spBlock != NULL);
	#endif

	//while (NumAllocatedBlock > 0) {   //DUONGTRAN comment
	// DUONGTRAN add 5 first lines
	while(spLinkedBlock != NULL){
 		if(spLinkedBlock->spBlock->nOriginalVPN != START_VPN){
			spLinkedBlock = spLinkedBlock->spNext;
			continue;
		}
		int nStartPPN  = spLinkedBlock->spBlock->nStartPPN;
		int nBlockSize = spLinkedBlock->spBlock->nBlockSize;
		#ifndef RBS
		int nOrder     = GetOrder(nBlockSize);
		#endif
	
		// Update FreeList. Merge buddy.
		#ifdef RBS
			this->cpFreeList->Merge_RangeBuddy(nStartPPN, nBlockSize);   // DUONGTRAN add for RangeBuddy
		#elif defined PBS
			this->cpFreeList->Merge_PageBuddy(nOrder, nStartPPN, nBlockSize);   // DUONGTRAN add for PageBuddy
		#elif defined BBS
			this->cpFreeList->Merge_Buddy(nOrder, nStartPPN);
		#else
			assert(0);
		#endif //RBS
		
		// Update "BlockList"
		//this->cpBlockList->RemoveHead();   // DUONGTRAN comment
		spLinkedBlock = this->cpBlockList->Remove(spLinkedBlock->spBlock->nStartPPN);	// DUONGTRAN add
		this->cpFreeList->nNumFreePage += nBlockSize;
        #ifdef DEBUG_BUDDY
//		this->cpFreeList->Display();
//		this->cpBlockList->Display();
		#endif
		
		// DUONGTRAN comment	
		//// Update number of allocated blocks 
		//NumAllocatedBlock --;
		//
		//// Update number of de-allocated blocks 
		//NumDeallocatedBlock ++;
		//
		//#ifdef DEBUG_BUDDY
		//assert (NumAllocatedBlock >= 0);
		//#endif
	};
 
	#ifdef DEBUG_BUDDY
	printf("---------------------------------------------\n");
	printf("Deallocations StartNPN %ld finished. \n", START_VPN);
	printf("---------------------------------------------\n");
	#endif

	// DUONGTRAN comment	
	//#ifdef DEBUG_BUDDY
	//assert (NumAllocatedBlock == 0);
	//assert (cpBlockList->GetCurNum() == 0);
	//#endif
	
	#ifdef DEBUG_BUDDY
	// DUONGTRAN comment to adaptive multiple allocation and deallocation
	//// Check free space 
	//NumFreePages = this->cpFreeList->GetNumFreePages();
	//assert (this->nInitial_NumFreePages == NumFreePages);
	#endif
 
	#ifdef DEBUG_BUDDY
		#ifdef BBS
			EResultType IsExist = this->cpFreeList->Is_Buddy_Exist_Not_Merged();
			if (IsExist == ERESULT_TYPE_YES) {
				printf("In FreeList, buddy remains un-merged.\n");
				assert (0);
			};
		#endif
//		this->cpFreeList->Display();
//		this->cpBlockList->Display();
	#endif

	// Destruct	// FIXME Be careful. Multiple applications
	// delete (this->cpBlockList);

	// Destruct	// FIXME Be careful. Multiple applications
	// for (int i=0; i < NUM_REQ_PAGES; i++) {
	//	delete (this->spPageTable[i]); 
	//	this->spPageTable[i] = NULL;
	// };
 
// #ifdef CAMB
 	// // delete (cpBlockArray);  // Delete only pointer FIXME
	// delete (this->cpPageArray);
// #endif
 
	// Measure time. De-allocation finished.
	// clock_gettime(CLOCK_MONOTONIC, &TimeEnd_dealloc);
	
	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Get page-table 
//-------------------------------------------------
SPPTE* CBuddy::Get_PageTable() {

	return (this->spPageTable);
};


//-------------------------------------------------
// Get FreeList 
//-------------------------------------------------
CPFreeList CBuddy::Get_FreeList() {

	return (this->cpFreeList);
};


//-------------------------------------------------
// Get BlockList 
//-------------------------------------------------
CPBlockList CBuddy::Get_BlockList() {

	return (this->cpBlockList);
};


//-------------------------------------------------
// Get BlockArray (CAMB)
//-------------------------------------------------
CPBlockArray CBuddy::Get_BlockArray() {

	return (this->cpBlockArray);
};


//-------------------------------------------------
// Get PageArray (CAMB)
//-------------------------------------------------
CPPageArray CBuddy::Get_PageArray() {

	return (this->cpPageArray);
};

int CBuddy::GetNumNodes(){
	return (this->cpFreeList->GetCurNum());
}


//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CBuddy::CheckBuddy() {

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Display page table 
//-------------------------------------------------
EResultType CBuddy::PageTable_Display(int Num_Requested_Pages) {

	printf("---------------------------------------------\n");
	printf("    Page table display\n");
	printf("---------------------------------------------\n");

	for (int i = 0; i < Num_Requested_Pages; i++) {
		// printf("\t Index        : \t %d\n", i);
		#ifndef cRCPT_ENABLE
		printf("\t PPN[%d]         : \t 0x%x\n", i, this->spPageTable[i]->nPPN);
		printf("\t BlockSize[%d]   : \t %d\n",   i, this->spPageTable[i]->nBlockSize);
		// printf("\t PageSize[%d] : \t %d\n",   i, spPageTable[i]->nPageSize);
		#endif

		#ifdef NAPOT
			printf("\t NAPOT[%d]       : \t %d\n", i, this->spPageTable[i]->N_flag);
		#endif

		#ifdef PCAD
			printf("\t Ascend[%d]       : \t %d\n", i, this->spPageTable[i]->nAscend);
			printf("\t Descend[%d]      : \t %d\n", i, this->spPageTable[i]->nDescend);
			#ifdef PBS
			printf("\t CodedPageSize[%d]: \t %d\n", i, this->spPageTable[i]->nCodedPageSize);
			#endif
		#endif

		#ifdef AT_ENABLE
			printf("\t Contiguity[%d]      : \t %d\n", i, this->spPageTable[i]->nContiguity);
		#endif

		#ifdef RCPT_ENABLE
			printf("\t PPN_1[%d]         : \t 0x%x\n", i, this->spPageTable[i]->nPPN_1);
			printf("\t BlockSize_1[%d]   : \t %d\n",   i, this->spPageTable[i]->nBlockSize_1);
		#endif

		#ifdef cRCPT_ENABLE
			if(i % GROUP_SIZE == 0){
				printf("\t PPN[%d]         : \t 0x%x\n", i, this->spPageTable[i]->nPPN);
				printf("\t BlockSize[%d]   : \t %d\n",   i, this->spPageTable[i]->nBlockSize);
			} else {
				printf("\t PPN_1[%d]         : \t 0x%x\n", i, this->spPageTable[i]->nPPN_1);
				printf("\t BlockSize_1[%d]   : \t %d\n",   i, this->spPageTable[i]->nBlockSize_1);
				printf("\t PPN_2[%d]         : \t 0x%x\n", i, this->spPageTable[i]->nPPN);
				printf("\t BlockSize_1[%d]   : \t %d\n",   i, this->spPageTable[i]->nBlockSize);
				if(this->spPageTable[i]->nCluster == true) {
					printf("\t PPN_3[%d]         : \t 0x%x\n", i, this->spPageTable[i]->nPPN_3);
					printf("\t BlockSize_1[%d]   : \t %d\n",   i, this->spPageTable[i]->nBlockSize_3);
				}
			}
		#endif
		printf("---------------------------------------------\n");
	};

	printf("---------------------------------------------\n");
	
	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Debug
//-------------------------------------------------
// EResultType CBuddy::Display() {
//
//	return (ERESULT_TYPE_SUCCESS);
// };

