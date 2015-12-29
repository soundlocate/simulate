#include "Client.h"

Client::Client(const char * ip, unsigned short port) {
	m_socket = new sf::TcpListener();

	std::cout << "binding to " << ip << ":" << port << std::endl;

	m_socket->listen(port);

	std::thread([=](){
			while(true) {
				auto newSocket = new sf::TcpSocket();

				m_socket->accept(*newSocket);

				newSocket->setBlocking(false);

				std::cout << "client connected (for positions): " << newSocket->getRemoteAddress() << ":" << newSocket->getRemotePort() << std::endl;

				m_connections.push_back(newSocket);
			}
		}).detach();
}

std::vector<float> Client::getPoints() {
	unsigned int point_count = 0;
	std::size_t received, to_receive;
	double * receive_buffer;

	if(m_connections.size() <= 0)
		return m_points;

	auto client = m_connections[m_connections.size() - 1];

	if(client->receive(&point_count, sizeof(unsigned int), received) == sf::Socket::NotReady) {
//y		std::cout << "no new points :(" << std::endl;
		return m_points;
	} else {
		if(received != sizeof(unsigned int)) {
			std::cerr << "did not receive whole int, don't know what to do :(" << std::endl;
			std::cerr << sizeof(unsigned int) << " != " << received << std::endl;
			return m_points;
		}

		to_receive = sizeof(double) * 3 * point_count;
		receive_buffer = (double *) malloc(to_receive);

//		std::cout << "going to receive " << point_count << " points" << std::endl;

	    client->receive(receive_buffer, to_receive, received);

		if(received != to_receive) {
			std::cerr << "did not receive all data in one go :(\nBuild support for that!" << std::endl;

			return m_points;
		}

		m_points.erase(m_points.begin(), m_points.end());

		for (int i = 0; i < point_count; i++) {
			m_new_point(receive_buffer[3 * i], receive_buffer[3 * i + 1]);
		}

	}

	return std::vector<float>(m_points);
}

int Client::m_new_point(float x, float y) {
	m_points.push_back(x);
	m_points.push_back(-1000);
	m_points.push_back(1);
	m_points.push_back(0);
	m_points.push_back(1);
	m_points.push_back(0);

	m_points.push_back(x);
	m_points.push_back(1000);
	m_points.push_back(1);
	m_points.push_back(0);
	m_points.push_back(1);
	m_points.push_back(0);

	m_points.push_back(-1000);
	m_points.push_back(y);
	m_points.push_back(1);
	m_points.push_back(0);
	m_points.push_back(1);
	m_points.push_back(0);

	m_points.push_back(1000);
	m_points.push_back(y);
	m_points.push_back(1);
	m_points.push_back(0);
	m_points.push_back(1);
	m_points.push_back(0);

	return 0;
}
