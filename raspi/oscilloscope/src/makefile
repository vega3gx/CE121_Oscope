myscope: main.o cmdargs.o usbcomm.o graphics.o data.o
	gcc -I /opt/vc/include -I /usr/include -I/usr/include/libusb-1.0/ -lshapes -lm -lusb-1.0 -lwiringPi -o myscope main.o cmdargs.o usbcomm.o graphics.o data.o 

main.o: main.c scope.h
	gcc -c main.c -I /opt/vc/include -I /usr/include -I/usr/include/libusb-1.0/ -lshapes -lm -lusb-1.0 -lwiringPi

cmdargs.o: cmdargs.c scope.h
	gcc -c cmdargs.c -I /opt/vc/include -I /usr/include -I/usr/include/libusb-1.0/ -lshapes -lm -lusb-1.0

usbcomm.o: usbcomm.c scope.h
	gcc -c usbcomm.c -I /opt/vc/include -I /usr/include -I/usr/include/libusb-1.0/ -lshapes -lm -lusb-1.0

graphics.o: graphics.c scope.h
	gcc -c graphics.c -I /opt/vc/include -I /usr/include -I/usr/include/libusb-1.0/ -lshapes -lm -lusb-1.0

data.o: data.c scope.h
	gcc -c data.c -I /opt/vc/include -I /usr/include -I/usr/include/libusb-1.0/ -lshapes -lm -lusb-1.0

clean: 
	rm *.o 


    

