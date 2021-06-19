#ifndef UART_
#define UART_

/* for general */
#include <cstdint>


class Uart
{
public:
	Uart() {}
	~Uart() {}
	int32_t initialize(const char* device);
	void finalize();
	int32_t send(const char* buffer);
	int32_t recv(char* buffer, int32_t len);

private:
	int32_t fd;
};


#endif
