/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdlib>
#include <cstring>

/* for device driver */
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <iostream>
#endif

#include "Uart.h"

/*** Macro ***/


/*** Function ***/
/* ref. https://www.programmersought.com/article/96355875673/ */
int32_t Uart::initialize(const char* device)
{
#ifdef _WIN32
	return 0;
#else
	fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		printf("[ERR] open\n");
		return -1;
	}

	if( (fcntl(fd, F_SETFL, 0)) < 0 ) {
		printf("[ERR] fcntl\n");
		close(fd);
		return -1;
	}

	termios stNew, stOld;
	if(tcgetattr(fd, &stOld) != 0) {
		printf("[ERR] tcgetattr\n");
		close(fd);
		return -1; 
	} 

	stNew = stOld;
	cfmakeraw(&stNew); 

	cfsetispeed(&stNew, B115200); 
	cfsetospeed(&stNew, B115200); 

	stNew.c_cflag |= (CLOCAL|CREAD);
	stNew.c_cflag &= ~CSIZE;
	stNew.c_cflag |= CS8;
	
	stNew.c_cflag &= ~PARENB;
	stNew.c_iflag &= ~INPCK;

	stNew.c_cflag &= ~CSTOPB;
	stNew.c_cc[VTIME]=0;
	stNew.c_cc[VMIN]=1;

	tcflush(fd, TCIFLUSH);
	if( tcsetattr(fd,TCSANOW, &stNew) != 0 ) {
		printf("[ERR] tcsetattr\n");
		close(fd);
		return -1;
	}
	return 0;
#endif
} 

void Uart::finalize()
{
#ifdef _WIN32
	return;
#else
	close(fd);
#endif
}

int32_t Uart::send(const char* buffer)
{
#ifdef _WIN32
	return 0;
#else
	int32_t len = strlen(buffer); 
	return write(fd, buffer, len); 
#endif
}

int32_t Uart::recv(char* buffer, int32_t len)
{
#ifdef _WIN32
	return 0;
#else
	return read(fd, buffer, len);
#endif
}

