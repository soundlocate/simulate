#include "Server.h"


Server::Server(unsigned short port, std::function<void (sf::TcpSocket *)> onAcceptFunction) {
	listener.listen(port);

	std::thread([this, onAcceptFunction](){
			while(true) {
				sf::TcpSocket * socket = new sf::TcpSocket;
				listener.accept(*socket);

				onAcceptFunction(socket);

			    clients.push_back(socket);
			}
		}).detach();
}

void Server::close() {
	for(auto socket : clients) {
		socket->disconnect();
	}

	listener.close();
}
