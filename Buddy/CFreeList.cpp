//-----------------------------------------------------------
// Filename     : CFreeList.cpp 
// Version	: 0.31
// Date         : 25 Apr 2020
// Contact	: JaeYoung.Hur@gmail.com
// Description	: FreeList definition
//-----------------------------------------------------------
// Note
//		  Array of CQueue
//-----------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "CFreeList.h"
 
//-------------------------------------------------
// Constructor
// 	(1) "MAX_ORDER" known
// 	(2) FreeList[i] empty
//-------------------------------------------------
CFreeList::CFreeList(string cName){

	#ifdef DEBUG_BUDDY
	int nMaxBlockSize = GetBlockSize(MAX_ORDER - 1);
	assert (NUM_TOTAL_PAGES >= nMaxBlockSize);
	#endif

	// Generate 
	this->cpFreeList = new CQueue* [MAX_ORDER];
	for (int i=0; i<MAX_ORDER; i++) {
		char cName_tmp[50];
		sprintf(cName_tmp, "FreeList[%d]", i);
		this->cpFreeList[i] = new CQueue(cName_tmp, i);
	};

	// Initialize	
	this->cName = cName;
	this->nNumFreePage = 0;
	this->nStartPPN = 0;
};

CFreeList::CFreeList(string cName, uint64_t nStartPPN){

	#ifdef DEBUG_BUDDY
	int nMaxBlockSize = GetBlockSize(MAX_ORDER - 1);
	assert (NUM_TOTAL_PAGES >= nMaxBlockSize);
	#endif

	// Generate 
	this->cpFreeList = new CQueue* [MAX_ORDER];
	for (int i=0; i<MAX_ORDER; i++) {
		char cName_tmp[50];
		sprintf(cName_tmp, "FreeList[%d]", i);
		this->cpFreeList[i] = new CQueue(cName_tmp, i);
	};

	// Initialize	
	this->cName = cName;
	this->nNumFreePage = 0;
	this->nStartPPN = nStartPPN;
};


//-------------------------------------------------
// Destructor
//-------------------------------------------------
CFreeList::~CFreeList() {

	for (int i=0; i<MAX_ORDER; i++) { 
		delete (this->cpFreeList[i]); 
		this->cpFreeList[i] = NULL;
	};
};


//-------------------------------------------------
// Remove all entries 
//-------------------------------------------------
EResultType CFreeList::RemoveAll() {

	for (int i=0; i<MAX_ORDER; i++) { 
		this->cpFreeList[i]->RemoveAll(); 

		#ifdef DEBUG_BUDDY
		EResultType IsEmpty = this->cpFreeList[i]->IsEmpty();
		assert (IsEmpty == ERESULT_TYPE_YES);
		#endif
	};

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Set all entries free
// 	Free highest order
//-------------------------------------------------
EResultType CFreeList::SetFree_Initial() {

	#ifdef DEBUG_BUDDY
	int nBlockSizeMax = GetBlockSize(MAX_ORDER-1);
	assert (nBlockSizeMax <= NUM_TOTAL_PAGES);
	#endif

#if defined   SET_MAXORDER_MINUS_1_FREE
	this->cpFreeList[MAX_ORDER - 1]->SetFree_Initial(); 
#elif defined SET_MAXORDER_MINUS_2_FREE
 	this->cpFreeList[MAX_ORDER - 2]->SetFree_Initial(); 
#elif defined SET_MAXORDER_MINUS_3_FREE
 	this->cpFreeList[MAX_ORDER - 3]->SetFree_Initial(); 
#elif defined SET_MAXORDER_MINUS_4_FREE
 	this->cpFreeList[MAX_ORDER - 4]->SetFree_Initial(); 

#elif defined SET_ORDER_0_ONLY_FREE 
	this->cpFreeList[0]->SetFree_Initial(); 
#elif defined SET_ORDER_1_ONLY_FREE 
	this->cpFreeList[1]->SetFree_Initial(); 
#elif defined SET_ORDER_2_ONLY_FREE 
	this->cpFreeList[2]->SetFree_Initial(); 
#elif defined SET_ORDER_3_ONLY_FREE 
	this->cpFreeList[3]->SetFree_Initial(); 
#elif defined SET_ORDER_4_ONLY_FREE 
	this->cpFreeList[4]->SetFree_Initial(); 
#endif

	this->nNumFreePage = NUM_TOTAL_PAGES; //DUONGTRAN add
	
	return (ERESULT_TYPE_SUCCESS);
};

// DUONGTRAN add
EResultType CFreeList::SetFree_Initial(int reqBlockSize, int numBlock) {

	#ifdef DEBUG_BUDDY
	int nBlockSizeMax = GetBlockSize(MAX_ORDER-1);
	assert (nBlockSizeMax <= NUM_TOTAL_PAGES);
    assert (nBlockSizeMax >= reqBlockSize);
    assert ((reqBlockSize * numBlock) <= NUM_TOTAL_PAGES);
	#endif

	int order = -1;
	order = GetOrder(reqBlockSize);
	this->cpFreeList[order]->SetFree_Initial(reqBlockSize, numBlock);
	this->nNumFreePage = reqBlockSize * numBlock;
	
	return (ERESULT_TYPE_SUCCESS);
}

// DUONGTRAN add for test BBS with miss aligned address
EResultType CFreeList::SetFree_Initial(bool memmap[], char typeBuddy){
	this->nNumFreePage = NUM_TOTAL_PAGES;
	int totalpages = 0;
	if(typeBuddy == 'B'){ // BBS
		for(int order = MAX_ORDER - 1; order >= 0; order--) {
			int range = 0;
			int blocksize = 1 << order;
			for(int i = 0; i < NUM_TOTAL_PAGES * 2; i++) {
				if(i % blocksize == 0){
					if(range == blocksize){
						this->cpFreeList[order]->Push(i - range);
						for(int j = i-1; j >= i - range; j--){
							memmap[j] = false;
						}
						totalpages += range;
						if(totalpages == NUM_TOTAL_PAGES)
							return (ERESULT_TYPE_SUCCESS);
						range = 0;
					}
					if(memmap[i] == false){
						i += (blocksize - 1);
						continue;
					}
				}
				if(memmap[i] == true){
					range++;
				} else {
					range = 0;
					i = i - (i % blocksize) + blocksize - 1;
				}
			}
		}
	} else if(typeBuddy == 'R') { // RBS
		int range = 0;
		for(int i = 0; i < NUM_TOTAL_PAGES * 2; i++) {
			if(memmap[i] == true) {
				range++;
			} else {
				if(range == 0)
					continue;
				int order = GetOrder(range);
				this->cpFreeList[order]->Push(i-range, range);
				for(int j = i-1; j >= i - range; j--){
					memmap[j] = false;
				}
				totalpages += range;
				if(totalpages == NUM_TOTAL_PAGES)
					return (ERESULT_TYPE_SUCCESS);
				range = 0;
			}
		}
	} else if(typeBuddy == 'P') { // PBS
		#ifdef DEBUG_BUDDY
		assert((MAX_ORDER - 1) % GRANULARITY == 0);
		#endif
		
		for(int order = MAX_ORDER - 1; order >= GRANULARITY; order -= GRANULARITY){
			int range = 0;
			int multiple = 1 << order;
			for(int i = 0; i < NUM_TOTAL_PAGES * 2; i++){
				if(memmap[i] == true){
					if(i % multiple == 0){
						if(range % multiple != 0){
							range = 0;
						}
					}
					range++;
				} else {
					if(range == 0)
						continue;
					int missAlignedQuantity = i % multiple;
					int alignedRange = range - missAlignedQuantity;
					if((alignedRange % multiple == 0) && alignedRange > 0){
						this->cpFreeList[order]->Push(i-range, alignedRange);
						for(int j = i-1-missAlignedQuantity; j >= i - range; j--){
							memmap[j] = false;
						}
					}
					range = 0;
				}
			}
		}
		//order0
		int range = 0;
		int multiple = 1 << GRANULARITY;
		int step     = multiple;
		for(int i = 0; i < NUM_TOTAL_PAGES * 2; i++) {
			if(i < multiple) {
				if(memmap[i]){
					range++;
				} else {
					if(range == 0)
						continue;
					this->cpFreeList[0]->Push(i - range + 1, range);
					for(int j = i; j > i - range; j--){
						memmap[j] = false;
					}
					totalpages += range;
					if(totalpages == NUM_TOTAL_PAGES){
						return (ERESULT_TYPE_SUCCESS);
					}
					range = 0;
				}
			} else {	// i == multiple
				multiple += step;
				i--;
				if(range == 0){
					continue;
				}
				this->cpFreeList[0]->Push(i-range, range);
				for(int j = i; j >= i - range; j--){
					memmap[j] = false;
				}
				totalpages += range;
				if(totalpages == NUM_TOTAL_PAGES){
					return (ERESULT_TYPE_SUCCESS);
				}
				range = 0;
			}
		}
	} else {
		assert(0);
	}

	return (ERESULT_TYPE_SUCCESS);
}


// DUONGTRAN add for test BBS with miss aligned address, mini-zone, only BBS
EResultType CFreeList::SetFree_Initial(bool memmap[], char typeBuddy, int64_t startPPN){
	int totalpages = 0;
	if(typeBuddy == 'B'){ // BBS
		for(int order = MAX_ORDER - 1; order >= 0; order--) {
			int range = 0;
			int blocksize = 1 << order;
			for(int64_t i = startPPN; i < startPPN + MAX_NUMBER_PAGE_IN_MINI_ZONE; i++) {
				if(i >= NUM_TOTAL_PAGES * 2) break;
				if(i % blocksize == 0){
					if(range == blocksize){
						this->cpFreeList[order]->Push(i - range);
						for(int64_t j = i-1; j >= i - range; j--){
							//printf("DUONGTRAN DEBUG:  i = %ld\n", i);
							//printf("DUONGTRAN DEBUG:  r = %d\n", range);
							//printf("DUONGTRAN DEBUG:  j = %ld\n", j);
							//printf("DUONGTRAN DEBUG:  i-r = %ld\n", i-range);
							memmap[j] = false;
						}
						totalpages += range;
						//if(totalpages == NUM_TOTAL_PAGES)
						//	return (ERESULT_TYPE_SUCCESS);
						range = 0;
					}
					if(memmap[i] == false){
						i += (blocksize - 1);
						continue;
					}
				}
				if(memmap[i] == true){
					range++;
				} else {
					range = 0;
					i = i - (i % blocksize) + blocksize - 1;
				}
			}
		}
	} else {
		assert(0);
	}

	this->nNumFreePage = totalpages;

	return (ERESULT_TYPE_SUCCESS);
}

//-------------------------------------------------
// Push
//-------------------------------------------------
EResultType CFreeList::Push(int nOrder, int nStartPPN) {

	// this->CheckFreeList();
	
	this->cpFreeList[nOrder]->Push(nStartPPN);	

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Check buddy exist not merged
//-------------------------------------------------
EResultType CFreeList::Is_Buddy_Exist_Not_Merged() {

	// Debug
	// this->CheckFreeList();

	int Order = 0;
	
	EResultType IsExist = ERESULT_TYPE_UNDEFINED;

	while (Order < MAX_ORDER-1) {

		IsExist = this->cpFreeList[Order]->Is_Buddy_Exist_Not_Merged();	
		if (IsExist == ERESULT_TYPE_YES) {
			return (ERESULT_TYPE_YES);
		};

		Order++;	
	};

	return (ERESULT_TYPE_NO);
};


//-------------------------------------------------
// Merge recursive 
//-------------------------------------------------
EResultType CFreeList::Merge_Buddy(int nOrder, int nStartPPN) {	// BlockSize 2^i pages

	// Debug
	// this->CheckFreeList();

	int Order = nOrder;
	int PPN   = nStartPPN;

	while (Order < MAX_ORDER) {
		// Check buddy exist.
		int nBuddyPPN = GetBuddyPPN(Order, PPN);
		EResultType IsBuddyExist = this->cpFreeList[Order]->IsExist(nBuddyPPN);

		// (1) If buddy not exist, push and quit.
		if ((IsBuddyExist == ERESULT_TYPE_NO) or (Order == MAX_ORDER-1)) {
			// Push "PPN" in "Order". 	
			this->cpFreeList[Order]->Push(PPN);
			return (ERESULT_TYPE_SUCCESS);
		};

		#ifdef DEBUG_BUDDY	
		assert (IsBuddyExist == ERESULT_TYPE_YES);
		#endif

		// (2) If buddy exist, remove buddy. Scan ascending freelist. 
		this->cpFreeList[Order]->Remove(nBuddyPPN);
		PPN = GetBuddy_MergedPPN(Order, PPN);
		Order++;	
	};

	return (ERESULT_TYPE_SUCCESS);
};

// DUONGTRAN add for RangeBuddy
EResultType CFreeList::Merge_RangeBuddy(int nStartPPN, int nBlockSize) {
	int Order = 0;
	int BuddyPPN_left = nStartPPN;
	int BuddyPPN_right = nStartPPN + nBlockSize;
	int BlockSize = nBlockSize;
	int BlockSizeNext = 0;
	int MaxBlockSize =  GetBlockSize(MAX_ORDER);
	while(Order < MAX_ORDER){
		//IsBuddyExistGetBlockSize_right = this->cpFreeList[Order]->IsExistGetBlockSize_right(BuddyPPN_right);
		//IsBuddyExistGetBuddyPPN_left = this->cpFreeList[Order]->IsExistGetBuddyPPN_left(BuddyPPN_left);
		int IsBuddyExistGetBuddyPPN_left = -1;
		int IsBuddyExistGetBlockSize_right = 0;
		this->cpFreeList[Order]->IsExistGetBuddyPPNLeft_BlockSizeRight(nStartPPN, nBlockSize, &IsBuddyExistGetBuddyPPN_left, &IsBuddyExistGetBlockSize_right);

		// found the buddy_right 
	   	BlockSizeNext = BlockSize + IsBuddyExistGetBlockSize_right;
		if((IsBuddyExistGetBlockSize_right > 0) && (BlockSizeNext < MaxBlockSize)){
			this->cpFreeList[Order]->Remove(BuddyPPN_right);
			BlockSize = BlockSizeNext;
			BuddyPPN_right += IsBuddyExistGetBlockSize_right;
			IsBuddyExistGetBlockSize_right = 0;
		}
		// found the buddy_left
		BlockSizeNext = BlockSize + BuddyPPN_left - IsBuddyExistGetBuddyPPN_left;
		if((IsBuddyExistGetBuddyPPN_left >= 0) && (BlockSizeNext < MaxBlockSize)){
			this->cpFreeList[Order]->Remove(IsBuddyExistGetBuddyPPN_left);
			BlockSize = BlockSizeNext;
			BuddyPPN_left = IsBuddyExistGetBuddyPPN_left;
			IsBuddyExistGetBuddyPPN_left = -1;
		}

		// push and quit
		if(Order == MAX_ORDER-1){
			// get order
			 Order = GetOrder(BlockSize);	
			// Push to FreeList
			this->cpFreeList[Order]->Push(BuddyPPN_left, BlockSize);
			return (ERESULT_TYPE_SUCCESS);
		}
		Order++;	
	}
	return (ERESULT_TYPE_SUCCESS);
}

// DUONGTRAN add for PageBuddy
EResultType CFreeList::Merge_PageBuddy(int nOrder, int nStartPPN, int nBlockSize) {
	int BuddyPPN_left = nStartPPN;
	int BuddyPPN_right = nStartPPN + nBlockSize;
	int BlockSize = nBlockSize;
	int BlockSizeNext = 0;
	int MinBlockSizeNextOrder = 1 << (nOrder + GRANULARITY);
	//int IsBuddyExistGetBlockSize_right = this->cpFreeList[nOrder]->IsExistGetBlockSize_right(BuddyPPN_right);
	//int IsBuddyExistGetBuddyPPN_left = this->cpFreeList[nOrder]->IsExistGetBuddyPPN_left(BuddyPPN_left);
	int IsBuddyExistGetBuddyPPN_left = -1;
	int IsBuddyExistGetBlockSize_right = 0;
	this->cpFreeList[nOrder]->IsExistGetBuddyPPNLeft_BlockSizeRight(nStartPPN, nBlockSize, &IsBuddyExistGetBuddyPPN_left, &IsBuddyExistGetBlockSize_right);
	// found the buddy_right 
	BlockSizeNext = BlockSize + IsBuddyExistGetBlockSize_right;
	if(IsBuddyExistGetBlockSize_right > 0){
		this->cpFreeList[nOrder]->Remove(BuddyPPN_right);
		BlockSize = BlockSizeNext;
	}
	// found the buddy_left
	BlockSizeNext = BlockSize + BuddyPPN_left - IsBuddyExistGetBuddyPPN_left;
	if(IsBuddyExistGetBuddyPPN_left >= 0){
		this->cpFreeList[nOrder]->Remove(IsBuddyExistGetBuddyPPN_left);
		BlockSize = BlockSizeNext;
		BuddyPPN_left = IsBuddyExistGetBuddyPPN_left;
	}
	
	if(BlockSize < MinBlockSizeNextOrder || nOrder == MAX_ORDER){
		this->cpFreeList[nOrder]->Push(BuddyPPN_left, BlockSize);
		return (ERESULT_TYPE_SUCCESS);
	} else {
		int nextBuddyRightPPN = IsBuddyExistGetBlockSize_right + BuddyPPN_right;
		int mask = ~(MinBlockSizeNextOrder - 1);
		//int alignedAddrR = nextBuddyRightPPN - (nextBuddyRightPPN % MinBlockSizeNextOrder) - MinBlockSizeNextOrder;
		int alignedAddrR = (nextBuddyRightPPN & mask) - MinBlockSizeNextOrder;
		//int checkBuddyPPN_leftNotAligned = BuddyPPN_left % MinBlockSizeNextOrder;
		//int checkBuddyPPN_leftNotAligned = BuddyPPN_left - (BuddyPPN_left & (~(MinBlockSizeNextOrder - 1)));
		int checkBuddyPPN_leftNotAligned = BuddyPPN_left - (BuddyPPN_left & mask);
		int alignedAddrL = (checkBuddyPPN_leftNotAligned) ? (BuddyPPN_left - checkBuddyPPN_leftNotAligned + MinBlockSizeNextOrder) : BuddyPPN_left;
		if(alignedAddrR == alignedAddrL){
			Merge_PageBuddy(nOrder + GRANULARITY, alignedAddrL, MinBlockSizeNextOrder);
		} else if(alignedAddrR > alignedAddrL){
			Merge_PageBuddy(nOrder + GRANULARITY, alignedAddrL, MinBlockSizeNextOrder << 1);
		} else {
			return (ERESULT_TYPE_SUCCESS);
		}
		int remainL = alignedAddrL - BuddyPPN_left;
		if(remainL > 0){
			this->cpFreeList[nOrder]->Push(BuddyPPN_left, remainL);
		}
		int remainR = nextBuddyRightPPN - alignedAddrR - MinBlockSizeNextOrder;
		if(remainR > 0){
			this->cpFreeList[nOrder]->Push(alignedAddrR + MinBlockSizeNextOrder, remainR);
		}
	}
	return (ERESULT_TYPE_SUCCESS);
}

//-------------------------------------------------
// Expand recursive up
//-------------------------------------------------
EResultType CFreeList::Expand(int nOrder) {		// BlockSize 2^i pages
     
	// Debug
	// this->CheckFreeList();
	
	// int Order = nOrder;
	int PPN   = -1;

	EResultType IsEmpty = ERESULT_TYPE_UNDEFINED;
	
	// for (int i=Order; i<MAX_ORDER; i++) {		// FIXME FIXME Need? Seems not

	// (1) Check empty. If emtpy, increment Order. Call recursive up
	IsEmpty = this->cpFreeList[nOrder]->IsEmpty();
	if (IsEmpty == ERESULT_TYPE_YES) {
		this->Expand(nOrder+1);
	};

	// (2) Check empty. If not empty. Split down. Return.
	IsEmpty = this->cpFreeList[nOrder]->IsEmpty();
	if (IsEmpty == ERESULT_TYPE_NO) {

		PPN = this->cpFreeList[nOrder]->GetStartPPN_Head();
		this->Split(nOrder, PPN);

		#ifdef DEBUG_BUDDY
		assert (PPN >= 0);
		#endif

		return (ERESULT_TYPE_SUCCESS);
	};
		

	// };
	
	return (ERESULT_TYPE_SUCCESS);
};

// DUONGTRAN add for RangeBuddy
EResultType CFreeList::Expand(int nOrder, int nNumRequestedPages) {		// BlockSize 2^i pages
#ifndef BBS     
	// Debug
	// this->CheckFreeList();
	
	// int Order = nOrder;

	EResultType IsEmpty = ERESULT_TYPE_UNDEFINED;
	
	// for (int i=Order; i<MAX_ORDER; i++) {		// FIXME FIXME Need? Seems not

	// (1) Check empty. If no emtpy, split then end
	IsEmpty = this->cpFreeList[nOrder]->IsEmpty();

	if(IsEmpty == ERESULT_TYPE_YES) {
		#ifdef RBS
			this->Expand(nOrder+1, nNumRequestedPages);
		#elif defined PBS
			this->Expand(nOrder + GRANULARITY, 1 << (nOrder + GRANULARITY));
		#else
			assert(0);
		#endif
	}

	// (2) Check empty. If not empty. Split down. Return.
	IsEmpty = this->cpFreeList[nOrder]->IsEmpty();
	if (IsEmpty == ERESULT_TYPE_NO) {
        #ifdef RBS
        	int PPN = this->cpFreeList[nOrder]->GetStartPPN_Head();
			#ifdef DEBUG_BUDDY
				assert (PPN >= 0);
			#endif
        	this->SplitRange(nOrder, PPN, nNumRequestedPages);
        #elif defined PBS
			this->SplitPage(nOrder, GRANULARITY, 1 << nOrder); // GRANULARITY mean expand
        #endif //RBS
	}


	//if (IsEmpty == ERESULT_TYPE_NO) {
    //    #ifdef RBS
    //    	int PPN = this->cpFreeList[nOrder]->GetStartPPN_Head();
	//		#ifdef DEBUG_BUDDY
	//			assert (PPN >= 0);
	//		#endif
    //    	this->SplitRange(nOrder, PPN, nNumRequestedPages);
    //    #elif defined PBS
	//		this->SplitPage(nOrder, true, 1 << nOrder); // true mean expand
    //    #endif //RBS
	//} else { // call the next order
	//	#ifdef RBS
	//		this->Expand(nOrder+1, nNumRequestedPages);
	//	#elif defined PBS
	//		this->Expand(nOrder+4, 1 << (nOrder + 4));
	//	#else
	//		assert(0);
	//	#endif
	//	
	//}
	
#endif //BBS
	return (ERESULT_TYPE_SUCCESS);
};

//-------------------------------------------------
// Split down 
// 	(1) Remove "nStartPPN" in "order i"
// 	(2) Push two nodes ("nStartPPN" and "nStartPPN + 1") in "order i-1"	
//-------------------------------------------------
EResultType CFreeList::Split(int nOrder, int nStartPPN) {

	// Debug
	// this->CheckFreeList();

	// Check order
	if (nOrder == 0) {
		return (ERESULT_TYPE_FAIL);	
	};

 	// Remove "nStartPPN" in "order i"
	this->cpFreeList[nOrder]->Remove(nStartPPN);


 	// Push two nodes in "order i-1"	
	this->cpFreeList[nOrder - 1]->Push(nStartPPN);

	int nBlockSize = GetBlockSize(nOrder - 1); 
	this->cpFreeList[nOrder - 1]->Push(nStartPPN + nBlockSize);

	return (ERESULT_TYPE_SUCCESS);	
};

// DUONGTRAN add for RangeBuddy
EResultType CFreeList::SplitRange(int nOrder, int nStartPPN, int nNumRequestedPages) {

	// Debug
	// this->CheckFreeList();

	// Check order
	if (nOrder == 0) {
		return (ERESULT_TYPE_FAIL);	
	};
	int nRemainHeadBlockSize = this->cpFreeList[nOrder]->spLinkedNode_head->spNode->nBlockSize;
	#ifdef DEBUG_BUDDY
		assert(nRemainHeadBlockSize > nNumRequestedPages);
	#endif
	nRemainHeadBlockSize -= nNumRequestedPages;
	int newOrderLeft = GetOrder(nNumRequestedPages); //from BuddyTop
	int newOrderRight = GetOrder(nRemainHeadBlockSize); //from BuddyTop

	// Push right side to the new order
	if(newOrderRight != nOrder){
		this->cpFreeList[nOrder]->RemoveHead(); // Remove Head Node
		this->cpFreeList[newOrderRight]->Push(nStartPPN + nNumRequestedPages, nRemainHeadBlockSize);  // Push right side to the new order
	} else { // update nStartPPN and nBlockSize of head node
		this->cpFreeList[nOrder]->spLinkedNode_head->spNode->nStartPPN = nStartPPN + nNumRequestedPages;
		this->cpFreeList[nOrder]->spLinkedNode_head->spNode->nBlockSize = nRemainHeadBlockSize;
	}
	// Push left side to the new order
	this->cpFreeList[newOrderLeft]->PushHead(nStartPPN, nNumRequestedPages);

	return (ERESULT_TYPE_SUCCESS);	
};

// DUONGTRAN add for PageBuddy
EResultType CFreeList::SplitPage(int nOrder, int expand, int nNumRequestedPages) {

	// Debug
	// this->CheckFreeList();

	int nRemainHeadBlockSize = this->cpFreeList[nOrder]->spLinkedNode_head->spNode->nBlockSize;
	int nStartPPN            = this->cpFreeList[nOrder]->spLinkedNode_head->spNode->nStartPPN;
	#ifdef DEBUG_BUDDY
		assert(nRemainHeadBlockSize >= nNumRequestedPages);
		assert(nNumRequestedPages % (1 << nOrder) == 0);
	#endif
	if(nRemainHeadBlockSize == nNumRequestedPages){ // Expand case and the block size of head node is minimum
		this->cpFreeList[nOrder]->RemoveHead(); // Remove Head Node
		this->cpFreeList[nOrder - GRANULARITY]->PushHead(nStartPPN, nNumRequestedPages);
	} else {
		nRemainHeadBlockSize -= nNumRequestedPages;
		this->cpFreeList[nOrder]->spLinkedNode_head->spNode->nStartPPN = nStartPPN + nNumRequestedPages;
		this->cpFreeList[nOrder]->spLinkedNode_head->spNode->nBlockSize = nRemainHeadBlockSize;
		this->cpFreeList[nOrder - expand]->PushHead(nStartPPN, nNumRequestedPages);
	}

	return (ERESULT_TYPE_SUCCESS);	
};

//-------------------------------------------------
// Remove
//-------------------------------------------------
// Remove
// 	Remove node in FreeList[i] if StartPPN matched
//-------------------------------------------------
EResultType CFreeList::Remove(int nOrder, int nStartPPN) {

	// Debug
	// this->CheckFreeList();

	this->cpFreeList[nOrder]->Remove(nStartPPN);

	return (ERESULT_TYPE_SUCCESS);	
};


//-------------------------------------------------
// Get FreeList[i] 
//-------------------------------------------------
CPQueue CFreeList::GetQueue(int nOrder) {

	return (this->cpFreeList[nOrder]);	
};


//-------------------------------------------------
// Get StartPPN head 
//-------------------------------------------------
int CFreeList::GetStartPPN_Head(int nOrder) {

	int nStartPPN = this->cpFreeList[nOrder]->GetStartPPN_Head();
	return (nStartPPN);	
};

int CFreeList::GetBlockSize_Head(int nOrder) {
	return this->cpFreeList[nOrder]->GetBlockSize_Head();
}

//-------------------------------------------------
// Get targeted order 
// 	If size fits, return order
// 	If size not fit, return 0 (4kB page)
//-------------------------------------------------
int CFreeList::GetTargetOrder(int NumRequestedPages) {

	EResultType High_order_has_entry = ERESULT_TYPE_NO;
	#ifdef PBS
		#ifdef DEBUG_BUDDY
			assert((MAX_ORDER-1) % GRANULARITY == 0);
		#endif
		int step = GRANULARITY;
	#else
		int step = 1;
	#endif

	for (int i=MAX_ORDER-1; i>=0; i = i - step) {
	
		int nBlockSize = GetBlockSize(i);       // Num. of pages

		// FreeList has page. 
		if ( (this->cpFreeList[i]->GetCurNum() > 0) or (High_order_has_entry == ERESULT_TYPE_YES) ) {
	
			High_order_has_entry = ERESULT_TYPE_YES;	

			// Block size fits
			if (nBlockSize <= NumRequestedPages) {

				#ifdef DEBUG_BUDDY
				assert (i >= 0);	
				assert (i <  MAX_ORDER);	
				#endif

				return (i);
			};
		};
	};

	// Nothing fit 
	assert (0);
	return (-1);	
};


//-------------------------------------------------
// Get number of available (4kB-size) pages 
//-------------------------------------------------
int CFreeList::GetNumFreePages() {

	int nAvailablePages = 0;

	// Get number of available pages
	for (int i=0; i<MAX_ORDER; i++) {
		int nBlockSize = GetBlockSize(i);
		int nOccupancy = this->cpFreeList[i]->GetCurNum();	
		int nPages = nOccupancy * nBlockSize;
		nAvailablePages += nPages;
	};

	return (nAvailablePages);
};


//-------------------------------------------------
// Get occupancy FreeList[i]
//-------------------------------------------------
int CFreeList::GetCurNum(int nOrder) {

	int nCurNum = this->cpFreeList[nOrder]->GetCurNum();

	return (nCurNum);
};

int CFreeList::GetCurNum() {
	int nAvailablePages = 0;
	for (int i=0; i<MAX_ORDER; i++) {
		int nNodes = GetCurNum(i);
		nAvailablePages += nNodes;
	};

	return (nAvailablePages);
}

//-------------------------------------------------
// Check FreeList[i] empty
//-------------------------------------------------
EResultType CFreeList::IsEmpty(int nOrder) {

	EResultType IsEmpty = this->cpFreeList[nOrder]->IsEmpty();

	return (IsEmpty);
};


//-------------------------------------------------
// Check FreeList[i] full 
//-------------------------------------------------
EResultType CFreeList::IsFull(int nOrder) {

	EResultType IsFull = this->cpFreeList[nOrder]->IsFull();

	return (IsFull);
};


//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CFreeList::CheckFreeList() {

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CFreeList::Display() {

	for (int j=0; j<MAX_ORDER; j++) {
		this->cpFreeList[j]->Display();
	};

	return (ERESULT_TYPE_SUCCESS);
};

