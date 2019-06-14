// #------------------------------------------------------------------------------
 // # Aaron Wood
 // # aamwood
 // # CMPE 121 
 // # 5/16/2019
 // # lab5.1.c
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
#include "scope.h"

#define SMALL_STREAM 5120
#define BIG_STREAM 51200
#define USB_SIZE 64
#define EP1_ADD (0x01 | 0x80)
#define EP2_ADD (0x02 | 0x80)

static int place = 0; //points to part of stream to transfer next

libusb_device_handle* USBinit(int *succ){
	libusb_init(NULL);
	libusb_device_handle* dev = libusb_open_device_with_vid_pid(NULL, 0x04B4, 0x8051);
	//initialization error checking
    if (dev == NULL){
        perror("device not found\n");
        *succ = 0;
        return(NULL);
    }
    // Reset the USB device.
    if (libusb_reset_device(dev) != 0){
        perror("Device reset failed \n");
        *succ = 0;
        return(NULL);
    } 
    // Set configuration of USB device
    if (libusb_set_configuration(dev, 1) != 0){
        perror("Set configuration failed\n");
        *succ = 0;
        return(NULL);
    } 
    // Claim the interface.
    if (libusb_claim_interface(dev, 0) !=0){
        perror("Cannot claim interface");
        *succ = 0;
        return(NULL);
    }
    *succ = 1;
    return(dev);
}

void getData(libusb_device_handle* dev, int *ret1, int *ret2, char *ch1_data, char *ch2_data, int *rcvd_bytes, int size){
    //BIG_STREAM bytes takes a long time for the USB to transfer, but is more robust
    //SMALL_STREAM is usually sufficent and faster
    int ceil;
    if(size<SMALL_STREAM){ceil=SMALL_STREAM;}
    else{ceil=BIG_STREAM;}
    char ignore[USB_SIZE];
    //transfer stale data into garbage array
    *ret1 = libusb_bulk_transfer(dev,EP1_ADD,ignore,USB_SIZE,rcvd_bytes,0);
	*ret2 = libusb_bulk_transfer(dev,EP2_ADD,ignore,USB_SIZE,rcvd_bytes,0);
    //transfer fresh data to stream after garbadge thrown out
    //fill up entire stream before printing
    for(int i=0;i<(ceil/USB_SIZE);i++){
        *ret1 = libusb_bulk_transfer(dev,EP1_ADD,&ch1_data[place],USB_SIZE,rcvd_bytes,0);
    	*ret2 = libusb_bulk_transfer(dev,EP2_ADD,&ch2_data[place],USB_SIZE,rcvd_bytes,0);
        place=(place+USB_SIZE)%STREAM;
    }
}
