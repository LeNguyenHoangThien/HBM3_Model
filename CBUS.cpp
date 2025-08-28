//-----------------------------------------------------------
// Filename     : CBUS.cpp 
// Version	: 0.79
// Date         : 18 Nov 2019
// Contact	: JaeYoung.Hur@gmail.com
// Description	: Nx1 Bus definition
//-----------------------------------------------------------
// (1) Buffer-less bus 
//-----------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "CBUS.h"


// Constructor
CBUS::CBUS(string cName, int NUM_PORT) {

	// Master interface
	this->cpTx_AR = new CTRx("BUS_Tx_AR", ETRX_TYPE_TX, EPKT_TYPE_AR);
	this->cpTx_AW = new CTRx("BUS_Tx_AW", ETRX_TYPE_TX, EPKT_TYPE_AW);
	this->cpRx_R  = new CTRx("BUS_Rx_R",  ETRX_TYPE_RX, EPKT_TYPE_R);
	this->cpTx_W  = new CTRx("BUS_Tx_W",  ETRX_TYPE_TX, EPKT_TYPE_W);
	this->cpRx_B  = new CTRx("BUS_Rx_B",  ETRX_TYPE_RX, EPKT_TYPE_B);

	// Slave interface
	this->cpRx_AR = new CTRx* [NUM_PORT];
	for (int i=0; i<NUM_PORT; i++) {
		char cName[50];
		sprintf(cName, "BUS_Rx%d_AR",i);
		this->cpRx_AR[i] = new CTRx(cName, ETRX_TYPE_RX, EPKT_TYPE_AR);
	};
	
	this->cpRx_AW = new CTRx* [NUM_PORT];
	for (int i=0; i<NUM_PORT; i++) {
		char cName[50];
		sprintf(cName, "BUS_Rx%d_AW",i);
		this->cpRx_AW[i] = new CTRx(cName, ETRX_TYPE_RX, EPKT_TYPE_AW);
	};

	this->cpTx_R = new CTRx* [NUM_PORT];
	for (int i=0; i<NUM_PORT; i++) {
		char cName[50];
		sprintf(cName, "BUS_Tx%d_R",i);
		this->cpTx_R[i] = new CTRx(cName, ETRX_TYPE_TX, EPKT_TYPE_R);
	};

	this->cpRx_W = new CTRx* [NUM_PORT];
	for (int i=0; i<NUM_PORT; i++) {
		char cName[50];
		sprintf(cName, "BUS_Rx%d_W",i);
		this->cpRx_W[i] = new CTRx(cName, ETRX_TYPE_RX, EPKT_TYPE_W);
	};

	this->cpTx_B = new CTRx* [NUM_PORT];
	for (int i=0; i<NUM_PORT; i++) {
		char cName[50];
		sprintf(cName, "BUS_Tx%d_B",i);
		this->cpTx_B[i] = new CTRx(cName, ETRX_TYPE_TX, EPKT_TYPE_B);
	};

	// Stat
	this->nAR_SI = new int [NUM_PORT];
	this->nAW_SI = new int [NUM_PORT];
	this->nR_SI  = new int [NUM_PORT];
	this->nB_SI  = new int [NUM_PORT];

	for (int i=0; i<NUM_PORT; i++) { this->nAR_SI[i] = -1; };
	for (int i=0; i<NUM_PORT; i++) { this->nAW_SI[i] = -1; };
	for (int i=0; i<NUM_PORT; i++) { this->nR_SI[i]  = -1; };
	for (int i=0; i<NUM_PORT; i++) { this->nB_SI[i]  = -1; };

	// Num bits port (ID encode)
	this->NUM_PORT = NUM_PORT;
	this->BIT_PORT = (int)(ceilf(log2f(NUM_PORT)));
	if(this->BIT_PORT == 0) {this->BIT_PORT = 1;}

	// FIFO
	// this->cpFIFO_AR = new CFIFO("BUS_FIFO_AR", EUD_TYPE_AR, 50);
	this->cpFIFO_AW    = new CFIFO("BUS_FIFO_AW", EUD_TYPE_AW, 50);
	// this->cpFIFO_W  = new CFIFO("BUS_FIFO_W",  EUD_TYPE_W,  50);

	// Arbiter
	this->cpArb_AR	= new CArb("BUS_ARBITER_AR", NUM_PORT);
	this->cpArb_AW	= new CArb("BUS_ARBITER_AW", NUM_PORT);
	
	// Initialize
	this->cName = cName;
	this->nMO_AR = -1;
	this->nMO_AW = -1;
};


// Destructor
CBUS::~CBUS() {

	// Debug
	assert (this->cpTx_AR != NULL);
	assert (this->cpTx_AW != NULL);
	assert (this->cpTx_W  != NULL);
	assert (this->cpRx_R  != NULL);
	assert (this->cpRx_B  != NULL);

	// MI
	delete (this->cpTx_AR);
	delete (this->cpRx_R);
	delete (this->cpTx_AW);
	delete (this->cpTx_W);
	delete (this->cpRx_B);

	// SI
	for (int i=0; i<this->NUM_PORT; i++) { delete (this->cpRx_AR[i]); };
	for (int i=0; i<this->NUM_PORT; i++) { delete (this->cpRx_AW[i]); };
	for (int i=0; i<this->NUM_PORT; i++) { delete (this->cpRx_W[i]);  };
	for (int i=0; i<this->NUM_PORT; i++) { delete (this->cpTx_R[i]);  };
	for (int i=0; i<this->NUM_PORT; i++) { delete (this->cpTx_B[i]);  };

	// FIFO
	delete (this->cpFIFO_AW);
	// delete (this->cpFIFO_AR);
	// delete (this->cpFIFO_W);

	// Arb
	delete (this->cpArb_AR);
	delete (this->cpArb_AW);

	this->cpTx_AR = NULL;
	this->cpTx_AW = NULL;
	this->cpTx_W  = NULL;
	this->cpRx_R  = NULL;
	this->cpRx_B  = NULL;

	for (int i=0; i<this->NUM_PORT; i++) { this->cpRx_AR[i] = NULL; };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpRx_AW[i] = NULL; };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpRx_W[i]  = NULL; };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpTx_R[i]  = NULL; };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpTx_B[i]  = NULL; };

	this->cpFIFO_AW = NULL;
	// this->cpFIFO_AR = NULL;
	// this->cpFIFO_W  = NULL;

	this->cpArb_AR = NULL;
	this->cpArb_AW = NULL;
};


// Initialize
EResultType CBUS::Reset() {

	// MI
	this->cpTx_AR->Reset();
	this->cpTx_AW->Reset();
	this->cpTx_W ->Reset();
	this->cpRx_R ->Reset();
	this->cpRx_B ->Reset();

        // SI 
	for (int i=0; i<this->NUM_PORT; i++) { this->cpRx_AR[i]->Reset(); };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpRx_AW[i]->Reset(); };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpRx_W[i] ->Reset(); };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpTx_R[i] ->Reset(); };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpTx_B[i] ->Reset(); };

	for (int i=0; i<this->NUM_PORT; i++) { this->nAR_SI[i] = 0; };
	for (int i=0; i<this->NUM_PORT; i++) { this->nAW_SI[i] = 0; };
	for (int i=0; i<this->NUM_PORT; i++) { this->nR_SI[i]  = 0; };
	for (int i=0; i<this->NUM_PORT; i++) { this->nB_SI[i]  = 0; };

	// FIFO
	this->cpFIFO_AW->Reset();	
	// this->cpFIFO_AR->Reset();	
	// this->cpFIFO_W->Reset();	

	// Arb
	this->cpArb_AR->Reset();	
	this->cpArb_AW->Reset();	

	this->nMO_AR = 0;
	this->nMO_AW = 0;

	return (ERESULT_TYPE_SUCCESS);
};


//------------------------------------------------------
// AR valid
//------------------------------------------------------
EResultType CBUS::Do_AR_fwd(int64_t nCycle) {

	if(nCycle % BUS_LATENCY != 0){
		return (ERESULT_TYPE_FAIL);
	}

	// Check Tx valid 
	if (this->cpTx_AR->IsBusy() == ERESULT_TYPE_YES) {
		return (ERESULT_TYPE_FAIL);
	};

	// Check MO
	if (this->GetMO_AR() >= MAX_MO_COUNT) {
		return (ERESULT_TYPE_FAIL);
	};

	// Arbiter
	int nCandidateList[this->NUM_PORT];
	for (int nPort = 0; nPort < this->NUM_PORT; nPort++) {

		// Check remote-Tx valid 
		if (this->cpRx_AR[nPort]->GetPair()->IsBusy() == ERESULT_TYPE_YES) {
			nCandidateList[nPort] = 1;
		} 
		else {
			nCandidateList[nPort] = 0;
		};
	};

	int nArbResult = this->cpArb_AR->Arbitration(nCandidateList);

	// Check  
	if (nArbResult == -1) { 		// Nothing arbitrated
		return (ERESULT_TYPE_SUCCESS);
	};

	#ifdef DEBUG
	assert (nArbResult < this->NUM_PORT);
	assert (nArbResult > -1);
	#endif

	// Get Ax
	CPAxPkt cpAR = this->cpRx_AR[nArbResult]->GetPair()->GetAx();

	#ifdef DEBUG
	assert (cpAR != NULL);
	// cpAR->CheckPkt();
	#endif

	// Put Rx
	this->cpRx_AR[nArbResult]->PutAx(cpAR);
	#ifdef DEBUG_BUS
	printf("[Cycle %3ld: %s.Do_AR_fwd] (%s) put Rx_AR[%d].\n", nCycle, this->cName.c_str(), cpAR->GetName().c_str(), nArbResult);
	// cpAR->Display();
	#endif

	// Stat
	this->nAR_SI[nArbResult]++;

	// Encode ID
	int     nID   = cpAR->GetID();
	int64_t nAddr = cpAR->GetAddr();
	int     nLen  = cpAR->GetLen();
	nID = (nID << this->BIT_PORT) + nArbResult;

	// Set pkt
	cpAR->SetPkt(nID, nAddr, nLen);
	
	// Put Tx
	this->cpTx_AR->PutAx(cpAR);
	#ifdef DEBUG_BUS
	printf("[Cycle %3ld: %s.Do_AR_fwd] (%s) put Tx_AR.\n", nCycle, this->cName.c_str(), cpAR->GetName().c_str());
	// cpAR->Display();
	#endif

	return (ERESULT_TYPE_SUCCESS);	
};


//------------------------------------------------------
// AW valid
//------------------------------------------------------
EResultType CBUS::Do_AW_fwd(int64_t nCycle) {

	if(nCycle % BUS_LATENCY != 0){
		return (ERESULT_TYPE_FAIL);
	}

	// Check Tx valid 
	if (this->cpTx_AW->IsBusy() == ERESULT_TYPE_YES) {
		return (ERESULT_TYPE_FAIL);
	};

	// Check MO
	if (this->GetMO_AW() >= MAX_MO_COUNT) {
		return (ERESULT_TYPE_FAIL);
	};

	// Arbiter
	int nCandidateList[this->NUM_PORT];
	for (int nPort = 0; nPort < this->NUM_PORT; nPort++) {

		// Check remote-Tx valid 
		if (this->cpRx_AW[nPort]->GetPair()->IsBusy() == ERESULT_TYPE_YES) {
			nCandidateList[nPort] = 1;
		} 
		else {
			nCandidateList[nPort] = 0;
		};
	};

	int nArbResult = this->cpArb_AW->Arbitration(nCandidateList);

	// Check 
	if (nArbResult == -1) {			// Nothing arbitrated
		return (ERESULT_TYPE_SUCCESS);
	};

	#ifdef DEBUG
	assert (nArbResult < this->NUM_PORT);
	assert (nArbResult > -1);
	#endif

	// Get Ax
	CPAxPkt cpAW = this->cpRx_AW[nArbResult]->GetPair()->GetAx();

	#ifdef DEBUG
	assert (cpAW != NULL);
	// cpAW->CheckPkt();
	#endif

	// Put Rx
	this->cpRx_AW[nArbResult]->PutAx(cpAW);
	// printf("[Cycle %3ld: %s.Do_AW_fwd] (%s) put Rx_AW[%d].\n", nCycle, this->cName.c_str(), cpAW->GetName().c_str(), nArbResult);
	// cpAW->Display();

	// Stat
	this->nAW_SI[nArbResult]++;

	// Encode ID
	int     nID   = cpAW->GetID();
	int64_t nAddr = cpAW->GetAddr();
	int     nLen  = cpAW->GetLen();
	nID = (nID << this->BIT_PORT) + nArbResult;

	// Set pkt
	cpAW->SetPkt(nID, nAddr, nLen);
	
	// Put Tx
	this->cpTx_AW->PutAx(cpAW);
	// printf("[Cycle %3ld: %s.Do_AW_fwd] (%s) put Tx_AW.\n", nCycle, this->cName.c_str(), cpAW->GetName().c_str());
	// cpAW->Display();


	#ifdef USE_W_CHANNEL
	// Generate new Ax arbitrated 
	UPUD upAW_new = new UUD;
	upAW_new->cpAW = Copy_CAxPkt(cpAW);

	// Push AW FIFO
	this->cpFIFO_AW->Push(upAW_new);

	// Debug	
	// printf("[Cycle %3ld: %s.Do_AW_fwd] (%s) push FIFO_AW.\n", nCycle, this->cName.c_str(), cpAW->GetName());
	// this->cpFIFO_AW->Display();
	// this->cpFIFO_AW->CheckFIFO();
	
	// Maintain     
	Delete_UD(upAW_new, EUD_TYPE_AW);

	#endif

	return (ERESULT_TYPE_SUCCESS);	
};


//------------------------------------------------------
// W valid
//------------------------------------------------------
EResultType CBUS::Do_W_fwd(int64_t nCycle) {

	if(nCycle % BUS_LATENCY != 0){
		return (ERESULT_TYPE_FAIL);
	}

	// Check Tx valid
	if (this->cpTx_W->IsBusy() == ERESULT_TYPE_YES) {
		return (ERESULT_TYPE_FAIL);
	};

	// Check FIFO_AW
	if (this->cpFIFO_AW->IsEmpty() == ERESULT_TYPE_YES) {
		return (ERESULT_TYPE_SUCCESS);
	};

	// Get arbitrated port
	// int nArb = this->cpFIFO_AW->GetTop()->cpAW->GetID() % this->NUM_PORT; // Only when power-of-2 ports FIXME
	int AxID = this->cpFIFO_AW->GetTop()->cpAW->GetID();
	int nArb = GetPortNum(AxID);

	#ifdef DEBUG	
	assert (nArb > -1);
	assert (nArb < this->NUM_PORT);
	#endif

	// Check remote-Tx valid
	if (this->cpRx_W[nArb]->GetPair()->IsBusy() == ERESULT_TYPE_NO) {
		return (ERESULT_TYPE_SUCCESS);	
	};

	// Get W
	CPWPkt cpW = this->cpRx_W[nArb]->GetPair()->GetW();
	
	#ifdef DEBUG
	assert (cpW != NULL);
	// cpW->CheckPkt();
	#endif

	// Put Rx 
	this->cpRx_W[nArb]->PutW(cpW);
	// printf("[Cycle %3ld: %s.Do_W_fwd] (%s) put Rx_W[%d].\n", nCycle, this->cName.c_str(), cpW->GetName().c_str(), nArbResult);
	// cpW->Display();

	// Encode ID
	int nID = cpW->GetID();
	nID = (nID << this->BIT_PORT) + nArb;

	// Set pkt
	cpW->SetID(nID);

	// Put Tx 
	this->cpTx_W->PutW(cpW);

	// Debug
	if (cpW->IsLast() == ERESULT_TYPE_YES) {	
		// printf("[Cycle %3ld: %s.Do_W_fwd] (%s) WLAST put Tx_W.\n", nCycle, this->cName.c_str(), cpW->GetName().c_str());
		// cpW->Display();
	} 
	else {
		// printf("[Cycle %3ld: %s.Do_W_fwd] (%s) put Tx_W.\n", nCycle, this->cName.c_str(), cpW->GetName().c_str());
		// cpW->Display();
	};

	return (ERESULT_TYPE_SUCCESS);	
};


//------------------------------------------------------
// AR ready
//------------------------------------------------------
EResultType CBUS::Do_AR_bwd(int64_t nCycle) {

	//if(nCycle % BUS_LATENCY != 0){
	//	return (ERESULT_TYPE_FAIL);
	//}

	// Check Tx ready
	if (this->cpTx_AR->GetAcceptResult() == ERESULT_TYPE_REJECT) {
		return (ERESULT_TYPE_SUCCESS);
	};

	CPAxPkt cpAR = this->cpTx_AR->GetAx();

	#ifdef DEBUG_BUS
	assert (cpAR != NULL);
	assert (this->cpTx_AR->IsBusy() == ERESULT_TYPE_YES);
	string cARPktName = cpAR->GetName();
	printf("[Cycle %3ld: %s.Do_AR_bwd] (%s) handshake Tx_AR.\n", nCycle, this->cName.c_str(), cARPktName.c_str());
	// cpAR->Display();
	#endif

	// Get arbitrated port FIXME Need? No
	int nArbResult = this->cpArb_AR->GetArbResult();
	assert (nArbResult > -1);
	assert (nArbResult < this->NUM_PORT);

	// Debug
	// int nPort = (cpAR->GetID()) % this->NUM_PORT; // Only when this->NUM_PORT power-of-2. FIXME
	// assert (nArbResult == nPort);

	// Check Rx valid 
	// if (this->cpRx_AR[nPort]->IsBusy() == ERESULT_TYPE_YES) {
	// 	// Set ready	
	// 	this->cpRx_AR[nPort]->SetAcceptResult(ERESULT_TYPE_ACCEPT);
	// 
	// 	// MO
	// 	if (this->cpTx_AR->IsBusy() == ERESULT_TYPE_YES ) {
	// 		this->Increase_MO_AR();
	// 	};
	// }; 

	int nID = cpAR->GetID();
	int nPort = GetPortNum(nID); 

	#ifdef DEBUG // FIXME Need? No
	assert (nArbResult == nPort);
	#endif

	// Check Rx valid 
	if (this->cpRx_AR[nPort]->IsBusy() == ERESULT_TYPE_YES) {
		// Set ready	
		this->cpRx_AR[nPort]->SetAcceptResult(ERESULT_TYPE_ACCEPT);
		#ifdef DEBUG_BUS
		printf("[Cycle %3ld: %s.Do_AR_bwd] (%s) handshake Rx_AR[%d].\n", nCycle, this->cName.c_str(), (this->cpRx_AR[nPort]->GetAx()->GetName()).c_str(), nPort);
		#endif
		// MO
		if (this->cpTx_AR->IsBusy() == ERESULT_TYPE_YES ) {
			this->Increase_MO_AR();
		};
	}; 

	return (ERESULT_TYPE_SUCCESS);
};


//------------------------------------------------------
// AW ready
//------------------------------------------------------
EResultType CBUS::Do_AW_bwd(int64_t nCycle) {

	//if(nCycle % BUS_LATENCY != 0){
	//	return (ERESULT_TYPE_FAIL);
	//}

	// Check Tx ready
	if (this->cpTx_AW->GetAcceptResult() == ERESULT_TYPE_REJECT) {
		return (ERESULT_TYPE_SUCCESS);
	};

	CPAxPkt cpAW = this->cpTx_AW->GetAx();

	#ifdef DEBUG_BUS
	assert (cpAW != NULL);
	assert (this->cpTx_AW->IsBusy() == ERESULT_TYPE_YES);
	printf("[Cycle %3ld: %s.Do_AW_bwd] (%s) handshake Tx_AW.\n", nCycle, this->cName.c_str(), (cpAW->GetName()).c_str());
	// cpAW->Display();
	#endif

	#ifdef DEBUG
	// Get arbitrated port  FIXME Need? No
	int nArbResult = this->cpArb_AW->GetArbResult();
	assert (nArbResult > -1);
	assert (nArbResult < this->NUM_PORT);
	#endif

	// Debug
	// int nPort = (cpAW->GetID()) % this->NUM_PORT; // Only when powe-of-2
	// assert (nArbResult == nPort);

	// // Check Rx valid 
	// if (this->cpRx_AW[nPort]->IsBusy() == ERESULT_TYPE_YES) {
	// 	// Set ready
	// 	this->cpRx_AW[nPort]->SetAcceptResult(ERESULT_TYPE_ACCEPT);
	// 
	// 	// MO
	// 	if (this->cpTx_AW->IsBusy() == ERESULT_TYPE_YES ) {
	// 		this->Increase_MO_AW();
	// 	};
	// }; 

	int nID = cpAW->GetID();
	int nPort = GetPortNum(nID); 

	#ifdef DEBUG // FIXME Need? No
	assert (nArbResult == nPort);
	#endif

	// Check Rx valid 
	if (this->cpRx_AW[nPort]->IsBusy() == ERESULT_TYPE_YES) {
		// Set ready
		this->cpRx_AW[nPort]->SetAcceptResult(ERESULT_TYPE_ACCEPT);
		#ifdef DEBUG_BUS
		printf("[Cycle %3ld: %s.Do_AW_bwd] (%s) handshake Rx_AW[%d].\n", nCycle, this->cName.c_str(), (this->cpRx_AW[nPort]->GetAx()->GetName()).c_str(), nPort);
		#endif

		// MO
		if (this->cpTx_AW->IsBusy() == ERESULT_TYPE_YES ) {
			this->Increase_MO_AW();
		};
	}; 

	return (ERESULT_TYPE_SUCCESS);
};


//------------------------------------------------------
// W ready
//------------------------------------------------------
EResultType CBUS::Do_W_bwd(int64_t nCycle) {

	//if(nCycle % BUS_LATENCY != 0){
	//	return (ERESULT_TYPE_FAIL);
	//}

	// Check Tx ready
	if (this->cpTx_W->GetAcceptResult() == ERESULT_TYPE_REJECT) {
		return (ERESULT_TYPE_SUCCESS);
	};

	CPWPkt cpW = this->cpTx_W->GetW();

	#ifdef DEBUG_BUS
	assert (cpW != NULL);
	assert (this->cpTx_W->IsBusy() == ERESULT_TYPE_YES);
	 printf("[Cycle %3ld: %s.Do_W_bwd] (%s) handshake Tx_W.\n", nCycle, this->cName.c_str(), cpW->GetName().c_str());
	// cpW->Display();
	assert (this->cpFIFO_AW->IsEmpty() == ERESULT_TYPE_NO);
	#endif

	// Get arbitrated port
	// int nArb = this->cpFIFO_AW->GetTop()->cpAW->GetID() % this->NUM_PORT; // Only when power-of-2 ports
	int AxID = this->cpFIFO_AW->GetTop()->cpAW->GetID();
	int nArb = GetPortNum(AxID);

	#ifdef DEBUG
	assert (nArb >= 0);
	assert (nArb < this->NUM_PORT);
	#endif

	// Check Rx valid 
	if (this->cpRx_W[nArb]->IsBusy() == ERESULT_TYPE_YES) {
		// Set ready
		this->cpRx_W[nArb]->SetAcceptResult(ERESULT_TYPE_ACCEPT);
	}; 

	// Check WLAST. Pop FIFO_AW
	if (this->cpTx_W->IsPass() == ERESULT_TYPE_YES and cpW->IsLast() == ERESULT_TYPE_YES) {

		#ifdef DEBUG
		assert (this->cpFIFO_AW->IsEmpty() == ERESULT_TYPE_NO);
		#endif

		UPUD up_new = this->cpFIFO_AW->Pop();
		// this->cpFIFO_AW->CheckFIFO();

		// Maintain	
		Delete_UD(up_new, EUD_TYPE_AW);
	};

	return (ERESULT_TYPE_SUCCESS);
};


//------------------------------------------------------
// R Valid
//------------------------------------------------------
EResultType CBUS::Do_R_fwd(int64_t nCycle) {

	if(nCycle % BUS_LATENCY != 0){
		return (ERESULT_TYPE_FAIL);
	}

    // Check remote-Tx valid 
    CPRPkt cpR = this->cpRx_R->GetPair()->GetR();
    if (cpR == NULL) {
        return (ERESULT_TYPE_SUCCESS);
    };
	// cpR->CheckPkt();

	// Get destination port
	int nID = cpR->GetID();
	// int nPort = nID % this->NUM_PORT; // Only when power-of-2 FIXME
	int nPort = GetPortNum(nID);

	// Check Tx valid 
	if (this->cpTx_R[nPort]->IsBusy() == ERESULT_TYPE_YES) {
         return (ERESULT_TYPE_SUCCESS);
	};

	// Put Rx
	this->cpRx_R->PutR(cpR);

	// Debug
	#ifdef DEBUG_BUS
	if (cpR->IsLast() == ERESULT_TYPE_YES) {	
		printf("[Cycle %3ld: %s.Do_R_fwd] (%s) RID 0x%x RLAST put Rx_R.\n", nCycle, this->cName.c_str(), cpR->GetName().c_str(), cpR->GetID());
		// cpR->Display();
	} 
	else {
		printf("[Cycle %3ld: %s.Do_R_fwd] (%s) RID 0x%x put Rx_R.\n", nCycle, this->cName.c_str(), cpR->GetName().c_str(), cpR->GetID());
		// cpR->Display();
	};
	#endif

	// Decode ID
	CPRPkt cpR_new = Copy_CRPkt(cpR);
	nID = nID >> this->BIT_PORT;
	cpR_new->SetID(nID);
	
	// Put Tx 
	this->cpTx_R[nPort]->PutR(cpR_new);

	// Stat
	this->nR_SI[nPort]++;

	// Debug
	#ifdef DEBUG_BUS
	if (cpR_new->IsLast() == ERESULT_TYPE_YES) {	
		 printf("[Cycle %3ld: %s.Do_R_fwd] (%s) RID 0x%x RLAST put Tx_R[%d].\n", nCycle, this->cName.c_str(), cpR_new->GetName().c_str(), cpR_new->GetID(), nPort);
		 // cpR_new->Display();
	} 
	else {
		 printf("[Cycle %3ld: %s.Do_R_fwd] (%s) RID 0x%x put Tx_R[%d].\n", nCycle, this->cName.c_str(), cpR_new->GetName().c_str(), cpR_new->GetID(), nPort);
		 // cpR_new->Display();
	};
	#endif

	// Maintain
	delete (cpR_new);
	cpR_new = NULL;

        return (ERESULT_TYPE_SUCCESS);
};


//------------------------------------------------------
// B Valid
//------------------------------------------------------
EResultType CBUS::Do_B_fwd(int64_t nCycle) {

	if(nCycle % BUS_LATENCY != 0){
		return (ERESULT_TYPE_FAIL);
	}

	// Check remote-Tx valid
	CPBPkt cpB = this->cpRx_B->GetPair()->GetB();
	if (cpB == NULL) {
		return (ERESULT_TYPE_SUCCESS);
	};
	// cpB->CheckPkt();
	
	// Get destination port
	int nID = cpB->GetID();
	// int nPort = nID % this->NUM_PORT;	 // Only when power-of-2 ports
	int nPort = GetPortNum(nID);

	// Check Tx valid 
	if (this->cpTx_B[nPort]->IsBusy() == ERESULT_TYPE_YES) {
                return (ERESULT_TYPE_SUCCESS);
	};

	// Put Rx
	this->cpRx_B->PutB(cpB);

	// Debug
	// printf("[Cycle %3ld: %s.Do_B_fwd] (%s) BID 0x%x put Rx_B.\n", nCycle, this->cName.c_str(), cpB->GetName().c_str(), cpB->GetID() );
	// cpB->Display();

	// Decode ID
	CPBPkt cpB_new = Copy_CBPkt(cpB);
	nID = nID >> this->BIT_PORT;
	cpB_new->SetID(nID);	

	// Put Tx 
	this->cpTx_B[nPort]->PutB(cpB_new);

	// Stat
	this->nB_SI[nPort]++;

	// Debug
	// printf("[Cycle %3ld: %s.Do_B_fwd] (%s) BID 0x%x put Tx_B[%d].\n", nCycle, this->cName.c_str(), cpB_new->GetName().c_str(), cpB->GetID(), nPort);
	// cpB_new->Display();

	// Maintain
	delete (cpB_new);
	cpB_new = NULL;
		
	return (ERESULT_TYPE_SUCCESS);
};


//------------------------------------------------------
// R ready
//------------------------------------------------------
EResultType CBUS::Do_R_bwd(int64_t nCycle) {

	//if(nCycle % BUS_LATENCY != 0){
	//	return (ERESULT_TYPE_FAIL);
	//}

	// Check Rx valid 
	CPRPkt cpR = this->cpRx_R->GetR();
	if (cpR == NULL) {
	        return (ERESULT_TYPE_SUCCESS);
	};

	#ifdef DEBUG
	// cpR->CheckPkt();
	assert (this->cpRx_R->IsBusy() == ERESULT_TYPE_YES);
	#endif
	
	// Get destination port
	int nID = cpR->GetID();
	// int nPort = nID % this->NUM_PORT; // Only when power-of-2 ports
	int nPort = GetPortNum(nID);
	
	// Check Tx ready FIXME FIXME
	EResultType eAcceptResult = this->cpTx_R[nPort]->GetAcceptResult();
	if (eAcceptResult == ERESULT_TYPE_REJECT) {
	    return (ERESULT_TYPE_SUCCESS);
	};

	#ifdef DEBUG_BUS
	assert (eAcceptResult == ERESULT_TYPE_ACCEPT);
	printf("[Cycle %3ld: %s.Do_R_bwd] (%s) handshake Tx_R[%d].\n", nCycle, this->cName.c_str(), this->cpTx_R[nPort]->GetR()->GetName().c_str(), nPort);
	#endif

	// Set ready 
	this->cpRx_R->SetAcceptResult(ERESULT_TYPE_ACCEPT);

	// Debug
	if (cpR->IsLast() == ERESULT_TYPE_YES) {
		#ifdef DEBUG_BUS
		printf("[Cycle %3ld: %s.Do_R_bwd] (%s) RLAST handshake Rx_R.\n", nCycle, this->cName.c_str(), cpR->GetName().c_str());
		#endif
	
		// Decrease MO
		this->Decrease_MO_AR();	
	} 
	#ifdef DEBUG_BUS
	else {
		printf("[Cycle %3ld: %s.Do_R_bwd] (%s) handshake Rx_R.\n", nCycle, this->cName.c_str(), cpR->GetName().c_str());
	};
	#endif
	
	// Debug
	// cpR->Display();
	
	return (ERESULT_TYPE_SUCCESS);
};


//------------------------------------------------------
// B ready
//------------------------------------------------------
EResultType CBUS::Do_B_bwd(int64_t nCycle) {
	
	//if(nCycle % BUS_LATENCY != 0){
	//	return (ERESULT_TYPE_FAIL);
	//}

	// Check Rx valid 
	CPBPkt cpB = this->cpRx_B->GetB();
	if (cpB == NULL) {
	        return (ERESULT_TYPE_SUCCESS);
	};

	#ifdef DEBUG
	// cpB->CheckPkt();
	assert (this->cpRx_B->IsBusy() == ERESULT_TYPE_YES);
	#endif
	
	// Get destination port
	int nID = cpB->GetID();
	// int nPort = nID % this->NUM_PORT; // Only when power-of-2 ports
	int nPort = GetPortNum(nID);
	
	// Check Tx ready FIXME FIXME When SLV->Do_R_bwd and BUS->Do_R_bwd reverse, function issue
	EResultType eAcceptResult = this->cpTx_B[nPort]->GetAcceptResult();
	if (eAcceptResult == ERESULT_TYPE_REJECT) {
	        return (ERESULT_TYPE_SUCCESS);
	};

	#ifdef DEBUG
	assert (eAcceptResult == ERESULT_TYPE_ACCEPT);
	#endif

	// Set ready 
	this->cpRx_B->SetAcceptResult(ERESULT_TYPE_ACCEPT);

	// Debug
	#ifdef DEBUG_BUS
	 printf("[Cycle %3ld: %s.Do_B_bwd] (%s) B handshake Rx_B.\n", nCycle, this->cName.c_str(), cpB->GetName().c_str());
	#endif
	
	// Decrease MO
	this->Decrease_MO_AW();	
	
	// Debug
	// cpB->Display();

        return (ERESULT_TYPE_SUCCESS);
};


// Debug
EResultType CBUS::CheckLink() {

	assert (this->cpTx_AR != NULL);
	assert (this->cpTx_AW != NULL);
	assert (this->cpTx_W  != NULL);
	assert (this->cpRx_R  != NULL);
	assert (this->cpRx_B  != NULL);
	assert (this->cpTx_AR->GetPair() != NULL);
	assert (this->cpTx_AW->GetPair() != NULL);
	assert (this->cpTx_W->GetPair()  != NULL);
	assert (this->cpRx_R ->GetPair() != NULL);
	assert (this->cpRx_B ->GetPair() != NULL);
	assert (this->cpTx_AR->GetTRxType() != this->cpTx_AR->GetPair()->GetTRxType());
	assert (this->cpTx_AW->GetTRxType() != this->cpTx_AW->GetPair()->GetTRxType());
	assert (this->cpTx_W ->GetTRxType() != this->cpTx_W ->GetPair()->GetTRxType());
	assert (this->cpRx_R ->GetTRxType() != this->cpRx_R ->GetPair()->GetTRxType());
	assert (this->cpRx_B ->GetTRxType() != this->cpRx_B ->GetPair()->GetTRxType());
	assert (this->cpTx_AR->GetPktType() == this->cpTx_AR->GetPair()->GetPktType());
	assert (this->cpTx_AW->GetPktType() == this->cpTx_AW->GetPair()->GetPktType());
	assert (this->cpTx_W ->GetPktType() == this->cpTx_W ->GetPair()->GetPktType());
	assert (this->cpRx_R ->GetPktType() == this->cpRx_R ->GetPair()->GetPktType());
	assert (this->cpRx_B ->GetPktType() == this->cpRx_B ->GetPair()->GetPktType());
	assert (this->cpTx_AR->GetPair()->GetPair()== this->cpTx_AR);
	assert (this->cpTx_AW->GetPair()->GetPair()== this->cpTx_AW);
	assert (this->cpTx_W ->GetPair()->GetPair()== this->cpTx_W);
	assert (this->cpRx_R ->GetPair()->GetPair()== this->cpRx_R);
	assert (this->cpRx_B ->GetPair()->GetPair()== this->cpRx_B);

        return (ERESULT_TYPE_SUCCESS);
};


// Increase AR MO count 
EResultType CBUS::Increase_MO_AR() {  

	this->nMO_AR++;

	#ifdef DEBUG
	assert (this->nMO_AR >= 0);
	assert (this->nMO_AR <= MAX_MO_COUNT);
	#endif

        return (ERESULT_TYPE_SUCCESS);
};


// Decrease AR MO count 
EResultType CBUS::Decrease_MO_AR() {  

	this->nMO_AR--;

	#ifdef DEBUG
	assert (this->nMO_AR >= 0);
	assert (this->nMO_AR <= MAX_MO_COUNT);
	#endif

        return (ERESULT_TYPE_SUCCESS);
};


// Increase AW MO count 
EResultType CBUS::Increase_MO_AW() {  

	this->nMO_AW++;

	#ifdef DEBUG
	assert (this->nMO_AW >= 0);
	assert (this->nMO_AW <= MAX_MO_COUNT);
	#endif

        return (ERESULT_TYPE_SUCCESS);
};


// Decrease AW MO count 
EResultType CBUS::Decrease_MO_AW() {  

	this->nMO_AW--;

	#ifdef DEBUG
	assert (this->nMO_AW >= 0);
	assert (this->nMO_AW <= MAX_MO_COUNT);
	#endif

        return (ERESULT_TYPE_SUCCESS);
};


// Get AR MO count
int CBUS::GetMO_AR() {

	#ifdef DEBUG
	assert (this->nMO_AR >= 0);
	assert (this->nMO_AR <= MAX_MO_COUNT);
	#endif

	return (this->nMO_AR);
};


// Get AW MO count
int CBUS::GetMO_AW() {

	#ifdef DEBUG
	assert (this->nMO_AW >= 0);
	assert (this->nMO_AW <= MAX_MO_COUNT);
	#endif

	return (this->nMO_AW);
};


int CBUS::GetPortNum(int nID) {

	int nPort = -1;

	if      (this->BIT_PORT == 1) nPort = nID & 0x1;  	// 1-bit.  Upto 2 ports
	else if (this->BIT_PORT == 2) nPort = nID & 0x3; 	// 2-bits. Upto 4 ports
	else if (this->BIT_PORT == 3) nPort = nID & 0x7;
	else if (this->BIT_PORT == 4) nPort = nID & 0xF;
	else assert(0);

	#ifdef DEBUG
	assert (nPort >= 0);	
	#endif
	
	return (nPort);
};


// Update state
EResultType CBUS::UpdateState(int64_t nCycle) {  

	// MI 
        this->cpTx_AR->UpdateState();
	this->cpTx_AW->UpdateState();
	// this->cpTx_W ->UpdateState();
        this->cpRx_R ->UpdateState();
        this->cpRx_B ->UpdateState();

	// SI 
	for (int i=0; i<this->NUM_PORT; i++) { this->cpRx_AR[i]->UpdateState(); };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpRx_AW[i]->UpdateState(); };
	// for (int i=0; i<this->NUM_PORT; i++) { this->cpRx_W[i] ->UpdateState(); };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpTx_R[i] ->UpdateState(); };
	for (int i=0; i<this->NUM_PORT; i++) { this->cpTx_B[i] ->UpdateState(); };

	// FIFO
	this->cpFIFO_AW->UpdateState();

        return (ERESULT_TYPE_SUCCESS);
}; 


// Stat 
EResultType CBUS::PrintStat(int64_t nCycle, FILE *fp) {

	printf("--------------------------------------------------------\n");
	printf("\t Name : %s\n", this->cName.c_str());
	printf("--------------------------------------------------------\n");

	printf("\t Number of SI ports         : %d\n\n",this->NUM_PORT);

	for (int i=0; i<this->NUM_PORT; i++) {
		printf("\t Number of AR SI[%d]         : %d\n",	i, this->nAR_SI[i]);
	};
	printf("\n");
	for (int i=0; i<this->NUM_PORT; i++) {
		printf("\t Number of AW SI[%d]         : %d\n",	i, this->nAW_SI[i]);
	};
	printf("\n");
	for (int i=0; i<this->NUM_PORT; i++) {
		printf("\t Number of R SI[%d]          : %d\n",	i, this->nR_SI[i]);
	};
	printf("\n");
	for (int i=0; i<this->NUM_PORT; i++) {
		printf("\t Number of B SI[%d]          : %d\n",	i, this->nB_SI[i]);
	};
	printf("\n");
	
	
	// printf("\t Max allowed AR MO                : %d\n",	 MAX_MO_COUNT);
	// printf("\t Max allowed AW MO                : %d\n\n",MAX_MO_COUNT);
	
        return (ERESULT_TYPE_SUCCESS);
};

