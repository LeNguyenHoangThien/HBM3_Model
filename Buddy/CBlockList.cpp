//-----------------------------------------------------------
// Filename     : CBlockList.cpp 
// Version	: 0.1
// Date         : 29 Sep 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: FIFO linked-list definition
//-----------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "CBlockList.h"

//-------------------------------------------------
// Constructor
// 	Size unknown. Initially empty.
//-------------------------------------------------
CBlockList::CBlockList(string cName) {

	// Generate a node when push conducted
	this->spLinkedBlock_head = NULL;	
	this->spLinkedBlock_tail = NULL;	

	// Initialize	
	this->cName   = cName;
	this->nCurNum = 0;
//	this->nMaxNum = -1;
};


//-------------------------------------------------
// Destructor
//-------------------------------------------------
CBlockList::~CBlockList() {

	// Delete all nodes
	SPLinkedBlock spScan = NULL;
	SPLinkedBlock spTarget = NULL;
	spScan = this->spLinkedBlock_head;
	while (spScan != NULL) {
		spTarget = spScan;
		spScan   = spScan->spNext;
		delete (spTarget->spBlock);
		spTarget->spBlock = NULL;
		spTarget = NULL;
		// spTarget->spNext = NULL;
	};

	// Debug
	// assert (this->spLinkedBlock_head == NULL);
	// assert (this->spLinkedBlock_tail == NULL);
};


//-------------------------------------------------
// Initialize
//-------------------------------------------------
EResultType CBlockList::RemoveAll() {

	// Delete all nodes 
	SPLinkedBlock spScan = NULL;
	SPLinkedBlock spTarget = NULL;
	spScan = this->spLinkedBlock_head;
	while (spScan != NULL) {
		spTarget = spScan;
		spScan   = spScan->spNext;
		delete (spTarget->spBlock);
		spTarget->spBlock = NULL;
		spTarget = NULL;
	};
	
	// Initialize 
	this->spLinkedBlock_head = NULL;
	this->spLinkedBlock_tail = NULL;
	this->nCurNum   = 0;
	
	return (ERESULT_TYPE_SUCCESS);
};


EResultType CBlockList::Append(CPBlockList list){
	if(this->spLinkedBlock_head == NULL){
		this->spLinkedBlock_head = list->spLinkedBlock_head;
	} else {
		this->spLinkedBlock_tail->spNext = list->spLinkedBlock_head;
	}
	this->spLinkedBlock_tail = list->spLinkedBlock_tail;

	this->nCurNum += list->GetCurNum();

	list->SetCurNum(0);
	list->spLinkedBlock_head = NULL;
	list->spLinkedBlock_tail = NULL;

	return (ERESULT_TYPE_SUCCESS);
}

EResultType CBlockList::SetCurNum(int nCurNum){
	this->nCurNum = nCurNum;
	return (ERESULT_TYPE_SUCCESS);
}

//-------------------------------------------------
// Push
//-------------------------------------------------
EResultType CBlockList::Push(SPBlock spNew) {		// spNew generated outside

	#ifdef DEBUG_BUDDY
	assert (spNew!= NULL);
//	assert (this->GetCurNum() < this->GetMaxNum());	
	#endif

	// Generate node and initialize
//	SPBlock spNew = Copy_UD(spNew); 			// Generate new sp 
	SPLinkedBlock spLinkedBlock_new = new SLinkedBlock;
	spLinkedBlock_new->spBlock = spNew;
	spLinkedBlock_new->spNext = NULL;

	// Push
	if (this->spLinkedBlock_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->spLinkedBlock_tail == NULL);
		#endif

		this->spLinkedBlock_head = spLinkedBlock_new;
		this->spLinkedBlock_tail = spLinkedBlock_new;
	} 
	else {

		#ifdef DEBUG_BUDDY
		assert (this->GetCurNum() > 0);	
		assert (this->spLinkedBlock_tail != NULL);
		#endif

		this->spLinkedBlock_tail->spNext = spLinkedBlock_new;
		this->spLinkedBlock_tail = spLinkedBlock_new;
	};

	// Increment occupancy
	this->nCurNum++;

	#ifdef DEBUG_BUDDY
	assert (spNew != NULL);
	// this->CheckBlockList();
	#endif

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Push
//-------------------------------------------------
EResultType CBlockList::Push(int nStartVPN, int nStartPPN, int nBlockSize) {

	// Generate node
	SPBlock spBlock_new = new SBlock;
	spBlock_new->nStartVPN  = nStartVPN;
	spBlock_new->nStartPPN  = nStartPPN;
	spBlock_new->nBlockSize = nBlockSize;

	#ifdef DEBUG_BUDDY
	assert (spBlock_new != NULL);
	#endif

	// Generate linked-node. Initialize
//	SPBlock spNew = Copy_UD(spNew); 			// Generate new sp 
	SPLinkedBlock spLinkedBlock_new = new SLinkedBlock;
	spLinkedBlock_new->spBlock = spBlock_new;
	spLinkedBlock_new->spNext = NULL;

	// Push
	if (this->spLinkedBlock_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->spLinkedBlock_tail == NULL);
		#endif

		this->spLinkedBlock_head = spLinkedBlock_new;
		this->spLinkedBlock_tail = spLinkedBlock_new;
	} 
	else {

		#ifdef DEBUG_BUDDY
		assert (this->GetCurNum() > 0);	
		assert (this->spLinkedBlock_tail != NULL);
		#endif

		this->spLinkedBlock_tail->spNext = spLinkedBlock_new;
		this->spLinkedBlock_tail = spLinkedBlock_new;
	};

	// Increment occupancy
	this->nCurNum++;

	#ifdef DEBUG_BUDDY
	assert (spBlock_new != NULL);
	// this->CheckBlockList();
	#endif

	return (ERESULT_TYPE_SUCCESS);
};

//DUONGTRAN add
EResultType CBlockList::Push(int nOriginalVPN, int nStartVPN, int nStartPPN, int nBlockSize) {

	// Generate node
	SPBlock spBlock_new = new SBlock;
	spBlock_new->nOriginalVPN = nOriginalVPN; 
	spBlock_new->nStartVPN  = nStartVPN;
	spBlock_new->nStartPPN  = nStartPPN;
	spBlock_new->nBlockSize = nBlockSize;

	#ifdef DEBUG_BUDDY
	assert (spBlock_new != NULL);
	#endif

	// Generate linked-node. Initialize
//	SPBlock spNew = Copy_UD(spNew); 			// Generate new sp 
	SPLinkedBlock spLinkedBlock_new = new SLinkedBlock;
	spLinkedBlock_new->spBlock = spBlock_new;
	spLinkedBlock_new->spNext = NULL;

	// Push
	if (this->spLinkedBlock_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->spLinkedBlock_tail == NULL);
		#endif

		this->spLinkedBlock_head = spLinkedBlock_new;
		this->spLinkedBlock_tail = spLinkedBlock_new;
	} 
	else {

		#ifdef DEBUG_BUDDY
		assert (this->GetCurNum() > 0);	
		assert (this->spLinkedBlock_tail != NULL);
		#endif

		this->spLinkedBlock_tail->spNext = spLinkedBlock_new;
		this->spLinkedBlock_tail = spLinkedBlock_new;
	};

	// Increment occupancy
	this->nCurNum++;

	#ifdef DEBUG_BUDDY
	assert (spBlock_new != NULL);
	// this->CheckBlockList();
	#endif

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Pop
//-------------------------------------------------
EResultType CBlockList::RemoveHead() {

	// Debug
	// this->CheckBlockList();

	// Get head
	SPLinkedBlock spTarget = this->spLinkedBlock_head;

	// Decrement occupancy
	this->nCurNum--;

	// Pop head
	if (this->nCurNum == 0) {
		this->spLinkedBlock_head = NULL;
		this->spLinkedBlock_tail = NULL;
	} 
	else {
		this->spLinkedBlock_head = this->spLinkedBlock_head->spNext;
	};

	#ifdef DEBUG_BUDDY
	assert (spTarget != NULL);
	// this->CheckBlockList();
	#endif

	// Maintain
	delete (spTarget->spBlock);
	spTarget->spBlock = NULL;
	delete (spTarget);
	spTarget = NULL;

	return (ERESULT_TYPE_SUCCESS);
};

//DUONGTRAN add
SPLinkedBlock CBlockList::Remove(int nStartPPN) {
	// Get head
	if(this->spLinkedBlock_head == NULL){
		printf("Not found nStartPPN %d in BlockList!!!\n", nStartPPN);
		assert(0);
		return NULL;
	}

	SPLinkedBlock spTarget = this->spLinkedBlock_head;
	SPLinkedBlock spPreTarget = NULL;

	while(spTarget != NULL){
		if(spTarget->spBlock->nStartPPN == nStartPPN){
			break;
		}
		spPreTarget = spTarget;
		spTarget = spTarget->spNext;		
	}

	if(spTarget == NULL) { // not found
		printf("Not found nStartPPN %d in BlockList!!!\n", nStartPPN);
		assert(0);
		return NULL;
	}

	if(spTarget == this->spLinkedBlock_head){  // head node
		this->spLinkedBlock_head = this->spLinkedBlock_head->spNext;
	} else if(spTarget == this->spLinkedBlock_tail){ // tail node
		this->spLinkedBlock_tail = spPreTarget;
		this->spLinkedBlock_tail->spNext = NULL;
	} else { // mid node
		spPreTarget->spNext = spTarget->spNext;
	}

	this->nCurNum--;
	
	return spTarget->spNext; 

};


//-------------------------------------------------
// Set number of blocks 
//-------------------------------------------------
// EResultType CBlockList::SetMaxNum(int nMaxNum) {
//
//	this->nMaxNum = nMaxNum;
//	return (ERESULT_TYPE_SUCCESS);
// };


//-------------------------------------------------
// Get current occupancy
//-------------------------------------------------
int CBlockList::GetCurNum() {

	return (this->nCurNum);
};


//-------------------------------------------------
// Get number of blocks 
//-------------------------------------------------
// int CBlockList::GetMaxNum() {
//
//	return (this->nMaxNum);
// };


//-------------------------------------------------
// Get head spBlock 
//-------------------------------------------------
SPBlock CBlockList::GetHead() {

	// Debug
	// this->CheckBlockList();

	if (this->spLinkedBlock_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->GetCurNum() == 0);
		#endif

		return (NULL);
	};

	return (this->spLinkedBlock_head->spBlock);
};


//-------------------------------------------------
// Get StartVPN head
//-------------------------------------------------
int CBlockList::GetStartVPN_Head() {

	// Debug
	// this->CheckBlockList();

	if (this->spLinkedBlock_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->GetCurNum() == 0);
		#endif

		return (-1);
	};

	return (this->spLinkedBlock_head->spBlock->nStartVPN);
};


//-------------------------------------------------
// Get StartPPN head
//-------------------------------------------------
int CBlockList::GetStartPPN_Head() {

	// Debug
	// this->CheckBlockList();

	if (this->spLinkedBlock_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->GetCurNum() == 0);
		#endif

		return (-1);
	};

	return (this->spLinkedBlock_head->spBlock->nStartPPN);
};


//-------------------------------------------------
// Get BlockSize head
//-------------------------------------------------
int CBlockList::GetBlockSize_Head() {

	// Debug
	// this->CheckBlockList();

	if (this->spLinkedBlock_head == NULL) {

		#ifdef DEBUG_BUDDY
		assert (this->GetCurNum() == 0);
		#endif

		return (-1);
	};

	return (this->spLinkedBlock_head->spBlock->nBlockSize);
};


//-------------------------------------------------
// Check BlockList empty
//-------------------------------------------------
EResultType CBlockList::IsEmpty() {

	if (this->nCurNum == 0) {

		#ifdef DEBUG_BUDDY
		assert (this->spLinkedBlock_head == NULL);
		assert (this->spLinkedBlock_tail == NULL);
		#endif

		return (ERESULT_TYPE_YES);	
	};

	#ifdef DEBUG_BUDDY
	assert (this->spLinkedBlock_head != NULL);
	assert (this->spLinkedBlock_tail != NULL);
	#endif

	return (ERESULT_TYPE_NO);
};


//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CBlockList::CheckBlockList() {

	// Debug
	assert (this->nCurNum >= 0);
	
	// Check NULL 
	if (this->nCurNum == 0) {        
		assert (this->spLinkedBlock_head == NULL);
		assert (this->spLinkedBlock_tail == NULL);
		assert (this->IsEmpty() == ERESULT_TYPE_YES);
		assert (this->GetCurNum() == 0); 
	} 
	else {
		assert (this->spLinkedBlock_head != NULL);
		assert (this->spLinkedBlock_tail != NULL);
		assert (this->IsEmpty() == ERESULT_TYPE_NO);
		assert (this->GetCurNum() > 0); 
	};
	
	// Check occupancy 
	int nCurOccupancy = 0;
	SPLinkedBlock spScan = this->spLinkedBlock_head;
	while (spScan != NULL) {
		spScan = spScan->spNext;
		nCurOccupancy++;
	};
	assert (nCurOccupancy == this->nCurNum);

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CBlockList::Display() {

//	printf("---------------------------------------------\n");
	printf("    BlockList display\n");
	printf("---------------------------------------------\n");
//	printf("\t Name    : \t %s\n", this->cName.c_str());
	printf("\t nCurNum : \t %d\n", this->nCurNum);
//	printf("\t nMaxNum : \t %d\n", this->nMaxNum);
	printf("\t-------------------------------------\n");
	
	SPLinkedBlock spScan = NULL;
	SPLinkedBlock spTarget = NULL;
	spScan = this->spLinkedBlock_head;
	while (spScan != NULL) {
		spTarget = spScan;
		spScan   = spScan->spNext;
		printf("\t StartVPN     : \t 0x%x\n", spTarget->spBlock->nStartVPN);
		printf("\t StartPPN     : \t 0x%x\n", spTarget->spBlock->nStartPPN);
		printf("\t BlockSize    : \t %d\n", spTarget->spBlock->nBlockSize);
		printf("---------------------------------------------\n");
	};
	printf("---------------------------------------------\n");
	
	return (ERESULT_TYPE_SUCCESS);
};

