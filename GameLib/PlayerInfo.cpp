#include <SFML\Network.hpp>
#include "PlayerInfo.h"


PlayerInfo::PlayerInfo(int _id, sf::IpAddress _ip, unsigned short _port)
{
	id = _id;
	name = "test";
	position.x = (rand() % 341) + 200;
	position.y = (rand() % 541) + 0;
	ip = _ip;
	port = _port;
	color = sf::Color(rand() % 255, rand() % 255, rand() % 255);
}

PlayerInfo::~PlayerInfo()
{

}