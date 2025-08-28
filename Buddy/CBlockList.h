//------------------------------------------------------------
// Filename	: CBlockList.h 
// Version	: 0.1
// Date		: 29 Sep 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: FIFO type header
//------------------------------------------------------------
// Summary
//	(1) When a block allocated, it is necessary to store block-mapping info to de-allocate the block later.	
//------------------------------------------------------------
// BlockList
// 	(1) CBlockList		: Linked-list of "SLinkedBlock"
// 	(2) SLinkedBlock	: Linked-node of "SBlock"
// 	(3) SBlock		: Structure ("BlockList" node)
//------------------------------------------------------------
// Maintenance
//	(1) When application invoked, "BlockList" size unknown.
//	    When constructed, list empty. 
//	(2) Push whenever a block allcoated.
//	    Add to linked-list FIFO.
//	(3) Pop whenever a block de-allocated.
//	    Remove node in FIFO.
//	(4) Null when all blocks de-allocated.
//-----------------------------------------------------------
// To do
// 	(1) Should be queue. Support out-of-order
//-----------------------------------------------------------
#ifndef CBLOCKLIST_H
#define CBLOCKLIST_H

#include <string>

#include "../Top.h"
#include "BuddyTop.h"

//-----------------------------------------------
// Node in "BlockList" (BuddyTop.h)
//-----------------------------------------------
// typedef struct tagSBlock* SPBlock;
// typedef struct tagSBlock{
//	int nStartVPN;
//	int nStartPPN;
//	int nBlockSize;         // Num. pages
// }SBlock;


//-------------------------------
// Linked-node in "BlockList" (BuddyTop.h)
//-------------------------------
// typedef struct tagSLinkedBlock* SPLinkedBlock;
// typedef struct tagSLinkedBlock{
//	SPBlock		spBlock;
//	SPLinkedBlock	spNext;
// }SLinkedBlock;


using namespace std;

//--------------------------------
// FIFO class
// 	Linked-list of "Block"
//--------------------------------
typedef class CBlockList* CPBlockList;
class CBlockList{

public:
        // 1. Contructor and Destructor
	CBlockList(string cName);						// List
	~CBlockList();

	// 2. Control
	EResultType	RemoveAll();

	// Set value
	EResultType	Push(SPBlock spNew);					// Generate node outside push	
	EResultType	Push(int nStartVPN, int nStatPPN, int nBlockSize);	// Generate node insde   push
	EResultType	Push(int nOrigitalVPN, int nStartVPN, int nStatPPN, int nBlockSize);	// DUONGTRAN add
	EResultType	RemoveHead();						// Delete head 
	SPLinkedBlock	Remove(int nStartPPN);						// DUONGTRAN add
	EResultType	Append(CPBlockList list);						// DUONGTRAN add
	EResultType SetCurNum(int nCurNum);							// DUONGTRAN add

//	EResultType	SetMaxNum(int nMaxNum);

	// Get value
	int		GetCurNum();
//	int		GetMaxNum();
	SPBlock		GetHead();
//	SPBlock		GetNode(int nStartPPN);
	int		GetStartVPN_Head();
	int		GetStartPPN_Head();
	int		GetBlockSize_Head();
	EResultType	IsEmpty();

	// Stat
  
	// Debug
	EResultType	CheckBlockList();
	EResultType	Display();

private:
	// Original
	string		cName;
	int		nCurNum;
//	int		nMaxNum;						// Number of blocks

	// Control

	// Stat

public: 
	// Node 
	SPLinkedBlock	spLinkedBlock_head;
	SPLinkedBlock	spLinkedBlock_tail;
};

#endif

