// #------------------------------------------------------------------------------
 // # Aaron Wood
 // # aamwood
 // # CMPE 121 
 // # 6/6/2019
 // # data.c for final project
 // #------------------------------------------------------------------------------

#include "scope.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <shapes.h>
#include <fontinfo.h>

static int block=0;

void data2stream(char* data, char*stream){
	for(int i=0;i<64;i++){
		stream[i+(block*64)]=data[i];
	}
	block++;
	block=block%4;
}


//returns the index of where the data stream matches trigger
int findTrig(char* data, int trigger, int slope){
	int i=0;
	while(i<STREAM){
		//calculate averages for i and i+1
		int ava = ((data[i])+(2*data[i+1])+(3*data[i+2])+(2*data[i+3])+(data[i+4]))/9;
		int j=i+1;
		int avb = ((data[j])+(2*data[j+1])+(3*data[j+2])+(2*data[j+3])+(data[j+4]))/9;
		if(slope==POS){
			//if a below trigger and b above trigger
			if(ava<=trigger && avb>trigger && findSlope(&data[i])==POS){
				break; //stop iterating
			}
		}else{
			//if a below trigger and b above trigger
			if(ava>=trigger && avb<trigger && findSlope(&data[i])==NEG){
				break;
			}
		}
		i++;
	}
	i--;
	if(i>=1000){i=0;}	//run in free mode if no trigger found
	return(i);
}

int findSlope(char* data){
	int slope=POS;
	int i=0;
	//get average value for first and second piece of data
	int ava = ((data[i])+(2*data[i+1])+(3*data[i+2])+(2*data[i+3])+(data[i+4]))/9;
	int j=i+1;
	int avb = ((data[j])+(2*data[j+1])+(3*data[j+2])+(2*data[j+3])+(data[j+4]))/9;
	//determine if slope is positive or negitive
	if(ava>avb){
		slope=NEG;
	}
	else if(avb>=ava){
		slope=POS;
	}
	return(slope);
}

//find number of data points needed
int findPP(int KSPS,int xscale){
	int PP;
	switch (KSPS){
		case 1:
			PP = (xscale)/100;
			break;
		case 10:
			PP= xscale/10;
			break;
		case 20:
			PP= xscale/5;
			break;
		case 50:
			PP=xscale/2;
			break;
		case 100:
			PP=xscale/10;
			break;
		default:
		PP=xscale/10;
	}
	return(PP);
}
