#include "Window.h"

Window::Window(int x, int y, const char * name) {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

	m_window = new sf::RenderWindow(sf::VideoMode(x, y), name, sf::Style::Default, settings);

    settings = m_window->getSettings();

    std::cout << "DEPTH BITS: " << settings.depthBits << std::endl;
    std::cout << "STENCIL BITS: " << settings.stencilBits << std::endl;
    std::cout << "ANTIALIASING LEVEL: " << settings.antialiasingLevel << std::endl;
}

bool Window::open() {
	return m_window->isOpen();
}

std::vector<sf::Event> Window::pollEvents() {
	std::vector<sf::Event> events;
	sf::Event event;

	while(m_window->pollEvent(event))
		events.push_back(event);

	return events;
}

int Window::display() {
	m_window->display();

	return 0;
}

int Window::close() {
	m_window->close();

	return 0;
}

sf::Vector2u Window::getSize() {
	return m_window->getSize();
}
