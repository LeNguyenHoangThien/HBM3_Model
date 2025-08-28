//---------------------------------------------------
// Filename	: CTRx.h
// Version	: 0.7
// DATE		: 28 Feb 2018
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Tx Rx header
//---------------------------------------------------
#ifndef CTRX_H
#define CTRX_H

#include <string>
#include <stdio.h>
#include "UD_Bus.h"

using namespace std;

//--------------------------------
// Tx Rx class
//--------------------------------
typedef class CTRx* CPTRx;
class CTRx{

public:
	// 1. Contructor and Destructor
	CTRx(string cName, ETRxType eTRxType, EPktType ePktType);
	~CTRx();

	// 2. Control
	// Set value
	EResultType 	PutAx(CPAxPkt cpPkt);
	EResultType 	PutR(CPRPkt cpPkt);
	EResultType 	PutW(CPWPkt cpPkt);
	EResultType 	PutB(CPBPkt cpPkt);
	EResultType 	SetAcceptResult(EResultType eResult);
	EResultType 	SetPair(CPTRx cpTRx);

	// Get value
	EResultType 	GetAcceptResult();
	EPktType 	GetPktType();
	ETRxType 	GetTRxType();
	CPTRx		GetPair();
	CPAxPkt		GetAx();
	CPRPkt		GetR();
	CPWPkt		GetW();
	CPBPkt		GetB();

	EResultType	IsPass();
	EResultType	IsIdle();
	EResultType	IsBusy();
	EResultType 	IsFirstValid();

	// Control
	EResultType 	Reset();
	EResultType 	UpdateState();
	EResultType 	FlushPkt();

	// Debug
	EResultType 	Display();

private:
	// Packet to transmit 
	CPAxPkt 	cpAx;
	CPRPkt 		cpR;
	CPWPkt 		cpW;
	CPBPkt 		cpB;

	// Control info
	string		cName;
	ETRxType	eTRxType;
	EPktType	ePktType;
	EStateType	eState;
	EStateType	eState_D;
	EResultType	eAcceptResult;
	EResultType	eAcceptResult_D;

	// Pair TRx
	CPTRx		cpPair;
};

#endif

