//------------------------------------------------------------
// Filename	: CFreeList.h 
// Version	: 0.2
// Date		: 9 Nov 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: FreeList type header
//------------------------------------------------------------
// Note
//	CFreeList	: Array of "CQueue"
//	CQueue		: Double-linked-list of "SLinkedNode"
//	SLinkedNode	: Linked-node of "SNode"
//	SNode		: Structure of "int"
//------------------------------------------------------------
#ifndef CFREELIST_H
#define CFREELIST_H

#include <string>

#include "../Top.h"
#include "CQueue.h"

using namespace std;

//--------------------------------
// FreeList 
// 	CAM 	
//--------------------------------
typedef class CFreeList* CPFreeList;
class CFreeList{

public:
        // 1. Contructor and Destructor
	CFreeList(string cName);
	CFreeList(string cName, uint64_t nStartPPN);
	~CFreeList();

	// 2. Control
	EResultType	RemoveAll();
	EResultType	SetFree_Initial();
	EResultType SetFree_Initial(bool memmap[], char typeBuddy);		// DUONGTRAN add
	EResultType SetFree_Initial(bool memmap[], char typeBuddy, int64_t startPPN);		// DUONGTRAN add
	EResultType	SetFree_Initial(int reqBlockSize, int numBlock);   // DUONGTRAN add

	// Set value
	EResultType	Push(int nOrder, int nStartPPN);
	EResultType	Remove(int nOrder, int nStartPPN);

	EResultType	Merge_Buddy(int nOrder, int nStartPPN);		// Merge recursive ascending to MAX_ORDER-1
	EResultType	Merge_RangeBuddy(int nStartPPN, int nNumRequestedPages);		// Merge recursive ascending to MAX_ORDER-1   			DUONGTRAN add
	EResultType	Merge_PageBuddy(int nOrder, int nStartPPN, int nNumRequestedPages);		// Merge recursive ascending to MAX_ORDER-1   			DUONGTRAN add
	EResultType	Split(int nOrder, int nStartPPN);		// Split to one descending order
	EResultType	SplitRange(int nOrder, int nStartPPN, int nNumRequestedPages);		// Split to one descending order     DUONGTRAN add
	EResultType	SplitPage(int nOrder, int expand, int nNumRequestedPages);		// Split to one descending order     DUONGTRAN add

	EResultType	Expand(int nOrder);				// Expand recursive ascending to MAX_ORDER-1
	EResultType	Expand(int nOrder, int nNumRequestedPages);				// Expand recursive ascending to MAX_ORDER-1  DUONGTRAN add


	// Get value
	int		GetStartPPN_Head(int nOrder);
	int		GetBlockSize_Head(int nOrder);

	int		GetTargetOrder(int NumRequestedPages);		// Starting order search for alloc

	int		GetNumFreePages();				// Total number of free (4kB) pages

	int		GetCurNum(int nOrder);
	int		GetCurNum();                    //DUONGTRAN add to count the number of nodes
	CPQueue		GetQueue(int nOrder);

	EResultType	Is_Buddy_Exist_Not_Merged();			// Check buddy not merged 

	EResultType	IsEmpty(int nOrder);
	EResultType	IsFull(int nOrder);
	EResultType	IsExist(int nOrder, int nStartPPN);

	// Stat
  
	// Debug
	EResultType	CheckFreeList();
	EResultType	Display();

private:
	// Original
	string		cName;

	// Control

	// Stat

public: 

	// Node 
	CPQueue*	cpFreeList;					// [MAX_ORDER]
	int nNumFreePage;		// DUONGTRAN add
	uint64_t nStartPPN;
};

#endif

