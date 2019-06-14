// #------------------------------------------------------------------------------
 // # Aaron Wood
 // # aamwood
 // # CMPE 121 
 // # 6/6/2019
 // # graphics.c for final project
 // #------------------------------------------------------------------------------

#include "scope.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <shapes.h>
#include <fontinfo.h>

//defines for Pixel per volt adjustment
#define Y100 950
#define Y500 720
#define Y1000 950
#define Y2000 980
#define Y2500 1200

#define Y100OFF 1000
#define Y500OFF 200
#define Y1000OFF 200
#define Y2000OFF 200
#define Y2500OFF 200

//local function declaration
void printScaleSettings(int xscale, int yscale, int xposition, int yposition, VGfloat tcolor[4], int tChan, int tSlope, int sRate);


// Draw grid lines
void grid(VGfloat x, VGfloat y,int nx, int ny,int w, int h){
    VGfloat ix, iy;
    Stroke(128, 128, 128, 0.5); // Set color
    StrokeWidth(2); // Set width of lines
    for (ix = x; ix <= x + w; ix += w/nx) {Line(ix, y, ix, y + h);}
    for (iy = y; iy <= y + h; iy += h/ny) {Line(x, iy, x + w, iy);}
}

// Draw the background for the oscilloscope screen
void drawBackground(int w, int h, int xdiv, int ydiv,int margin){
  VGfloat x1 = margin;
  VGfloat y1 = margin;
  VGfloat x2 = w - 2*margin;
  VGfloat y2 = h - 2*margin;

  Background(128, 128, 128); //color grey

  Stroke(204, 204, 204, 1); //color lighter grey
  StrokeWidth(1);
  
  Fill(1, 1, 1, 1);     //color black
  Rect(10, 10, w-20, h-20); // Draw framing rectangle

  grid(x1, y1, xdiv, ydiv, x2, y2); // Draw grid lines
}

// Display x and scale settings
void printScaleSettings(int xscale, int yscale, int xposition, int yposition, VGfloat tcolor[4], int tChan, int tSlope, int sRate){
    char str[1000];
    setfill(tcolor);
    //print xscale
    if (xscale >= 1000){sprintf(str, "X scale = %0d ms/div", xscale/1000);}
    else{sprintf(str, "X scale = %0d us/div", xscale);}
    Text(xposition, yposition, str, SansTypeface, 18);
    //print yscale
    sprintf(str, "Y scale = %3.2f V/div", yscale * 0.001);
    Text(xposition, yposition-50, str, SansTypeface, 18);
    //print trigger channel
    sprintf(str, "Trigger on CH %d", tChan);
    Text(xposition, yposition-100, str, SansTypeface, 18);
    //print trigger slope
    if(tSlope==POS){sprintf(str, "Trigger on POSITIVE slope");}
    else{sprintf(str, "Trigger on NEGITIVE slope");}
    Text(xposition, yposition-150, str, SansTypeface, 18);
    //print sample rate
    sprintf(str, "Sample rate is %dKsps", sRate);
    Text(xposition, yposition-200, str, SansTypeface, 18);
}

// Convert waveform samples into screen coordinates
void processSamples(char *data,int nsamples,int xstart,int xfinish,float yscale,data_point *point_array){
    VGfloat x1, y1;
    data_point p;

    for (int i=0; i< nsamples; i++){
        x1 = xstart + (xfinish-xstart)*i/nsamples;
        y1 = data[i] * 5 * yscale/256;
        p.x = x1;
        p.y = y1;
        point_array[i] = p;
    }
}

// Plot waveform
void plotWave(data_point *data,int nsamples,int yoffset,VGfloat linecolor[4]){

    data_point p;

    Stroke(linecolor[0], linecolor[1], linecolor[2], linecolor[3]);
    StrokeWidth(4);
    VGfloat x[nsamples];
    VGfloat y[nsamples];
    for (int i=0; i<nsamples; i++){
        p = data[i];
        x[i]=p.x;
        y[i]=p.y+yoffset;
    }
    Polyline(x,y,nsamples);
}

void graphInit(int* width, int* height){
    saveterm(); // Save current screen
    init(height,width); // Initialize display and get width and height
}

// wait for a specific key pressed
void waituntil(int endchar) {
    int key;
    for (;;) {
        key = getchar();
        if (key == endchar || key == '\n') {
            break;
        }
    }
}

void makeGraph(int width, int height, char *ch1Data, char *ch2Data,int xscale, int yscale, int ch1Offset, int ch2Offset,int plotPoints,int tChan, int tSlope, int sRate){
    int margin = 10; // Margin spacing around screen
    int xdivisions = 10; // Number of x-axis divisionsC
    int ydivisions = 8; // Number of y-axis divisions
    char str[100];

    VGfloat textcolor[4] = {0, 200, 200, 0.5}; // Color for displaying text
    VGfloat wave1color[4] = {0, 255, 0, 1}; // Color for displaying Channel 1 data
    VGfloat wave2color[4] = {255, 255, 0, 1}; // Color for displaying Channel 2 data

    data_point channel1_points[STREAM];
    data_point channel2_points[STREAM];

    int xstart = margin;
    int xlimit = width - 2*margin;
    int ystart = margin;
    int ylimit = height - 2*margin;
    Start(width, height);
    int yadj;
    int offFix;
    switch(yscale){     //select adjustment and offset fix for each y scale
        case 100:
            yadj=Y100;
            offFix=Y100OFF;
            break;
        case 500:
            yadj=Y500;
            offFix =Y500OFF;
            break;
        case 1000:
            yadj=Y1000;
            offFix=Y1000OFF;
            break;
        case 2000:
            yadj=Y2000;
            offFix=Y2000OFF;
            break;
        case 2500:
            yadj=Y2500;
            offFix=Y2500OFF;
            break;
        default:
        yadj=0;
    }
    int pixels_per_volt = ((ylimit-ystart)*yadj)/(ydivisions*yscale);
    drawBackground(width, height, xdivisions, ydivisions, margin);
    printScaleSettings(xscale, yscale, width-400, height-50, textcolor,tChan,tSlope,sRate);
    processSamples(ch1Data, plotPoints, margin, width-2*margin, pixels_per_volt, channel1_points);
    processSamples(ch2Data, plotPoints, margin, width-2*margin, pixels_per_volt, channel2_points);    
    // Display 2 waves
    plotWave(channel1_points, plotPoints, ch1Offset-(offFix), wave1color);
    plotWave(channel2_points, plotPoints, ch2Offset-(offFix), wave2color);
    End(); 
}
