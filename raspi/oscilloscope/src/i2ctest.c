//
// Raspberry Pi I2C Demo 
// Use with the I2Ctest project on PSoC-5.
// Connects to the I2C slave component in PSoC and performs reads and writes.

// A. Varma, 11/11/2018
//
// To compile, use the command
// gcc -o i2ctest i2ctest.c -lwiringPi
//
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <wiringPiI2C.h>

#define  I2C_SLAVE_ADDR 0x53 // 7-bit I2C address of PSoC

int main() 
  {
   
   int fd;
   unsigned int command = 1;
   unsigned int pot1_data, pot2_data;

   // Initialize the I2C interface.
   // It returns a standard file descriptor.
   fd = wiringPiI2CSetup(I2C_SLAVE_ADDR);
   if (fd == -1){
   perror("device not found\n");
   }

   while (1)
   {

    // Write command to PSoC
     wiringPiI2CWrite(fd, command);

    // Read potentiometer data
    pot1_data = wiringPiI2CRead(fd);
    pot2_data = wiringPiI2CRead(fd);

    printf("Potentiometer 1 data = %d Potentiometer 2 data = %d \n", pot1_data, pot2_data);
    command++;
    }
   }
    
