//-------------------------------------------------------------
// Filename	: BuddyTop.h
// Version	: 0.3
// Date		: 19 Apr 2020
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Global top header
//-------------------------------------------------------------
// FreeList
//	CFreeList	: Array of "CQueue"
//	CQueue		: Double linked-list of "SLinkedNode"
//	SLinkedNode	: Linked-node of "SNode"
//	SNode		: Structure of "int" (FreeList node)
//
// BlockList		: Block-mapping info
//	CBlockList	: Linked-list of "SLinkedBlock"
//	SLinkedBlock	: Linked-node of "SBlock"
//	SBlock		: Structure ("BlockList" node)
//
// BlockArray		: CAMB. Block-mapping info. Same info "BlockList". Array is fast
//	CBlockArray	: Array of "SBlock"
//
// PageArray		: CAMB
//	CPageArray	: Array of "SPage"
//------------------------------------------------------------
// Terminology 
//	BlockSize	: Number of (contiguous) pages in a block 
//	GroupSize	: Number of PTEs in a group
//			  Number of PTEs that a page-table walk acquires 
//-------------------------------------------------------------
#ifndef BUDDYTOP_H
#define BUDDYTOP_H

#include <string>
#include <math.h>
#include <stdint.h>

//-------------------------------------------------------------
// Memory size (power-of-2)
// 	Total number of (4kB-sized) pages in memory
//-------------------------------------------------------------
// #define NUM_TOTAL_PAGES		8		//  32-kB (= 2^3  x 4kB)
// #define NUM_TOTAL_PAGES		16		//  64-kB (= 2^4  x 4kB)
// #define NUM_TOTAL_PAGES		32		// 128-kB (= 2^5  x 4kB)
// #define NUM_TOTAL_PAGES		64		// 256-kB (= 2^6  x 4kB)	
// #define NUM_TOTAL_PAGES		128		// 512-kB (= 2^7  x 4kB)	
// #define NUM_TOTAL_PAGES		256		//   1-MB (= 2^8  x 4kB)	
// #define NUM_TOTAL_PAGES		512		//   2-MB (= 2^9  x 4kB)	
// #define NUM_TOTAL_PAGES		1024		//   4-MB (= 2^10 x 4kB)	
// #define NUM_TOTAL_PAGES		2048		//   8-MB (= 2^11 x 4kB)
// #define NUM_TOTAL_PAGES		4096		//  16-MB (= 2^12 x 4kB)	
// #define NUM_TOTAL_PAGES		8192		//  32-MB (= 2^13 x 4kB)	
// #define NUM_TOTAL_PAGES		16384		//  64-MB (= 2^14 x 4kB)	
// #define NUM_TOTAL_PAGES		32768		// 128-MB (= 2^15 x 4kB)
// #define NUM_TOTAL_PAGES		65536		// 256-MB (= 2^16 x 4kB)
// #define NUM_TOTAL_PAGES		262144		//   1-GB (= 2^18 x 4kB)
// #define NUM_TOTAL_PAGES		524288		//   2-GB (= 2^19 x 4kB)
// #define NUM_TOTAL_PAGES		1048576		//   4-GB (= 2^20 x 4kB)


//-------------------------------------------------------------
// Application size 
// 	Number of requested pages	
//-------------------------------------------------------------
// #define NUM_REQUESTED_PAGES		7
// #define NUM_REQUESTED_PAGES		338		//  720 x  480. RGB.  
// #define NUM_REQUESTED_PAGES		900		// 1280 x  720. RGB.
// #define NUM_REQUESTED_PAGES		8640		// 4096 x 2160. RGB.  


//-------------------------------------------------------------
// MAX_ORDER
// 	MAX_ORDER (1,2,...) is number of free-lists
// 	Order is 0,1,2,...
// 	A block size = 2^(Order) pages. 
//-------------------------------------------------------------
// #define MAX_ORDER		1	// Max   4kB.  Max   1 page.	Max FreeList[0]
// #define MAX_ORDER		2	// Max   8kB.  Max   2 pages.	Max FreeList[1] 
// #define MAX_ORDER		3	// Max  16kB.  Max   4 pages.	Max FreeList[2] 
// #define MAX_ORDER		4	// Max  32kB.  Max   8 pages.	Max FreeList[3]
// #define MAX_ORDER		5	// Max  64kB.  Max  16 pages.	Max FreeList[4]
// #define MAX_ORDER		6	// Max 128kB.  Max  32 pages. 	Max FreeList[5] 
// #define MAX_ORDER		7	// Max 256kB.  Max  64 pages. 	Max FreeList[6] 
// #define MAX_ORDER		8	// Max 512kB.  Max 128 pages. 	Max FreeList[7] 
// #define MAX_ORDER		9	// Max   1MB.  Max 256 pages. 	Max FreeList[8] 
// #define MAX_ORDER		10	// Max   2MB.  Max 512 pages. 	Max FreeList[9] 
// #define MAX_ORDER		11	// Max   4MB.  Max 1024 pages. 	Max FreeList[10]   //DUONGTRAN add
// #define MAX_ORDER		12	// Max   8MB.  Max 2048 pages. 	Max FreeList[11]   //DUONGTRAN add
// #define MAX_ORDER		13	// Max   16MB.  Max 4096 pages. 	Max FreeList[12]   //DUONGTRAN add


//-------------------------------------------------------------
// Allocation algorithm 
// 	DEFAULT, BCT, CAMB
//-------------------------------------------------------------
// #define DEFAULT 
// #define CAMB


//-------------------------------------------------------------
// Debug mode enable (defined Top.h)
//-------------------------------------------------------------
// #define DEBUG_BUDDY

//-------------------------------------------------------------
// Initialize freelist 
//-------------------------------------------------------------
// #define SET_MAXORDER_MINUS_1_FREE
// #define SET_MAXORDER_MINUS_2_FREE
// #define SET_MAXORDER_MINUS_3_FREE
// #define SET_MAXORDER_MINUS_4_FREE
// #define SET_ORDER_0_ONLY_FREE


//-------------------------------------------------------------
// Application start address 
// 	start VPN 
//-------------------------------------------------------------
// #define START_VPN			0


//-------------------------------------------------------------
// Number of PTEs in page table	
// 	Assume Same as NUM_TOTAL_PAGES
//-------------------------------------------------------------
// #define PAGE_TABLE_SIZE	8	// FIXME


//-------------------------------------------------------------
// Group size PTEs (CAMB)
// 	Number of PTEs (power-of-2)
//-------------------------------------------------------------
//#define GROUP_SIZE		4	// 4 PTEs DUONGTRAN comment, move to ../Top.h


using namespace std;

//-------------------------------------------------------------
// Result
//	(1) Function result yes/no
//	(2) Function result success/fail
// Note
// 	FIXME Same name as SIM
//-------------------------------------------------------------
// typedef enum{
//	ERESULT_TYPE_YES,		// Function result YES
//	ERESULT_TYPE_NO,		// Function result NO
//	
//	ERESULT_TYPE_SUCCESS,		// Function success
//	ERESULT_TYPE_FAIL,		// Function fails
//	
//	ERESULT_TYPE_UNDEFINED
// }EResultType;


//-------------------------------------------------------------
// Allocation algorithm 
//-------------------------------------------------------------
// typedef enum{
//	EALGORITHM_TYPE_DEFAULT,
//	EALGORITHM_TYPE_BCT,		// Block Contiguity Translation. IEICE19 paper	
//	EALGORITHM_TYPE_CAMB,		// Packed Allocated Memory Blocks 
//	EALGORITHM_TYPE_UNDEFINED
// }EAlgorithmType;


//-----------------------------------------------
// Node in "BlockList"
// 	Block-mapping information
//-----------------------------------------------
typedef struct tagSBlock* SPBlock;
typedef struct tagSBlock{
	int nStartVPN;
	int nStartPPN;
	int nBlockSize;			// Num. pages in a block
	int nOriginalVPN;		// DUONGTRAN add
}SBlock;


//-------------------------------
// Linked-node in "BlockList"
//-------------------------------
typedef struct tagSLinkedBlock* SPLinkedBlock;
typedef struct tagSLinkedBlock{
	SPBlock			spBlock;
	SPLinkedBlock		spNext;
}SLinkedBlock;


//-------------------------------------------------------------
// Get Buddy PPN 
//-------------------------------------------------------------
int GetBuddyPPN(int nOrder, int PPN);

//-------------------------------------------------------------
// Get PPN when merged with buddy 
//-------------------------------------------------------------
int GetBuddy_MergedPPN(int nOrder, int PPN);


//-------------------------------------------------------------
// Convert "BlockSize" and "Order"
//-------------------------------------------------------------
int GetOrder(int nBlockSize);
int GetBlockSize(int nOrder);

//-------------------------------------------------------------
// Debug 
//-------------------------------------------------------------

#endif

