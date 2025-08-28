//------------------------------------------------------------
// Filename	: CBlockArray.h 
// Version	: 0.2
// Date		: 11 Nov 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Class type header
//------------------------------------------------------------
// Summary
// 	(1) Used in CAMB
// 	(2) This contains same info as "BlockList"
// 	    It takes time to access "BlockList" because it is linked-list.
// 	    When all allocations finisehd, number of blocks is known. 
// 	    Then copy "BlockList" pointer address into "BlockArray".
// 	    It takes less time to access "BlockArray" because it is array.
//	(3) In CAMB, when all allocations finisehd, this array is used to set PTEs.
//------------------------------------------------------------
// BlockList
// 	(1) CBlockList		: Linked-list of "SLinkedBlock"
// 	(2) SLinkedBlock	: Linked-node of "SBlock"
// 	(3) SBlock		: Structure ("BlockList" node)
//
// BlockArray
// 	(1) CBlockArray		: "Array" of "SBlock"
//------------------------------------------------------------
// Maintenance
//	(1) Construct entire array when all blocks allocated
//	(2) When all blocks are allocated, store in spArray.
//-----------------------------------------------------------

#ifndef CBLOCKARRAY_H
#define CBLOCKARRAY_H

#include <string>

#include "../Top.h"
#include "BuddyTop.h"
#include "CBlockList.h"

//-----------------------------------------------
// Same node as "BlockList" (BuddyTop.h)
//-----------------------------------------------
// typedef struct tagSBlock* SPBlock;
// typedef struct tagSBlock{
//	int nStartVPN;
//	int nStartPPN;
//	int nBlockSize;         // Num. pages
// }SBlock;


using namespace std;

//--------------------------------
// FIFO class
// 	Array of "Block"
//--------------------------------
typedef class CBlockArray* CPBlockArray;
class CBlockArray{

public:
        // 1. Contructor and Destructor
	CBlockArray(string cName, int nNumBlock);					// Array 
	~CBlockArray();	

	// 2. Control
	EResultType	RemoveAll();
	EResultType	Remove(int Index);

	// Set value
	EResultType	SetMaxNum(int nMaxNum);

	EResultType	SetArray(int Index, int StartVPN, int StatPPN, int BlockSize);	// Set array 
	EResultType	SetArray(int Index, SPBlock spBlock);

	EResultType	SetPoiner_Same_BlockList(CPBlockList cpBlockList);		// Assign same pointer

	// Debug
	EResultType	Check_Identical(CPBlockList cpBlockList);			// Check all contents same 


	// Get value
	int		GetCurNum();
	int		GetMaxNum();

	int		GetStartVPN(int Index);
	int		GetStartPPN(int Index);
	int		GetBlockSize(int Index);

	EResultType	IsEmpty();
	EResultType	IsFull();

	SPBlock		GetBlock(int Index);

	// Stat
  
	// Debug
	EResultType	CheckBlockArray();
	EResultType	Display();

private:
	// Original
	string		cName;
	int		nCurNum;
	int		nMaxNum;							// Number of blocks

	// Control

	// Stat

public: 

	SPBlock*	spArray;							// [nMaxNum]	
};

#endif

