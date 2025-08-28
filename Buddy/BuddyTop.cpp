//-----------------------------------------------------------
// Filename     : BuddyTop.cpp 
// Version	: 0.3
// Date         : 18 Apr 2020
// Contact	: JaeYoung.Hur@gmail.com
// Description  : Global functions 
//-----------------------------------------------------------
// Note
//-----------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "BuddyTop.h"
#include "../Top.h"

// DUONGTRAN comment for RangeBuddy
////---------------------------------------------
//// Convert "BlockSize" to "Order" 
////---------------------------------------------
//int GetOrder(int nBlockSize) {
//
//	// int nOrder = (int)(floorf(log2f(nBlockSize)));		// Very slow
//
//	// int nOrder = -1;
//	switch(nBlockSize) {
//		case 1	:	return (0);	break;
//		case 2	:	return (1);	break;
//		case 4	:	return (2);	break;
//		case 8	:	return (3);	break;
//		case 16	:	return (4);	break;
//		case 32	:	return (5);	break;
//		case 64	:	return (6);	break;
//		case 128:	return (7);	break;
//		case 256:	return (8);	break;
//		case 512:	return (9);	break;
//		default:	assert (0);	break;
//	};
//
//	printf("MAX_ORDER is larger than 9.\n");
//	assert (0);
//	return (-1);		
//};

// DUONGTRAN add for RangeBuddy
int GetOrder(int nBlockSize) {
	#ifdef RBS
		int i = -1;
		while(nBlockSize){
			i++;
			nBlockSize = nBlockSize >> 1;
		}
		return i;
	#elif defined PBS
		int order = MAX_ORDER - 1;
		#ifdef DEBUG_BUDDY
			assert(order % GRANULARITY == 0);
		#endif
		while(order >= 0){
			if(nBlockSize >> order) return order;
			order = order - GRANULARITY;
		}
		assert(0);
		return -1;
	#elif defined BBS
		// int nOrder = (int)(floorf(log2f(nBlockSize)));		// Very slow

		// int nOrder = -1;
		switch(nBlockSize) {
			case 1	:	return (0);	break;
			case 2	:	return (1);	break;
			case 4	:	return (2);	break;
			case 8	:	return (3);	break;
			case 16	:	return (4);	break;
			case 32	:	return (5);	break;
			case 64	:	return (6);	break;
			case 128:	return (7);	break;
			case 256:	return (8);	break;
			case 512:	return (9);	break;
			case 1024:	return (10);break;  //DUONGTRAN add
			default:	assert (0);	break;
		};

		printf("MAX_ORDER is larger than 10.\n");
		assert (0);
		return (-1);		
		
	#else
		assert(0);
	#endif //RBS
};


//---------------------------------------------
// Convert "Order" to "BlockSize"
//---------------------------------------------
int GetBlockSize(int nOrder) {

	// Get number of pages
	// int nBlockSize = pow(2, nOrder);				// Very slow 

	switch(nOrder) {
		case 0	:	return (1);			break;
		case 1	:	return (2);			break;
		case 2	:	return (4);			break;
		case 3	:	return (8);			break;
		case 4	:	return (16);		break;
		case 5	:	return (32);		break;
		case 6	:	return (64);		break;
		case 7	:	return (128);		break;
		case 8	:	return (256);		break;
		case 9	:	return (512);		break;
		case 10	:	return (1024);		break;  // DUONGTRAN add
		case 11	:	return (2048);		break;  // DUONGTRAN add
		case 12	:	return (4096);		break;  // DUONGTRAN add
		case 13	:	return (8192);		break;  // DUONGTRAN add
		case 14	:	return (16384);		break;  // DUONGTRAN add
		case 15	:	return (32768);		break;  // DUONGTRAN add
		case 16	:	return (65536);		break;  // DUONGTRAN add
		case 17	:	return (131072);	break;  // DUONGTRAN add
		case 18	:	return (262144);	break;  // DUONGTRAN add
		case 19	:	return (524288);	break;  // DUONGTRAN add
		case 20	:	return (1048576);	break;  // DUONGTRAN add
		case 21	:	return (2097152);	break;  // DUONGTRAN add
		case 22	:	return (4194304);	break;  // DUONGTRAN add
		default:	assert (0);	break;
	};

//	printf("MAX_ORDER is larger than 9.\n");  // DUONGTRAN comment
	assert (0);

	return (-1);		
};


//---------------------------------------------
// Get buddy PPN 
//---------------------------------------------
int GetBuddyPPN(int nOrder, int PPN) {

	int nBuddyPPN = -1;

	// Shrink 0,1,2,3...
	int PPN_tmp = PPN >> nOrder;

	// Check even odd
	if (PPN_tmp % 2 == 0) {	// Even. +1
		PPN_tmp ++;	
	}
	else {			// Odd. -1
		PPN_tmp --;	
	};

	// Restore 
	nBuddyPPN = PPN_tmp << nOrder;

	return (nBuddyPPN);		
};


//---------------------------------------------
// Get PPN when merged with buddy 
//---------------------------------------------
int GetBuddy_MergedPPN(int nOrder, int PPN) {

	int nBuddyPPN = -1;

	// Shrink 0,1,2,3...
	int PPN_tmp = PPN >> nOrder;

	// Check even odd
	if (PPN_tmp % 2 == 0) {	// Even

		nBuddyPPN = PPN_tmp << nOrder;	
	}
	else {			// Odd. -1
		PPN_tmp --;	
		nBuddyPPN = PPN_tmp << nOrder;	
	};

	return (nBuddyPPN);		
};

