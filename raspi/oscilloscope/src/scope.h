// #------------------------------------------------------------------------------
 // # Aaron Wood
 // # aamwood
 // # CMPE 121 
 // # 6/6/2019
 // # universal .h file for final project
 // #------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <shapes.h>
#include <fontinfo.h>
#include <libusb.h>
#include <wiringPiI2C.h>


#define FREE 0
#define TRIGGER 1
#define POS 0
#define NEG 1
#define STREAM 51200
#define C1KPP
#define C10KPP
#define C20KPP
#define C50KPP
#define C100KPP

typedef struct{
  VGfloat x, y;
} data_point;

//cmd protos
void cmdargs(char **argv,int *mode,int *tLevel, int *tSlope,int *sRate,int *tChan, int *xScale, int *yScale, int numArgs);

//graph protos
void waituntil(int endchar);

void grid(VGfloat x, VGfloat y, int nx, int ny, int w, int h);

void drawBackground(int w, int h, int xdiv, int ydiv, int margin);

void processSamples(char *data, int nsamples,int xstart,int xfinish, float yscale,data_point *point_array);

void plotWave(data_point *data, int nsamples, int yoffset, VGfloat linecolor[4]);

void makeGraph(int width, int height, char *ch1Data, char *ch2Data,int xscale, int yscale, int ch1Offset, int ch2Offset, int plotPoints, int tChan, int tSlope, int sRate);

void graphInit(int* width, int* height);

//USB protos
libusb_device_handle* USBinit(int *succ);

void getData(libusb_device_handle* dev, int *ret1, int *ret2, char *ch1_data, char *ch2_data, int *rcvd_bytes, int size);

//data protos
void data2stream(char* data, char*stream);

int findSlope(char* data);

int findTrig(char* data, int trigger, int slope);

int findPP(int KSPS,int xscale);
