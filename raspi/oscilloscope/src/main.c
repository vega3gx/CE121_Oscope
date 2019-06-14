// #------------------------------------------------------------------------------
 // # Aaron Wood
 // # aamwood
 // # CMPE 121 
 // # 6/6/2019
 // # main.c for final project
 // #------------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <wiringPi.h>
#include <errno.h>
#include <libusb.h>
#include <shapes.h>
#include <fontinfo.h>
#include <wiringPiI2C.h>
#include "scope.h"

#define  I2C_SLAVE_ADDR 0x53
#define VOLT_MAX 5000
#define BYTE_MAX 255
#define USB_SIZE 64

int main (int argc, char * argv[]){
	//variable initialization and declaration

	//data variables
	char ch1_data[USB_SIZE];
	char ch2_data[USB_SIZE];
	char *ch1_stream = malloc(STREAM*sizeof (char));
	char *ch2_stream = malloc(STREAM*sizeof (char));

	//USB and I2C variables
	int rcvd_bytes = 5; 
	int sent_bytes = 0;
	int return_valTX = 0;
	int return_valRX = 0;
	int fd;
	int succ;
	unsigned int command = 1;
	unsigned int pot1_data, pot2_data;

	//command argument variables
	int mode;
	int tLevel;
	int tSlope;
	int sRate;
	int tChan;
	int xScale;
	int yScale;

	//graphics variables
	int height,width;

	//Initialization functions

	//set up I2C
	fd = wiringPiI2CSetup(I2C_SLAVE_ADDR);
	//parse and save input information
	cmdargs(argv,&mode,&tLevel,&tSlope,&sRate,&tChan,&xScale,&yScale,argc);
	wiringPiI2CWrite(fd, sRate);	//write sample rate to PSOC via i2c
	int PP =findPP(sRate,xScale);	//find number of data points to plot
	libusb_device_handle* dev = USBinit(&succ); //init USB
	tLevel = (tLevel*BYTE_MAX)/VOLT_MAX;	//convert trigger level to terms of bytes
	graphInit(&height,&width);	//initalize graph
	for(;;){
		int ret1 = 3;
		int ret2 = 4;
		//get data from USB
		getData(dev,&ret1,&ret2,ch1_stream,ch2_stream,&rcvd_bytes,PP);
		if (ret1 == 0 && ret2 == 0){
            //successful transfer
            //for test purposes only
		}
		//USB error checking
		if (ret1 < 0){
            printf(libusb_strerror(ret1));
            printf("\tch1 Error\n");
            return(1);
        }
        if (ret2 < 0){
            printf(libusb_strerror(ret2));
            printf("\tch2 Error\n");
            return(1);
		}
		//find offset in stream needed to satisfy trigger
		int triggerOff=0;
		if(tChan ==1){	//trigger on channel 1
			for(int i=0;i<8;i++){triggerOff += findTrig(ch1_stream,tLevel,tSlope);}
		}else{	//trigger on channel 2
			for(int i=0;i<8;i++){triggerOff += findTrig(ch2_stream,tLevel,tSlope);}
		}
		if(mode==FREE){triggerOff=0;} //reset trigger if in mode free
		triggerOff=triggerOff/8;	//average out trigger values
		//read I2C values
		pot1_data = wiringPiI2CRead(fd);
		pot2_data = wiringPiI2CRead(fd);
		//makes graph
		makeGraph(width,height,&ch1_stream[triggerOff],&ch2_stream[triggerOff],xScale,yScale,(10*(pot1_data+10))/3,(10*(pot2_data+10))/3,PP,tChan,tSlope,sRate);
	}
}
