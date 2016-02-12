#ifndef _POSCLIENT_H
#define _POSCLIENT_H

#include <SFML/Network.hpp>
#include <cassert>

class PosClient {
public:
	PosClient(const char * ip, unsigned short port) {
		socket = new sf::TcpSocket;
		sf::IpAddress addr(ip);

		std::cout << addr << ":" << port << std::endl;

		while(socket->connect(addr, port) != sf::Socket::Done) {
		    // NOOP, just wait
		}

		socket->setBlocking(true);
	}

	int setPosition(double x, double y, double z) {
		std::size_t to_receive = 0, received = 0;

		socket->send(&x, sizeof(double));
		socket->send(&y, sizeof(double));
		socket->send(&z, sizeof(double));

		char answer;

		while(received != sizeof(char))
			if(socket->receive(&answer, sizeof(char), received) == sf::Socket::Disconnected) {
				std::cout << "Pos server closed" << std::endl;
				std::terminate();
			}

		assert(received == sizeof(char));
		assert(answer == 1);

		return 0;
	}

private:
	sf::TcpSocket * socket;
};

#endif
