#ifndef _SERVER_H
#define _SERVER_H

class Server {
public:
	Server(unsigned short port, unsigned int listenerCount);
	int send(double [][8], unsigned int count);
private:
};

#endif
