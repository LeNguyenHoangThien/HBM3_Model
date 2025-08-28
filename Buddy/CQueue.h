//------------------------------------------------------------
// Filename	: CQueue.h 
// Version	: 0.2
// Date		: 11 Nov 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: FreeList[i] Queue type header. "i" is order.
//------------------------------------------------------------
// Note
// 	CFreeList	: Array of "CQueue"
// 	CQueue		: Double-linked-list of "SLinkedNode"
// 	SLinkedNode	: Linked-node of "SNode"
// 	SNode		: Structure of "int"
//------------------------------------------------------------
// Consider PPN for requested VPN. 
// nStartPPN = PPN - (PPN % BlockSize)
//	nOrder = 0. BlockSize = 1. Block = 4 kB. FreeList[0] = 0,1,2,3,4,5,6,7,8,...
//	nOrder = 1. BlockSize = 2. Block = 8 kB. FreeList[1] = 0,  2,  4,  6,  8,...
//	nOrder = 2. BlockSize = 4. Block = 16kB. FreeList[2] = 0,      4,      8,...
//	nOrder = 3. BlockSize = 8. Block = 32kB. FreeList[3] = 0,              8,...
//
//------------------------------------------------------------
#ifndef CQUEUE_H
#define CQUEUE_H

#include <string>

#include "../Top.h"
#include "BuddyTop.h"

using namespace std;

//-----------------------------------------------
// Node in FreeList[i] 
//-----------------------------------------------
typedef struct tagSNode* SPNode;
typedef struct tagSNode{
	int nStartPPN;		// Start-PPN of block
    int nBlockSize; 	// DUONGTRAN add
}SNode;


//-------------------------------
// Linked-node in FreeList[i]
//-------------------------------
typedef struct tagSLinkedNode* SPLinkedNode;
typedef struct tagSLinkedNode{
	SPNode		spNode;
	SPLinkedNode	spPrev;
	SPLinkedNode	spNext;
}SLinkedNode;


//--------------------------------
// Queue class
// 	Linked-list of Node
//--------------------------------
typedef class CQueue* CPQueue;
class CQueue{

public:
        // 1. Contructor and Destructor
	CQueue(string cName, int nOrder);
	~CQueue();

	// 2. Control
	EResultType	RemoveAll();
	EResultType	SetFree_Initial();		// Initialize 
	EResultType	SetFree_Initial(int reqBlockSize, int numBlock);		// DUONGTRAN add

	// Set value
	EResultType	Push(int nStartPPN);		// Generate node inside push
	EResultType	Push(int nStartPPN, int nBlockSize);		// Generate node inside push DUONGTRAN add
	EResultType	PushHead(int nStartPPN, int nBlockSize);		// Generate node inside push DUONGTRAN add
	EResultType	Remove(int nStartPPN);		// Search. Delete node inside
	EResultType	RemoveHead();			// Delete head 

	// Get value
	int		GetCurNum();
	int		GetMaxNum();
	SPNode		GetHead();
	SPNode		GetNode(int nStartPPN);		// Search. Get node.
	int		GetOrderNum();
	int		GetStartPPN_Head();
	int		GetBlockSize_Head();  // DUONGTRAN add

	EResultType	IsEmpty();
	EResultType	IsFull();
	EResultType	IsExist(int nStartPPN);		// Search

	// DUONGTRAN add for RangeBuddy
	int	IsExistGetBlockSize_right(int nStartPPN);		// Search PPN right
	int	IsExistGetBuddyPPN_left(int nStartPPN);		// Search PPN left
	EResultType IsExistGetBuddyPPNLeft_BlockSizeRight(int nStartPPN, int nBlockSize, int* PPNLeft, int* BlockSizeRight); // // Search PPN left and PPN right

	EResultType	Is_Buddy_Exist_Not_Merged();	// Check buddy not merged 
  
	// Debug
	EResultType	CheckQueue();
	EResultType	Display();

private:
	// 3. Original
	string		cName;
	int		nOrder;
	int		nCurNum;
	int		nMaxNum;

	// Control

	// Stat

public: 
	// Linked-node 
	SPLinkedNode	spLinkedNode_head;
	SPLinkedNode	spLinkedNode_tail;
};

#endif

