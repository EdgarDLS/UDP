#pragma once
#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>

class PlayerInfo
{
	int id;
	std::string name;
	sf::Vector2i position;
	sf::IpAddress ip;
	unsigned short port;

public:
	PlayerInfo(int id, sf::IpAddress ip, unsigned short port);
	~PlayerInfo();

	sf::IpAddress getIp() { return ip; }
	unsigned short getPort() { return port; }
};