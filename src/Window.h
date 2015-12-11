#ifndef _WINDOW_H
#define _WINDOW_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <iostream>

class Window {
public:
	Window(int x, int y, const char * name);
	bool open();
	std::vector<sf::Event> pollEvents();
	int display();
	int close();
	sf::Vector2u getSize();
private:
	sf::RenderWindow * m_window = nullptr;
};

#endif
