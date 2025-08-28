//------------------------------------------------------------
// Filename	: CBuddy.h 
// Version	: 0.1
// Date		: 19 Apr 2020
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Buddy header 
//------------------------------------------------------------
#ifndef CBUDDY_H
#define CBUDDY_H

#include <string>
#include <math.h>

#include "../Top.h"
#include "BuddyTop.h"

#include "CFreeList.h"
#include "CBlockList.h"
#include "CBlockArray.h"
#include "CPageArray.h"


using namespace std;

//-----------------------------------------------
// Raw PTE structure
//-----------------------------------------------
typedef struct tagSPTE* SPPTE;
typedef struct tagSPTE{
	int	nPPN;
	int	nBlockSize;
	// int	nPageSize;					// Currently 4kB support only
	#ifdef PCAD   //DUONGTRAN add
		int nAscend;
		int nDescend;
		#ifdef PBS
		int nCodedPageSize;    // for PBS, 0: 1x, 1: 16x, 2, 256x
		#endif //PBS
	#endif

	#ifdef AT_ENABLE
		int nContiguity;
	#endif

	#ifdef RCPT_ENABLE
		int nPPN_1;
		int nBlockSize_1;
	#endif

	#ifdef cRCPT_ENABLE
		bool nCluster;
		int  nPPN_1;
		int  nBlockSize_1;
		//int  nPPN_2;
		//int  nBlockSize_2;
		int  nPPN_3;
		int  nBlockSize_3;
	#endif

	#ifdef NAPOT
		int N_flag;
	#endif
}SPTE;


typedef class CBuddy* CPBuddy;
class CBuddy{

public:
	// 1. Contructor. Destructor
	CBuddy(string cName);
	~CBuddy();
	
	// 2. Control
	// Handler
	EResultType	Do_Allocate(int64_t START_VPN, int Num_Requested_Pages);
	//EResultType	Do_Deallocate(int64_t START_VPN, int Num_Requested_Pages); // DUONGTRAN comment
	EResultType	Do_Deallocate(int64_t START_VPN);

	// Set value
	EResultType	Set_FreeList();
	EResultType	Set_FreeList(char typeBuddy); // DUONGTRAN  add
	EResultType	Set_FreeList(int reqBlockSize, int numBlock); // DUONGTRAN  add

	// Get value
	SPPTE*		Get_PageTable(); 
	CPFreeList	Get_FreeList();
	CPBlockList	Get_BlockList();

	CPBlockArray	Get_BlockArray();			// CAMB
	CPPageArray	Get_PageArray();			// CAMB

	int         GetNumNodes();  //DUONGTRAN add to count number of node
	float		GetAvgAddrCovrPTE();

	// Control
	// EResultType	Reset();

	// Debug
	EResultType	CheckBuddy();
	EResultType	PageTable_Display(int Num_Requested_Pages);

	int anchorDistance; //DUONGTRAN add
	bool* memmap;  // DUONGTRAN add
	EResultType InitializeMemMap(int BlockSize, int TotalPages); // DUONGTRAN add
	EResultType InitializeMemMapAligned(int BlockSize, int TotalPages); // DUONGTRAN add
	EResultType InitializeMemMapRandom(int BlockSize, int TotalPages); // DUONGTRAN add

private:
        // Original
	string		cName;

	// Control
	// int		Num_Requested_Pages;
	int64_t		StartVPN;

	SPPTE*		spPageTable;				// [NUM_REQUESTED_PAGES]
	// int		Num_PTE_PageTable;

	CPFreeList	cpFreeList;
//	CPFreeList*  cpMiniZone;
	CPBlockList	cpBlockList;

	CPBlockArray	cpBlockArray;				// CAMB
	CPPageArray	cpPageArray;				// CAMB
	
	// Debug 
	//int		nInitial_NumFreePages;			// Initial num free pages in freelist  DUONGTRAN comment

	// Stat
//public:
//	int	nNumFreePage;

};

#endif

