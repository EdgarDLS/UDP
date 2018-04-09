#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <PacketType.h>

#define IP_SERVER "localhost"
#define PORT_SERVER 50000

int pos;
sf::UdpSocket sock;


struct Player
{
	int id;
	int posX;
	int posY;
	sf::Color color;
};

void CheckPacket(sf::Packet packet, std::vector<Player>* aPlayers)
{
	int packetType;

	packet >> packetType;

	if (packetType == PLAYER)
	{
		std::cout << "Añadiendo jugador..." << std::endl;
		// Create and store the player
		Player newPlayer;
		packet >> newPlayer.id;
		packet >> newPlayer.posX;
		packet >> newPlayer.posY;
		aPlayers->push_back(newPlayer);
	}
}

void DibujaSFML(std::vector<Player> aPlayers)
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Sin acumulación en cliente");

	// Create the first packet to say hello
	sf::Packet firstPacket;
	sf::IpAddress ipInitial;		// We always need an ip and port to receive even if we don´t need to use it later.
	unsigned short portInitial;
	bool connectionEstabished = false;

	
	sf::Clock clock;

	do
	{
		if (clock.getElapsedTime().asMilliseconds() > 1000)
		{
			firstPacket << HELLO;
			sock.send(firstPacket, IP_SERVER, PORT_SERVER);
			std::cout << "HELLO enviado" << std::endl;

			clock.restart();
		}

		// Receive the welcoming from the server
		sf::Socket::Status status = sock.receive(firstPacket, ipInitial, portInitial);

		if (status == sf::Socket::Done)
		{
			int packetType;
			firstPacket >> packetType;

			if (packetType == WELCOME)
			{
				int clientId;
				firstPacket >> clientId;

				std::cout << "Conexion con el servidor establecida. | ID: " << clientId << std::endl;
				connectionEstabished = true;
			}
		}
	} while (!connectionEstabished);


	while (window.isOpen())
	{
		sf::Event event;

		//Este primer WHILE es para controlar los eventos del mouse
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Left)
				{
					sf::Packet pckLeft;
					int posAux = pos - 1;
					pckLeft << posAux;
					sock.send(pckLeft, IP_SERVER, PORT_SERVER);
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					sf::Packet pckRight;
					int posAux = pos + 1;
					pckRight << posAux;
					sock.send(pckRight, IP_SERVER, PORT_SERVER);
				}
				break;

			default:
				break;

			}
		}

		sf::Packet pck;
		sf::IpAddress ipRem;
		unsigned short portRem;
		sf::Socket::Status status = sock.receive(pck, ipRem, portRem);

		if (status == sf::Socket::Done)
		{
			CheckPacket(pck, &aPlayers);

			/*pck >> pos;
			std::cout << "Recibo la confirmacion: " << pos << std::endl;*/
		}

		window.clear();

		sf::RectangleShape rectBlanco(sf::Vector2f(1, 600));
		rectBlanco.setFillColor(sf::Color::White);
		rectBlanco.setPosition(sf::Vector2f(200, 0));
		window.draw(rectBlanco);
		rectBlanco.setPosition(sf::Vector2f(600, 0));
		window.draw(rectBlanco);
		

		for (int i = 0; i < aPlayers.size(); i++)
		{
			sf::RectangleShape rectAvatar(sf::Vector2f(60, 60));
			rectAvatar.setFillColor(sf::Color::Green);
			rectAvatar.setPosition(sf::Vector2f(aPlayers[i].posX, aPlayers[i].posY));
			window.draw(rectAvatar);
		}	

		window.display();
	}
}

int main()
{
	std::vector<Player> aPlayers;	// List with all the players that are online
	pos = 200;
	sock.setBlocking(false);
	DibujaSFML(aPlayers);
	return 0;
}