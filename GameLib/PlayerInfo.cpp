#include <SFML\Network.hpp>
#include "PlayerInfo.h"


PlayerInfo::PlayerInfo(int _id, sf::IpAddress _ip, unsigned short _port)
{
	id = _id;
	name = "test";
	position.x = 0 + (rand() % static_cast<int>(100 - 0 + 1));
	position.y = 0 + (rand() % static_cast<int>(100 - 0 + 1));
	ip = _ip;
	port = _port;
}

PlayerInfo::~PlayerInfo()
{

}