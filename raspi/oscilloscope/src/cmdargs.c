// #------------------------------------------------------------------------------
 // # Aaron Wood
 // # aamwood
 // # CMPE 121 
 // # 6/6/2019
 // # cmdargs.c for final project
 // #------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope.h"

void cmdargs(char **argv,int *mode,int *tLevel, int *tSlope,int *sRate,int *tChan, int *xScale, int *yScale, int numArgs){
	*mode =-1;
	*tLevel=-1;
	*tSlope=-1;
	*sRate=-1;
	*tChan=-1;
	*xScale=-1;
	*yScale=-1;

	for(int i=1;i<numArgs;i++){
		if(!strcmp(argv[i],"-m")){
			if(argv[i+1]==NULL){
				printf("Mode must be set to either <trigger> or <free>\n");
        		exit(EXIT_FAILURE);
			}
			if(!strcmp(argv[i+1],"free")){*mode=FREE;}
			else if(!strcmp(argv[i+1],"trigger")){*mode=TRIGGER;}
			else{
				printf("Mode must be set to either <trigger> or <free>\n");
        		exit(EXIT_FAILURE);
        	}
		}
	}
	for(int i=1;i<numArgs;i++){
		if(!strcmp(argv[i],"-t")){
			if(argv[i+1]==NULL){
				printf("Trigger Level must be in increments of 100\n");
        		exit(EXIT_FAILURE);
			}
			int trig = atoi(argv[i+1]);
			if(!strcmp(argv[i+1],"0")){
				*tLevel=0;
			}else if(trig==0){
				printf("Trigger slope must be number\n");
        		exit(EXIT_FAILURE);
			} 
			else if((trig<0)||(trig>5000)){
				printf("Trigger Level out of Bounds\n");
        		exit(EXIT_FAILURE);
			}else if(trig%100 != 0){
				printf("Trigger Level must be in increments of 100\n");
        		exit(EXIT_FAILURE);
			}else{
				*tLevel=trig;
			}
		}
	}
	for(int i=1;i<numArgs;i++){
		if(!strcmp(argv[i],"-s")){
			if(argv[i+1]==NULL){
				printf("Trigger slope must be set to either <pos> or <neg> \n");
        		exit(EXIT_FAILURE);
			}
			if(!strcmp(argv[i+1],"pos")){*tSlope=POS;}
			else if(!strcmp(argv[i+1],"neg")){*tSlope=NEG;}
			else{
				printf("Trigger slope must be set to either <pos> or <neg> \n");
        		exit(EXIT_FAILURE);
        	}
		}
	}
	for(int i=1;i<numArgs;i++){
		if(!strcmp(argv[i],"-r")){
			if(argv[i+1]==NULL){
				printf("sample rate must be one of following values:\n");
				printf("1, 10, 20, 50, 100\n");
				exit(EXIT_FAILURE);
			}
			int trig = atoi(argv[i+1]);
			if(trig==0){
				printf("sample rate must be number\n");
        		exit(EXIT_FAILURE);
			} 
			switch(trig){
				case 1:
					*sRate=1;
					break;
				case 10:
					*sRate=10;
					break;
				case 20:
					*sRate=20;
					break;
				case 50:
					*sRate=50;
					break;
				case 100:
					*sRate=100;
					break;
				default:
					printf("sample rate must be one of following values:\n");
					printf("1, 10, 20, 50, 100\n");
        			exit(EXIT_FAILURE);
			}
		}
	}
	for(int i=1;i<numArgs;i++){
		if(!strcmp(argv[i],"-c")){
			if(argv[i+1]==NULL){
				printf("Trigger Channel must be set to either 1 or 2\n");
        		exit(EXIT_FAILURE);
			}
			if(atoi(argv[i+1])==0){
				printf("Trigger Channel must be set to either 1 or 2\n");
        		exit(EXIT_FAILURE);
			} 
			else if(atoi(argv[i+1])==1){*tChan=1;}
			else if(atoi(argv[i+1])==2){*tChan=2;}
			else{
				printf("Trigger Channel must be set to either 1 or 2\n");
        		exit(EXIT_FAILURE);
        	}
		}
	}
	for(int i=1;i<numArgs;i++){
		if(!strcmp(argv[i],"-x")){
			if(argv[i+1]==NULL){
				printf("x scale must be one of following values:\n");
				printf("100, 500, 1000, 2000, 5000, 10000, 50000, 100000\n");
    			exit(EXIT_FAILURE);
			}
			int trig = atoi(argv[i+1]);
			if(trig==0){
				printf("x scale must be number\n");
        		exit(EXIT_FAILURE);
			} 
			switch(trig){
				case 100:
					*xScale=100;
					break;
				case 500:
					*xScale=500;
					break;
				case 1000:
					*xScale=1000;
					break;
				case 2000:
					*xScale=2000;
					break;
				case 5000:
					*xScale=5000;
					break;
				case 10000:
					*xScale=10000;
					break;
				case 50000:
					*xScale=50000;
					break;
				case 100000:
					*xScale=100000;
					break;
				default:
					printf("x scale must be one of following values:\n");
					printf("100, 500, 1000, 2000, 5000, 10000, 50000, 100000\n");
	    			exit(EXIT_FAILURE);
    		}
		}
	}
	for(int i=1;i<numArgs;i++){
		if(!strcmp(argv[i],"-y")){
			if(argv[i+1]==NULL){
				printf("y scale must be one of following values:\n");
				printf("100, 500, 1000, 2000, 25000\n");
    			exit(EXIT_FAILURE);
			}
			int trig = atoi(argv[i+1]);
			if(trig==0){
				printf("y scale must be number\n");
        		exit(EXIT_FAILURE);
			} 
			switch(trig){
				case 100:
					*yScale=100;
					break;
				case 500:
					*yScale=500;
					break;
				case 1000:
					*yScale=1000;
					break;
				case 2000:
					*yScale=2000;
					break;
				case 2500:
					*yScale=2500;
					break;
				default:
					printf("y scale must be one of following values:\n");
					printf("100, 500, 1000, 2000, 2500\n");
        			exit(EXIT_FAILURE);
    		}
		}
	}
	if(*mode==-1){*mode=TRIGGER;}
	if(*tLevel==-1){*tLevel=2500;}
	if(*tSlope==-1){*tSlope=POS;}
	if(*sRate==-1){*sRate=50;}
	if(*tChan==-1){*tChan=1;}
	if(*xScale==-1){*xScale=1000;}
	if(*yScale==-1){*yScale=1000;}
}
