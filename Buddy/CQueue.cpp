//-----------------------------------------------------------
// Filename     : CQueue.cpp 
// Version	: 0.2
// Date         : 13 Nov 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Queue definition
//-----------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "CQueue.h"

//-------------------------------------------------
// Constructor
// 	"Number of total pages" known
//-------------------------------------------------
CQueue::CQueue(string cName, int nOrder){

	// Generate node later when push conducted
	this->spLinkedNode_head = NULL;	
	this->spLinkedNode_tail = NULL;	

	// Initialize	
	this->cName   = cName;
	this->nMaxNum = NUM_TOTAL_PAGES >> nOrder;
	this->nOrder  = nOrder;
	this->nCurNum = 0;
};


//-------------------------------------------------
// Destructor
//-------------------------------------------------
CQueue::~CQueue() {

	// Delete all nodes
	SPLinkedNode spScan   = NULL;
	SPLinkedNode spTarget = NULL;
	spScan = this->spLinkedNode_head;

	while (spScan != NULL) {
		spTarget = spScan;
		spScan   = spScan->spNext;
		delete (spTarget->spNode);		// Careful. 
		spTarget->spNode = NULL;		// Careful. 

		// spTarget->spPrev = NULL;		// Careful. 
		// spTarget->spNext = NULL;		
		spTarget = NULL;	
	};

	this->spLinkedNode_head = NULL;			// Careful. 
	this->spLinkedNode_tail = NULL;

	#ifdef DEBUG_BUDDY
	assert (this->spLinkedNode_head == NULL);
	assert (this->spLinkedNode_tail == NULL);
	#endif
};


//-------------------------------------------------
// Remove all entries 
//-------------------------------------------------
EResultType CQueue::RemoveAll() {

	// Delete all nodes 
	SPLinkedNode spScan = NULL;
	SPLinkedNode spTarget = NULL;
	spScan = this->spLinkedNode_head;

	while (spScan != NULL) {
		spTarget = spScan;
		spScan   = spScan->spNext;
		delete (spTarget->spNode);		// Careful. 
		spTarget->spNode = NULL;		// Careful. 

		spTarget->spPrev = NULL;		// Careful 
		spTarget->spNext = NULL;		
		spTarget = NULL;	
	};

	// Initialize	
	this->nCurNum = 0;
	this->spLinkedNode_head = NULL;	
	this->spLinkedNode_tail = NULL;	

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Set entries free
//-------------------------------------------------
EResultType CQueue::SetFree_Initial() {

	int nNumEntries = this->nMaxNum;
	int nBlockSize = GetBlockSize(this->nOrder);

#ifdef SET_MAXORDER_MINUS_1_FREE
	// MAX_ORDER : Fill every node
	for (int i=0; i<nNumEntries; i++) { 
		#ifdef RBS
			this->Push(i * nBlockSize, nBlockSize);
		#else
			this->Push(i * nBlockSize);
		#endif
	};

	#ifdef DEBUG_BUDDY
	assert (this->GetCurNum() == this->nMaxNum);
	#endif
#else
	// Fill every other node
	for (int i=0; i<(nNumEntries/2); i++) { 
		this->Push(2 * i * nBlockSize); 
	};

	#ifdef DEBUG_BUDDY
	assert (this->GetCurNum() == this->nMaxNum / 2);
	#endif
#endif

	#ifdef DEBUG_BUDDY
	// assert (this->GetCurNum() == this->nMaxNum);
	// assert (this->nCurNum     == this->nMaxNum);
	assert (this->spLinkedNode_head != NULL);	
	assert (this->spLinkedNode_tail != NULL);	
	#endif

	return (ERESULT_TYPE_SUCCESS);
};

// DUONGTRAN add
EResultType CQueue::SetFree_Initial(int reqBlockSize, int numBlock) {

	for (int i = 0; i < numBlock; i++) {
		#ifdef RBS
			this->Push((i * reqBlockSize) << 1, reqBlockSize); 
		#else // BBS
			this->Push((i * reqBlockSize) << 1);
		#endif //RBS
	};

	#ifdef DEBUG_BUDDY
	assert (this->GetCurNum() == numBlock);
	#endif

	#ifdef DEBUG_BUDDY
	// assert (this->GetCurNum() == this->nMaxNum);
	// assert (this->nCurNum     == this->nMaxNum);
	assert (this->spLinkedNode_head != NULL);	
	assert (this->spLinkedNode_tail != NULL);	
	#endif

	return (ERESULT_TYPE_SUCCESS);
};

//-------------------------------------------------
// Check buddy not merged 
//-------------------------------------------------
EResultType CQueue::Is_Buddy_Exist_Not_Merged() {

	SPLinkedNode spScan = this->spLinkedNode_head;
	EResultType IsExist = ERESULT_TYPE_UNDEFINED;
	int MyPPN    = -1;
	int BuddyPPN = -1;

	while (spScan != NULL) {

		MyPPN = spScan->spNode->nStartPPN;
		BuddyPPN = GetBuddyPPN(this->nOrder, MyPPN);

		#ifdef DEBUG_BUDDY
		int nBlockSize = GetBlockSize(this->nOrder);
		assert (MyPPN    != -1);
		assert (BuddyPPN != -1);
		assert (MyPPN    % nBlockSize == 0);
		assert (BuddyPPN % nBlockSize == 0);
		#endif
			
		IsExist = this->IsExist(BuddyPPN);
		if (IsExist == ERESULT_TYPE_YES) {	
			return (ERESULT_TYPE_YES);
		};

		spScan   = spScan->spNext;
	};

	return (ERESULT_TYPE_NO);
};


//-------------------------------------------------
// Push
//-------------------------------------------------
// Function
//	Insert node in-order wrt StartPPN 
//	(1) If empty, just push 
//	(2) Search. Find target-node (first-node greater than StartPPN)
//	    - If target-node not exist, add to tail
//	    - If target-node is head, insert before head
//	    - If target-node is not head, insert before target-node
// Maintenance
// 	Generate linked-node inside 
//------------------------------------------------
EResultType CQueue::Push(int nStartPPN) {

	#ifdef DEBUG_BUDDY
	assert (nStartPPN>= 0);
	#endif 

	#ifdef DEBUG_BUDDY
	// Check exist
	EResultType IsExist = this->IsExist(nStartPPN);
	if (IsExist == ERESULT_TYPE_YES) {
		printf("PPN %x already exists.\n", nStartPPN);
		assert (0);
	};
	#endif

	// Generate linked-node
	SPLinkedNode spLinkedNode_new = new SLinkedNode;

	// Generate node. Initialize.
	SPNode spNode_new = new SNode;
	spNode_new->nStartPPN = nStartPPN;

	// Initialize linked-node
	spLinkedNode_new->spNode = spNode_new;
	spLinkedNode_new->spPrev = NULL;
	spLinkedNode_new->spNext = NULL;

	// Find target-node (first-node greater than StartPPN)
	SPLinkedNode spScan = NULL;
	SPLinkedNode spTarget = NULL;
	spScan = this->spLinkedNode_head;
	while (spScan != NULL) {
		if (spScan->spNode->nStartPPN > nStartPPN) {
			spTarget = spScan;
			break;
	        };
		spScan = spScan->spNext;
	};

	// Push
	// (1) Empty
	if (this->spLinkedNode_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->nCurNum == 0);
		assert (this->spLinkedNode_tail == NULL);
		#endif

		this->spLinkedNode_head = spLinkedNode_new;
		this->spLinkedNode_tail = spLinkedNode_new;
	}
	// (2) No target-node
	else if (spTarget == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->nCurNum > 0);
		assert (this->spLinkedNode_head != NULL);
		assert (this->spLinkedNode_tail != NULL);
		#endif

		// Add to tail 
		this->spLinkedNode_tail->spNext = spLinkedNode_new;
		spLinkedNode_new->spPrev        = this->spLinkedNode_tail;
		this->spLinkedNode_tail         = spLinkedNode_new;
	}
	// (3) Target is head
	else if (spTarget == this->spLinkedNode_head) {

		#ifdef DEBUG_BUDDY
		assert (this->nCurNum > 0);
		// assert (spTarget->spPrev == NULL);			// Careful 
		#endif

		// Insert before head 
		this->spLinkedNode_head->spPrev = spLinkedNode_new;
		spLinkedNode_new->spNext        = this->spLinkedNode_head;
		this->spLinkedNode_head         = spLinkedNode_new;
	}
	// (4) Target is not head
	else if (spTarget != this->spLinkedNode_head) {

		#ifdef DEBUG_BUDDY
		assert (this->nCurNum > 0);
		assert (this->spLinkedNode_head != NULL);
		assert (this->spLinkedNode_tail != NULL);
		assert (spTarget->spPrev != NULL);
		#endif

		// Insert before target
		spTarget->spPrev->spNext = spLinkedNode_new;
		spLinkedNode_new->spNext = spTarget; 			//DUONGTRAN add
		spLinkedNode_new->spPrev = spTarget->spPrev;    //DUONGTRAN add 
		spTarget->spPrev         = spLinkedNode_new;
	}
	else {
		assert (0);
	};	

	// Increment occupancy
	this->nCurNum++;

	#ifdef DEBUG_BUDDY
	assert (this->spLinkedNode_head != NULL);
	assert (this->spLinkedNode_tail != NULL);
	// this->CheckQueue();
	#endif

	return (ERESULT_TYPE_SUCCESS);
};

// DUONGTRAN add for RangeBuddy
EResultType CQueue::Push(int nStartPPN, int nBlockSize) {

	#ifdef DEBUG_BUDDY
	assert (nStartPPN>= 0);
	#endif 

	#ifdef DEBUG_BUDDY
	// Check exist
	EResultType IsExist = this->IsExist(nStartPPN);
	if (IsExist == ERESULT_TYPE_YES) {
		printf("PPN %x already exists.\n", nStartPPN);
		assert (0);
	};
	#endif

	// Generate linked-node
	SPLinkedNode spLinkedNode_new = new SLinkedNode;

	// Generate node. Initialize.
	SPNode spNode_new = new SNode;
	spNode_new->nStartPPN = nStartPPN;
	spNode_new->nBlockSize = nBlockSize;

	// Initialize linked-node
	spLinkedNode_new->spNode = spNode_new;
	spLinkedNode_new->spPrev = NULL;
	spLinkedNode_new->spNext = NULL;

	// Find target-node (first-node greater than StartPPN)
	SPLinkedNode spScan = NULL;
	SPLinkedNode spTarget = NULL;
	spScan = this->spLinkedNode_head;
	while(spScan != NULL) {
		if(spScan->spNode->nStartPPN >= nStartPPN) {
			spTarget = spScan;
			break;
	    };
		spScan = spScan->spNext;
	};

	// Push
	// (1) Empty
	if (this->spLinkedNode_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->nCurNum == 0);
		assert (this->spLinkedNode_tail == NULL);
		#endif

		this->spLinkedNode_head = spLinkedNode_new;
		this->spLinkedNode_tail = spLinkedNode_new;
	}
	// (2) No target-node
	else if (spTarget == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->nCurNum > 0);
		assert (this->spLinkedNode_head != NULL);
		assert (this->spLinkedNode_tail != NULL);
		#endif

		// Add to tail 
		this->spLinkedNode_tail->spNext = spLinkedNode_new;
		spLinkedNode_new->spPrev        = this->spLinkedNode_tail;
		this->spLinkedNode_tail         = spLinkedNode_new;
	}
	// (3) Target is head
	else if (spTarget == this->spLinkedNode_head) {

		#ifdef DEBUG_BUDDY
		assert (this->nCurNum > 0);
		// assert (spTarget->spPrev == NULL);			// Careful 
		#endif

		// Insert before head 
		this->spLinkedNode_head->spPrev = spLinkedNode_new;
		spLinkedNode_new->spNext        = this->spLinkedNode_head;
		this->spLinkedNode_head         = spLinkedNode_new;
	}
	// (4) Target is not head
	else if (spTarget != this->spLinkedNode_head) {

		#ifdef DEBUG_BUDDY
		assert (this->nCurNum > 0);
		assert (this->spLinkedNode_head != NULL);
		assert (this->spLinkedNode_tail != NULL);
		assert (spTarget->spPrev != NULL);
		#endif

		// Insert before target
		spTarget->spPrev->spNext = spLinkedNode_new;
		spLinkedNode_new->spNext = spTarget;
		spLinkedNode_new->spPrev = spTarget->spPrev;
		spTarget->spPrev         = spLinkedNode_new;
	}
	else {
		assert (0);
	};	

	// Increment occupancy
	this->nCurNum++;

	#ifdef DEBUG_BUDDY
	assert (this->spLinkedNode_head != NULL);
	assert (this->spLinkedNode_tail != NULL);
	// this->CheckQueue();
	#endif

	return (ERESULT_TYPE_SUCCESS);
};

// DUONGTRAN add for RangeBuddy
EResultType CQueue::PushHead(int nStartPPN, int nBlockSize) {

	#ifdef DEBUG_BUDDY
	assert (nStartPPN>= 0);
	#endif 

	#ifdef DEBUG_BUDDY
	// Check exist
	EResultType IsExist = this->IsExist(nStartPPN);
	if (IsExist == ERESULT_TYPE_YES) {
		printf("PPN %x already exists.\n", nStartPPN);
		assert (0);
	};
	#endif

	// Generate linked-node
	SPLinkedNode spLinkedNode_new = new SLinkedNode;

	// Generate node. Initialize.
	SPNode spNode_new = new SNode;
	spNode_new->nStartPPN = nStartPPN;
	spNode_new->nBlockSize = nBlockSize;

	// Initialize linked-node
	spLinkedNode_new->spNode = spNode_new;
	spLinkedNode_new->spPrev = NULL;
	spLinkedNode_new->spNext = NULL;

	// Push
	// (1) Empty
	if(this->spLinkedNode_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->nCurNum == 0);
		assert (this->spLinkedNode_tail == NULL);
		#endif

		this->spLinkedNode_head = spLinkedNode_new;
		this->spLinkedNode_tail = spLinkedNode_new;
	} else {	// (2) Target is head
		#ifdef DEBUG_BUDDY
		assert (this->nCurNum > 0);
		// assert (spTarget->spPrev == NULL);			// Careful 
		#endif

		// Insert before head 
		this->spLinkedNode_head->spPrev = spLinkedNode_new;
		spLinkedNode_new->spNext        = this->spLinkedNode_head;
		this->spLinkedNode_head         = spLinkedNode_new;
	}

	// Increment occupancy
	this->nCurNum++;

	#ifdef DEBUG_BUDDY
	assert (this->spLinkedNode_head != NULL);
	assert (this->spLinkedNode_tail != NULL);
	// this->CheckQueue();
	#endif

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Remove node matching StartPPN 
//-------------------------------------------------
EResultType CQueue::Remove(int nStartPPN) {

	#ifdef DEBUG_BUDDY
	// this->CheckQueue();
	#endif

	// Get target node 
	SPLinkedNode spScan = NULL;
	SPLinkedNode spTarget = NULL;
	spScan = this->spLinkedNode_head;
	while (spScan != NULL) {
		if (spScan->spNode->nStartPPN == nStartPPN) {
			spTarget = spScan;
			break;
		};
		spScan = spScan->spNext;
	};

	// No target 
	if (spTarget == NULL) {
		printf("Remove StartPPN %d in FreeList[%d]. No node found.\n", nStartPPN, this->nOrder);	
		assert (0);
		return (ERESULT_TYPE_FAIL);
	};

	#ifdef DEBUG_BUDDY
	assert (spTarget != NULL);
	#endif

	// Decrement occupancy
	this->nCurNum--;

	// Pop target 
	if (this->nCurNum == 0) {
		this->spLinkedNode_head = NULL;
		this->spLinkedNode_tail = NULL;
	} 
	else {
		#ifdef DEBUG_BUDDY
		assert (this->nCurNum >= 1);
		assert (this->spLinkedNode_head != NULL);
		assert (this->spLinkedNode_tail != NULL);
		#endif

		if (spTarget == this->spLinkedNode_head) {
			this->spLinkedNode_head = this->spLinkedNode_head->spNext;
		} 
		else if (spTarget == this->spLinkedNode_tail) {
			this->spLinkedNode_tail = spTarget->spPrev;
			spTarget->spPrev->spNext = NULL; 
		} 
		else {
			spTarget->spPrev->spNext = spTarget->spNext;
			spTarget->spNext->spPrev = spTarget->spPrev;
		};
	};

	#ifdef DEBUG_BUDDY
	assert (spTarget != NULL);
	// this->CheckQueue();
	#endif
	
	// Maintain
	delete (spTarget->spNode);
	spTarget->spNode = NULL;
	delete (spTarget);
	spTarget = NULL;

	return (ERESULT_TYPE_SUCCESS);	
};


//-------------------------------------------------
// Remove head
//-------------------------------------------------
EResultType CQueue::RemoveHead() {

	#ifdef DEBUG_BUDDY
	// this->CheckQueue();
	#endif
	
	// Get head
	SPLinkedNode spTarget = this->spLinkedNode_head;

	#ifdef DEBUG_BUDDY
	// Check empty 
	if (spTarget == NULL) {
		printf("Remove head in FreeList[%d]. No node found.\n", this->nOrder);	
		assert (0);
		return (ERESULT_TYPE_FAIL);
	};
	#endif

	// Decrement occupancy
	this->nCurNum--;
	
	// Pop head
	if (this->nCurNum == 0) {
		this->spLinkedNode_head = NULL;
		this->spLinkedNode_tail = NULL;
	} 
	else {
		this->spLinkedNode_head = this->spLinkedNode_head->spNext;
	};
	
	#ifdef DEBUG_BUDDY
	assert (spTarget != NULL);
	// this->CheckQueue();
	#endif
	
	// Maintain
	delete (spTarget->spNode);
	spTarget->spNode = NULL;
	delete (spTarget);
	spTarget = NULL;
	
	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Get node matching StartPPN 
//-------------------------------------------------
SPNode CQueue::GetNode(int nStartPPN) {

	#ifdef DEBUG_BUDDY
	// this->CheckQueue();
	#endif

	#ifdef DEBUG_BUDDY
	// Check empty 
	if (this->IsEmpty() == ERESULT_TYPE_YES) {
		printf("Get node matching StartPPN %d. FreeList[%d] is empty.\n", nStartPPN, this->nOrder);	
		assert (0);
	};
	#endif

	// Get target node 
	SPLinkedNode spScan = NULL;
	SPLinkedNode spTarget = NULL;
	spScan = this->spLinkedNode_head;

	while (spScan != NULL) {
		if (spScan->spNode->nStartPPN == nStartPPN) {
			spTarget = spScan;
			return (spTarget->spNode);
		};
		spScan = spScan->spNext;
	};

	return (NULL);
};


//-------------------------------------------------
// Get head
//-------------------------------------------------
SPNode CQueue::GetHead() {

	#ifdef DEBUG_BUDDY
	// this->CheckQueue();
	#endif

	#ifdef DEBUG_BUDDY
	// Check empty 
	if (this->IsEmpty() == ERESULT_TYPE_YES) {
		printf("Get head node. FreeList[%d] is empty.\n", this->nOrder);	
		assert (0);
	};
	#endif

	// Get head
	SPLinkedNode spTarget = this->spLinkedNode_head;
	
	#ifdef DEBUG_BUDDY
	assert (spTarget != NULL);
	// this->CheckQueue();
	#endif
	
	return (spTarget->spNode);
};


//-------------------------------------------------
// Get head StartPPN 
//-------------------------------------------------
int CQueue::GetStartPPN_Head() {

	#ifdef DEBUG_BUDDY
		this->CheckQueue();
	#endif

	#ifdef DEBUG_BUDDY
	// Check empty 
	if (this->IsEmpty() == ERESULT_TYPE_YES) {
		printf("Get head StartPPN. FreeList[%d] is empty.\n", this->nOrder);	
		assert (0);
	};
	#endif

	// Get head
	SPLinkedNode spTarget = this->spLinkedNode_head;
	
	#ifdef DEBUG_BUDDY
	assert (spTarget != NULL);
	// this->CheckQueue();
	#endif
	
	return (spTarget->spNode->nStartPPN);
};

//-------------------------------------------------
// Get head StartPPN // DUONGTRAN add
//-------------------------------------------------
int CQueue::GetBlockSize_Head() {

	#ifdef DEBUG_BUDDY
	 this->CheckQueue();
	#endif

	#ifdef DEBUG_BUDDY
	// Check empty 
	if (this->IsEmpty() == ERESULT_TYPE_YES) {
		printf("Get head BlockSize. FreeList[%d] is empty.\n", this->nOrder);	
		assert (0);
	};
	#endif

	// Get head
	SPLinkedNode spTarget = this->spLinkedNode_head;
	
	#ifdef DEBUG_BUDDY
	assert (spTarget != NULL);
	// this->CheckQueue();
	#endif
	
	return (spTarget->spNode->nBlockSize);
}

//-------------------------------------------------
// Get current occupancy
//-------------------------------------------------
int CQueue::GetCurNum() {

	return (this->nCurNum);
};


//-------------------------------------------------
// Get max size
// 	Known when constructed
//-------------------------------------------------
int CQueue::GetMaxNum() {

	return (this->nMaxNum);
};


//-------------------------------------------------
// Get order num
// 	Known when constructed
//-------------------------------------------------
int CQueue::GetOrderNum(){
	
	return (this->nOrder);
};


//-------------------------------------------------
// Check Queue empty
//-------------------------------------------------
EResultType CQueue::IsEmpty() {

	if (this->nCurNum == 0) {

		#ifdef DEBUG_BUDDY
		assert (this->spLinkedNode_head == NULL);
		assert (this->spLinkedNode_tail == NULL);
		#endif

		return (ERESULT_TYPE_YES);	
	};

	#ifdef DEBUG_BUDDY
	assert (this->nCurNum > 0); 
	assert (this->spLinkedNode_head != NULL);
	assert (this->spLinkedNode_tail != NULL);
	#endif

	return (ERESULT_TYPE_NO);
};


//-------------------------------------------------
// Check Queue full 
//-------------------------------------------------
EResultType CQueue::IsFull() {

	if (this->nCurNum == this->nMaxNum) {

		#ifdef DEBUG_BUDDY
		assert (this->spLinkedNode_head != NULL);
		assert (this->spLinkedNode_tail != NULL);
		#endif

		return (ERESULT_TYPE_YES);	
	};

	return (ERESULT_TYPE_NO);
};


//-------------------------------------------------
// Check exist 
//-------------------------------------------------
EResultType CQueue::IsExist(int nStartPPN) {

	SPLinkedNode spScan = this->spLinkedNode_head;

	while (spScan != NULL) {

		if (spScan->spNode->nStartPPN > nStartPPN) {
			return (ERESULT_TYPE_NO);
		};

		if (spScan->spNode->nStartPPN == nStartPPN) {
			return (ERESULT_TYPE_YES);
		};
		spScan = spScan->spNext;
	};

	return (ERESULT_TYPE_NO);
};

//DUONGTRAN add for RangeBuddy
int CQueue::IsExistGetBlockSize_right(int nStartPPN) {
	SPLinkedNode spScan = this->spLinkedNode_head;
	while (spScan != NULL) {
		if (spScan->spNode->nStartPPN > nStartPPN) {
			return 0;
		};
		if (spScan->spNode->nStartPPN == nStartPPN) {
			return spScan->spNode->nBlockSize;
		};
		spScan = spScan->spNext;
	};
	return 0;
};

//DUONGTRAN add for RangeBuddy
int CQueue::IsExistGetBuddyPPN_left(int nStartPPN) {
	SPLinkedNode spScan = this->spLinkedNode_head;
    while (spScan != NULL) {
		if (spScan->spNode->nStartPPN + spScan->spNode->nBlockSize > nStartPPN) {
			return -1;
		};
        if((spScan->spNode->nStartPPN + spScan->spNode->nBlockSize) == nStartPPN) {
            return spScan->spNode->nStartPPN;
        };
        spScan = spScan->spNext;
    };
    return -1;
}

//-------------------------------------------------

//DUONGTRAN add for RangeBuddy
EResultType CQueue::IsExistGetBuddyPPNLeft_BlockSizeRight(int nStartPPN, int nBlockSize, int* PPNLeft, int* BlockSizeRight) {
	SPLinkedNode spScan = this->spLinkedNode_head;
    while (spScan != NULL) {
		if (spScan->spNode->nStartPPN > nStartPPN) {
			*PPNLeft = -1;
			break;
		};
        if((spScan->spNode->nStartPPN + spScan->spNode->nBlockSize) == nStartPPN) {
			*PPNLeft = spScan->spNode->nStartPPN;
			break;
        };
        spScan = spScan->spNext;
    };
	int PPNRight = nStartPPN + nBlockSize;
	while (spScan != NULL) {
		if (spScan->spNode->nStartPPN > PPNRight) {
			*BlockSizeRight = 0;
			return ERESULT_TYPE_FAIL;
		};
		if (spScan->spNode->nStartPPN == PPNRight) {
			*BlockSizeRight = spScan->spNode->nBlockSize;
			return ERESULT_TYPE_SUCCESS;
		};
		spScan = spScan->spNext;
	};
	return (ERESULT_TYPE_SUCCESS);
}

//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CQueue::CheckQueue() {

	// Debug
	assert (this->nCurNum >= 0);
	assert (this->nCurNum <= this->nMaxNum);
	
	// Check NULL 
	if (this->nCurNum == 0) {        
		assert (this->spLinkedNode_head == NULL);
		assert (this->spLinkedNode_tail == NULL);
		// assert (this->spLinkedNode_head->spPrev == NULL);
		// assert (this->spLinkedNode_head->spNext == NULL);
		// assert (this->spLinkedNode_tail->spPrev == NULL);
		// assert (this->spLinkedNode_tail->spNext == NULL);
		assert (this->IsEmpty() == ERESULT_TYPE_YES);
		assert (this->GetCurNum() == 0); 
	} 
	else {
		assert (this->spLinkedNode_head != NULL);
		assert (this->spLinkedNode_tail != NULL);
		// assert (this->spLinkedNode_head->spPrev == NULL);
		// assert (this->spLinkedNode_tail->spNext == NULL);
	};
	
	// Check occupancy 
	int nCurOccupancy = 0;
	SPLinkedNode spScan = this->spLinkedNode_head;
	while (spScan != NULL) {

		spScan = spScan->spNext;
		nCurOccupancy++;
	};
	assert (nCurOccupancy == this->nCurNum);

	// Check full	
	if (nCurOccupancy == this->nMaxNum) {
		assert (this->IsFull() == ERESULT_TYPE_YES);
	} 
	else {
		assert (this->IsFull() == ERESULT_TYPE_NO);
	};
	
	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CQueue::Display() {

//	printf("---------------------------------------------\n");
//	printf("Queue display\n"				);
	printf("    %s display\n", this->cName.c_str());
	printf("---------------------------------------------\n");
//	printf("\t Name		: \t %s\n", this->cName.c_str()	);
	printf("\t nCurNum	: \t %d\n", this->nCurNum	);
	printf("\t nMaxNum	: \t %d\n", this->nMaxNum	);
	printf("\t-------------------------------------\n");
	
	SPLinkedNode spScan = NULL;
	SPLinkedNode spTarget = NULL;
	spScan = this->spLinkedNode_head;
	#if defined RBS || defined PBS
		if(spScan != NULL)
			printf("\t (StartPPN, BlockSize)    : \t");
	#endif
	while (spScan != NULL) {
		spTarget = spScan;
		spScan   = spScan->spNext;
		#if defined RBS || defined PBS
			printf("(0x%x, %d)", spTarget->spNode->nStartPPN, spTarget->spNode->nBlockSize);	  //DUONGTRAN add for RangeBuddy
			if(spScan != NULL)
				printf(" --> ");
		#else // BBS
			printf("\t StartPPN : \t 0x%x\n", spTarget->spNode->nStartPPN);
		#endif // RBS
//		printf("\t-------------------------------------\n");
	};
	printf("\n---------------------------------------------\n");
	
	return (ERESULT_TYPE_SUCCESS);
};

