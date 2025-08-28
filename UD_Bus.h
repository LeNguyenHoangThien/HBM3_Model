//------------------------------------------------------------
// Filename	: UD_Bus.h 
// Version	: 0.71
// Date		: 24 Feb 2020
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Unified data type header 
//------------------------------------------------------------
// Global type for bus transaction
//------------------------------------------------------------
#ifndef UD_BUS_H
#define UD_BUS_H

#include "Top.h"
#include "CAxPkt.h"
#include "CRPkt.h"
#include "CWPkt.h"
#include "CBPkt.h"


//-------------------------------
// Transaction. Unified union data
//-------------------------------
typedef union tagUUD* UPUD;
typedef union tagUUD{
	CPAxPkt cpAR;
	CPRPkt	cpR;
	CPAxPkt cpAW;
	CPWPkt	cpW;
	CPBPkt	cpB;
}UUD;


//-------------------------------
// Linked list node (for UD FIFO)
//-------------------------------
typedef struct tagSLinkedUD* SPLinkedUD;
typedef struct tagSLinkedUD{
	UPUD		upData;
	int		nLatency;
	// SPLinkedUD	spPrev;					// FIXME Careful
	SPLinkedUD	spNext;
}SLinkedUD;


//-------------------------------
// Get value
//-------------------------------
int		GetID(UPUD upThis, EUDType eType);


//-------------------------------
// Control (pointer copy)
//-------------------------------
UPUD		Copy_UD(UPUD upThis, EUDType eType);
CPAxPkt		Copy_CAxPkt(CPAxPkt cpPkt);
CPRPkt		Copy_CRPkt(CPRPkt cpPkt);
CPWPkt		Copy_CWPkt(CPWPkt cpPkt);
CPBPkt		Copy_CBPkt(CPBPkt cpPkt);

EResultType	Delete_UD(UPUD upThis, EUDType eType);		// FIXME Check upThis deleted


//-------------------------------
// Debug
//-------------------------------
// EResultType	CheckUD(UPUD upThis, EDUType eType);
EResultType	Display_UD(UPUD upThis, EUDType eType);

#endif

