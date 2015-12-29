#ifndef _CLIENT_H
#define _CLIENT_H

#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

class Client {
public:
	Client(const char * ip, unsigned short port);

	std::vector<float> getPoints();

private:
	int m_new_point(float x, float y);

	sf::TcpListener * m_socket;
	std::vector<sf::TcpSocket *> m_connections;
	std::vector<float> m_points;
	unsigned int defaultTimeout = 50;
};

#endif
