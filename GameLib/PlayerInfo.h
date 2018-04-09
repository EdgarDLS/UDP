#pragma once
#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>

class PlayerInfo
{
	int id;
	std::string name;
	sf::IpAddress ip;
	unsigned short port;
	sf::Color color;

public:
	PlayerInfo(int id, sf::IpAddress ip, unsigned short port);
	~PlayerInfo();

	sf::Vector2i position;

	int getId() { return id; }
	sf::IpAddress getIp() { return ip; }
	unsigned short getPort() { return port; }
	int getPositionX() { return position.x; }
	int getPositionY() { return position.y; }
	int getRed() { return color.r; }
	int getGreen() { return color.g; }
	int getBlue() { return color.b; }
};