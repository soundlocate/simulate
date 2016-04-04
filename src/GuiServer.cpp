#include "GuiServer.h"

GuiServer::GuiServer(unsigned short port) {
	m_socket = new sf::TcpListener();

	std::cout << "binding to " << port << std::endl;

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

std::vector<float> GuiServer::getPoints() {
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
			// std::cerr << "did not receive whole int, don't know what to do :(" << std::endl;
			// std::cerr << sizeof(unsigned int) << " != " << received << std::endl;
			return m_points;
		}

		to_receive = sizeof(double) * 3 * point_count;
		receive_buffer = (double *) malloc(to_receive);

	    client->receive(receive_buffer, to_receive, received);

		if(received != to_receive) {
			// std::cerr << "did not receive all data in one go :(\nBuild support for that!" << std::endl;

			return m_points;
		}

		if(buffer != nullptr)
			free(buffer);

		buffer = receive_buffer;

		m_points.erase(m_points.begin(), m_points.end());

		for (int i = 0; i < point_count; i++) {
			v3 low = {0, 0, 1};
			v3 high = {0, 1, 0};
			v3 color = {0, 0, 0};

			for(int j = 0; j < 3; j++) {
				color.pos[j] = low.pos[j] + ((double) i / (double) (point_count - 1)) * (high.pos[j] - low.pos[j]);
			}

			m_new_point(receive_buffer[3 * i], receive_buffer[3 * i + 1], color);
		}
	}

	return std::vector<float>(m_points);
}

int GuiServer::m_new_point(float x, float y, v3 color) {
/*  m_points.push_back(x);
	m_points.push_back(-1000);
	m_points.push_back(1);
	m_points.push_back(color.x);
	m_points.push_back(color.y);
	m_points.push_back(color.z);

	m_points.push_back(x);
	m_points.push_back(1000);
	m_points.push_back(1);
	m_points.push_back(color.x);
	m_points.push_back(color.y);
	m_points.push_back(color.z);

	m_points.push_back(-1000);
	m_points.push_back(y);
	m_points.push_back(1);
	m_points.push_back(color.x);
	m_points.push_back(color.y);
	m_points.push_back(color.z);

	m_points.push_back(1000);
	m_points.push_back(y);
	m_points.push_back(1);
	m_points.push_back(color.x);
	m_points.push_back(color.y);
	m_points.push_back(color.z);
*/

	m_points.push_back(x);
	m_points.push_back(y);
	m_points.push_back(20);
	m_points.push_back(color.x);
	m_points.push_back(color.y);
	m_points.push_back(color.z);

	return 0;
}
