#ifndef _POSCLIENT_H
#define _POSCLIENT_H

#include <SFML/Network.hpp>

class PosClient {
public:
	PosClient(const char * ip, unsigned short port) {
		socket = new sf::TcpSocket;
		sf::IpAddress addr = new sf::IpAddress(ip);

		while(socket->connect(addr, port) != sf::Socket::Done) {
		    // NOOP, just wait
		}
	}

	int setPosition(double x, double y, double z) {
		std::size_t to_receive = 0, received = 0;

		socket->send(x, sizeof(double));
		socket->send(y, sizeof(double));
		socket->send(z, sizeof(double));

		char answer;

		socket->receive(&answer, sizeof(char), received);

		assert(received == sizeof(char));
		assert(answer == 1);

		return 0;
	}

private:
	sf::TcpSocket * socket;
}

#endif
