#ifndef _CLIENT_H
#define _CLIENT_H

#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

class Client {
public:
	Client(const char * ip, unsigned short port);

	std::vector<float> getPoints();

	double * buffer = nullptr;
private:
	union v3 {
		struct {
			float x;
			float y;
			float z;
		};

		float pos[3];

		v3(float x, float y, float z) : x(x), y(y), z(z) {
		}
	};

	int m_new_point(float x, float y, v3 color);

	sf::TcpListener * m_socket;
	std::vector<sf::TcpSocket *> m_connections;
	std::vector<float> m_points;
	unsigned int defaultTimeout = 50;
};

#endif
