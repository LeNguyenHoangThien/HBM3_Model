//------------------------------------------------------------
// Filename	: CPageArray.h 
// Version	: 0.1
// Date		: 29 Sep 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Class type header
//------------------------------------------------------------
// Summary 
// 	(1) In CAMB, "Start BlockNum in a group" info is necessary.
//	(2) Granularity of GroupSize
//	(3) When PTE is set, refer to this table using index (0,1,2,....)
//------------------------------------------------------------
// Maintenance
//	(1) When application invoked, nMaxNum known.
//	(2) Construct entire array when application invoked.
//	(3) Update, whenever a block allocated and a page mapped.
//	(4) Access, when all allocation finished and a PTE is set
//	(5) Destruct entire array when all de-allocations finished.
//------------------------------------------------------------
#ifndef CPAGEARRAY_H
#define CPAGEARRAY_H

#include <string>

#include "../Top.h"
#include "BuddyTop.h"

//-----------------------------------------------
// Node in "PageArray" 
//-----------------------------------------------
typedef struct tagSPage* SPPage;
typedef struct tagSPage{
	int nBlockNum;		// Block number of first PTE in a group. Block num for aligned-VPN.
}SPage;


using namespace std;

//--------------------------------
// Class
// 	Array of "Page"
//--------------------------------
typedef class CPageArray* CPPageArray;
class CPageArray{

public:
        // 1. Contructor and Destructor
	CPageArray(string cName, int nNumPage);			// Array 
	~CPageArray();	

	// 2. Control
	EResultType	Reset();

	// Set value
	EResultType	SetMaxNum(int nMaxNum);

	EResultType	SetArray(int Index, int BlockNum);	// Set array 
	EResultType	SetArray(int Index, SPPage spPage);

	// Get value
	int		GetCurNum();
	int		GetMaxNum();
	int		GetBlockNum(int Index);
	EResultType	IsEmpty();
	EResultType	IsFull();

	SPPage		GetNode(int Index);

	// Stat
  
	// Debug
	EResultType	CheckPageArray();
	EResultType	Display();

private:
	// Original
	string		cName;
	int		nCurNum;
	int		nMaxNum;				// Number of blocks

	// Control

	// Stat

public: 

	SPPage*		spArray;				// [nMaxNum]

};

#endif

