#ifndef _SERVER_H
#define _SERVER_H

#include <SFML/Network.hpp>
#include <thread>

class Server {
public:
	Server(unsigned short port, std::function<void (sf::TcpSocket *)> onAcceptFunction);

	template <typename T>
	int send(T * data, unsigned int count);
private:
	sf::TcpListener listener;
	std::vector<sf::TcpSocket *> clients;
};

template <typename T>
int Server::send(T * data, unsigned int count) {
	int i = 0;

	for(auto client : clients) {
		if(client->send(data, sizeof(T) * count) != sf::Socket::Done) {
			clients.erase(clients.begin() + i);
		}

		i++;
	}

	return 0;
}

#endif
