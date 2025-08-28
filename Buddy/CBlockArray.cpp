//-----------------------------------------------------------
// Filename     : CBlockArray.cpp 
// Version	: 0.2
// Date         : 12 Nov 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Array definition
//-----------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "CBlockArray.h"

//-------------------------------------------------
// Constructor
// 	Array. Size known.
//-------------------------------------------------
CBlockArray::CBlockArray(string cName, int NUM_BLOCK) {

	// Genearte
	this->spArray = new SBlock* [NUM_BLOCK];	// Careful. Need to generate?
	for (int i=0; i<NUM_BLOCK; i++) {
		this->spArray[i] = new SBlock;
	};

	this->cName   = cName; 
	this->nCurNum = -1;
	this->nMaxNum = NUM_BLOCK;
};


//-------------------------------------------------
// Destructor
// 	Array block
//-------------------------------------------------
CBlockArray::~CBlockArray() {

	// Delete all nodes
	for (int i=0; i<this->nMaxNum; i++) { 

		delete (this->spArray[i]); 
		this->spArray[i] = NULL;
	};
};


//-------------------------------------------------
// Initialize reset all
//-------------------------------------------------
EResultType CBlockArray::RemoveAll() {

	for (int i=0; i<this->nMaxNum; i++) { 

		this->spArray[i]->nStartVPN  = -1; 
		this->spArray[i]->nStartPPN  = -1; 
		this->spArray[i]->nBlockSize = -1; 
	};

	this->nCurNum = 0;
	
	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Reset entry 
//-------------------------------------------------
EResultType CBlockArray::Remove(int nIndex) {

	this->spArray[nIndex]->nStartVPN  = -1; 
	this->spArray[nIndex]->nStartPPN  = -1; 
	this->spArray[nIndex]->nBlockSize = -1; 

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Set array 
// 	Generate internally
//-------------------------------------------------
EResultType CBlockArray::SetArray(int nIndex, int nStartVPN, int nStartPPN, int nBlockSize) {

	// Generate node
	SPBlock spBlock_new = new SBlock;		// Careful. Need to generate?
	spBlock_new->nStartVPN  = nStartVPN;
	spBlock_new->nStartPPN  = nStartPPN;
	spBlock_new->nBlockSize = nBlockSize;

	this->spArray[nIndex] = spBlock_new;	

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Set array 
// 	Assign pointer	
// 	Not generate new node.
//-------------------------------------------------
EResultType CBlockArray::SetArray(int nIndex, SPBlock spBlock) { 

	this->spArray[nIndex] = spBlock;	

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Assign pointer-address same as "BlockList" 
// 	(1) Copy "spBlock" pointer address 
//-------------------------------------------------
EResultType CBlockArray::SetPoiner_Same_BlockList(CPBlockList cpBlockList) {

	int nIndex= 0;
	SPLinkedBlock spScan = cpBlockList->spLinkedBlock_head;
	while (spScan != NULL) {
		this->spArray[nIndex] = spScan->spBlock;
		spScan = spScan->spNext;
		nIndex++;
	};

	#ifdef DEBUG_BUDDY
	assert (this->nMaxNum == nIndex);
	#endif

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Check "BlockList" and "BlockArray" same contents
//-------------------------------------------------
EResultType CBlockArray::Check_Identical(CPBlockList cpBlockList) {

	int nIndex= 0;
	SPLinkedBlock spScan = cpBlockList->spLinkedBlock_head;
	while (spScan != NULL) {

		// Check "spBlock" pointer address same
		if (this->spArray[nIndex] != spScan->spBlock) {
			return (ERESULT_TYPE_NO);
		};

		// Check contents same 
		if (this->spArray[nIndex]->nStartVPN  != spScan->spBlock->nStartVPN)  { return (ERESULT_TYPE_NO); };
		if (this->spArray[nIndex]->nStartPPN  != spScan->spBlock->nStartPPN)  { return (ERESULT_TYPE_NO); };
		if (this->spArray[nIndex]->nBlockSize != spScan->spBlock->nBlockSize) { return (ERESULT_TYPE_NO); };

		spScan = spScan->spNext;
		nIndex++;
	};

	#ifdef DEBUG_BUDDY
	assert (this->nMaxNum == nIndex);
	#endif

	return (ERESULT_TYPE_YES);
};


//-------------------------------------------------
// Set number of blocks 
//-------------------------------------------------
EResultType CBlockArray::SetMaxNum(int NUM_BLOCK) {

	this->nMaxNum = NUM_BLOCK;
	
	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Get current occupancy
//-------------------------------------------------
int CBlockArray::GetCurNum() {

	int nOccupancy = 0;
	for (int j=0; j<this->nMaxNum; j++) {
		if (this->spArray[j]->nBlockSize >= 0) {
			nOccupancy++;
		};
	};

	return (nOccupancy);
};


//-------------------------------------------------
// Get number of blocks 
//-------------------------------------------------
 int CBlockArray::GetMaxNum() {

	return (this->nMaxNum);
};


//-------------------------------------------------
// Get block 
//-------------------------------------------------
SPBlock CBlockArray::GetBlock(int Index) {

	// Debug
	// this->CheckBlockArray();

	return (this->spArray[Index]);
};


//-------------------------------------------------
// Get StartVPN 
//-------------------------------------------------
int CBlockArray::GetStartVPN(int Index) {

	// Debug
	// this->CheckBlockArray();

	return (this->spArray[Index]->nStartVPN);
};


//-------------------------------------------------
// Get StartPPN 
//-------------------------------------------------
int CBlockArray::GetStartPPN(int Index) {

	// Debug
	// this->CheckBlockArray();

	return (this->spArray[Index]->nStartPPN);
};


//-------------------------------------------------
// Get BlockSize 
//-------------------------------------------------
int CBlockArray::GetBlockSize(int Index) {

	// Debug
	// this->CheckBlockArray();

	return (this->spArray[Index]->nBlockSize);
};


//-------------------------------------------------
// Check BlockArray empty
//-------------------------------------------------
EResultType CBlockArray::IsEmpty() {

	for (int j=0; j<this->nMaxNum; j++) {
		if (this->spArray[j]->nBlockSize >= 0) {
			return (ERESULT_TYPE_NO);
		};
	};

	return (ERESULT_TYPE_YES);	
};


//-------------------------------------------------
// Check BlockArray full 
//-------------------------------------------------
EResultType CBlockArray::IsFull() {

	for (int j=0; j<this->nMaxNum; j++) {
		if (this->spArray[j]->nBlockSize == -1) {
			return (ERESULT_TYPE_NO);
		};
	};

	return (ERESULT_TYPE_YES);	
};


//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CBlockArray::CheckBlockArray() {

	// Debug
	assert (this->nCurNum >= 0);
//	assert (this->nCurNum <= this->nMaxNum);
	
	// Check NULL 
	if (this->nCurNum == 0) {        
		assert (this->IsEmpty() == ERESULT_TYPE_YES);
		assert (this->GetCurNum() == 0); 
	} 
	else {
		assert (this->IsEmpty() == ERESULT_TYPE_NO);
		assert (this->GetCurNum() > 0); 
	};
	
	// Check occupancy 

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CBlockArray::Display() {

//	printf("---------------------------------------------\n");
	printf("    BlockArray display\n");
	printf("---------------------------------------------\n");
//	printf("\t Name    : \t %s\n", this->cName.c_str());
//	printf("\t nCurNum : \t %d\n", this->nCurNum);
	printf("\t nMaxNum : \t %d\n", this->nMaxNum);
	printf("\t-------------------------------------\n");
	
	for (int j=0; j<this->nMaxNum; j++) {
		printf("\t StartVPN 0x%x, StartPPN 0x%x, BlockSize 0x%x\n", this->spArray[j]->nStartVPN, this->spArray[j]->nStartPPN, this->spArray[j]->nBlockSize);
//		printf("\t StartVPN     : \t 0x%x\n", this->spArray[j]->nStartVPN);
//		printf("\t StartPPN     : \t 0x%x\n", this->spArray[j]->nStartPPN);
//		printf("\t BlockSize    : \t 0x%x\n", this->spArray[j]->nBlockSize);
	};
	printf("---------------------------------------------\n");
	
	return (ERESULT_TYPE_SUCCESS);
};

