//-----------------------------------------------------------
// Filename     : CPageArray.cpp 
// Version	: 0.1
// Date         : 29 Sep 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Array definition
//-----------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "CPageArray.h"

//-------------------------------------------------
// Constructor
// 	Array. Size known
//-------------------------------------------------
CPageArray::CPageArray(string cName, int NUM_PAGE) {

	// Genearte
	this->spArray = new SPage* [NUM_PAGE];
	for (int i=0; i<NUM_PAGE; i++) {
		this->spArray[i] = new SPage;
	};

	// Initialize
	for (int i=0; i<NUM_PAGE; i++) { 
		this->spArray[i]->nBlockNum= -1; 
	};

	this->cName   = cName; 
	this->nCurNum = -1;
	this->nMaxNum = NUM_PAGE;
};


//-------------------------------------------------
// Destructor
//-------------------------------------------------
CPageArray::~CPageArray() {

	// Delete all nodes
	for (int i=0; i<this->nMaxNum; i++) { 

		delete (this->spArray[i]); 
		this->spArray[i] = NULL;
	};
};


//-------------------------------------------------
// Initialize
//-------------------------------------------------
EResultType CPageArray::Reset() {

	for (int i=0; i<this->nMaxNum; i++) { 
		this->spArray[i]->nBlockNum= -1; 
	};

	this->nCurNum = -1;
	
	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Set array 
//-------------------------------------------------
EResultType CPageArray::SetArray(int nIndex, int nBlockNum) {

	this->spArray[nIndex]->nBlockNum = nBlockNum;	

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Set array 
//-------------------------------------------------
EResultType CPageArray::SetArray(int nIndex, SPPage spPage) { 

	this->spArray[nIndex] = spPage;	

	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Set number of blocks 
//-------------------------------------------------
EResultType CPageArray::SetMaxNum(int NUM_PAGE) {

	this->nMaxNum = NUM_PAGE;
	
	return (ERESULT_TYPE_SUCCESS);
};


//-------------------------------------------------
// Get block number 
//-------------------------------------------------
int CPageArray::GetBlockNum(int Index) {

	int nBlockNum = this->spArray[Index]->nBlockNum;
	return (nBlockNum);
};


//-------------------------------------------------
// Get current occupancy
//-------------------------------------------------
int CPageArray::GetCurNum() {

	int nOccupancy = 0;
	for (int j=0; j<this->nMaxNum; j++) {
		if (this->spArray[j]->nBlockNum >= 0) {
			nOccupancy++;
		};
	};

	return (nOccupancy);
};


//-------------------------------------------------
// Get number of blocks 
//-------------------------------------------------
 int CPageArray::GetMaxNum() {

	return (this->nMaxNum);
};


//-------------------------------------------------
// Get array
//-------------------------------------------------
SPPage CPageArray::GetNode(int Index) {

	// Debug
	// this->CheckPageArray();

	return (this->spArray[Index]);
};


//-------------------------------------------------
// Check PageArray empty
//-------------------------------------------------
EResultType CPageArray::IsEmpty() {

	for (int j=0; j<this->nMaxNum; j++) {
		if (this->spArray[j]->nBlockNum >= 0) {
			return (ERESULT_TYPE_NO);
		};
	};

	return (ERESULT_TYPE_YES);	
};


//-------------------------------------------------
// Check PageArray full 
//-------------------------------------------------
EResultType CPageArray::IsFull() {

	for (int j=0; j<this->nMaxNum; j++) {
		if (this->spArray[j]->nBlockNum == -1) {
			return (ERESULT_TYPE_NO);
		};
	};

	return (ERESULT_TYPE_YES);	
};


//-------------------------------------------------
// Debug
//-------------------------------------------------
EResultType CPageArray::CheckPageArray() {

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
EResultType CPageArray::Display() {

//	printf("---------------------------------------------\n");
	printf("    PageArray display\n");
	printf("---------------------------------------------\n");
//	printf("\t Name    : \t %s\n", this->cName.c_str());
//	printf("\t nCurNum : \t %d\n", this->nCurNum);
	printf("\t nMaxNum : \t %d\n", this->nMaxNum);
	printf("\t-------------------------------------\n");
	
	for (int j=0; j<this->nMaxNum; j++) {
		printf("\t BlockNum[3%d] : \t %3d\n", j, this->spArray[j]->nBlockNum);
	};
	printf("---------------------------------------------\n");
	
	return (ERESULT_TYPE_SUCCESS);
};

