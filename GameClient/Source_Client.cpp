#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <PacketType.h>

#define IP_SERVER "localhost"
#define PORT_SERVER 50000

int clientId = -1;;
sf::Vector2i clientPosition;
sf::UdpSocket sock;


struct Player
{
	int id;
	sf::Vector2i position;
	sf::Color color;
};

void CheckPacket(sf::Packet packet, std::vector<Player>* aPlayers)
{
	int packetType;

	packet >> packetType;

	if (packetType == PLAYER)
	{
		std::cout << "Añadiendo jugador..." << std::endl;

		// To check if any player is missing.
		// Mainly used for the latest players connecting to the server
		int totalPlayers = 0;

		// Create and store the player
		Player newPlayer;
		packet >> newPlayer.id;
		packet >> newPlayer.position.x;
		packet >> newPlayer.position.y;
		/*packet >> newPlayer.color.r;
		packet >> newPlayer.color.g;
		packet >> newPlayer.color.b;*/

		aPlayers->push_back(newPlayer);

		packet >> totalPlayers;

		if (totalPlayers != aPlayers->size())
		{
			std::cout << aPlayers->size() << std::endl;
			std::cout << "Faltan jugadores | Pidiendo informacion al servidor..." << std::endl;

			// Delete all the current players except the client
			// The client is always the first position
			for (int i = 1; i < aPlayers->size(); i++)
			{
				aPlayers->erase(aPlayers->begin() + i);
			}

			// Ask the players to the server
			sf::Packet resendPlayers;
			resendPlayers << RESEND;
			sock.send(resendPlayers, IP_SERVER, PORT_SERVER);
		}
	}

	else if (packetType == SYNC)
	{
		// Sync the missing players
		int blueColor = 0;
		Player newPlayer;
		packet >> newPlayer.id;
		packet >> newPlayer.position.x;
		packet >> newPlayer.position.y;
		/*packet >> newPlayer.color.r;
		packet >> newPlayer.color.g;
		packet >> newPlayer.color.b;*/

		aPlayers->push_back(newPlayer);

		std::cout << "Jugador faltante " << newPlayer.id << " añadido" << std::endl;

	}

	else if (packetType == POSITION)
	{
		int playerId = -1;
		int newPosX = -1;
		int newPosY = -1;
		
		packet >> playerId;
		packet >> newPosX;
		packet >> newPosY;

		for (int i = 0; i < aPlayers->size(); i++)
		{
			if (aPlayers->at(i).id == playerId)
			{
				aPlayers->at(i).position.x = newPosX;	
				aPlayers->at(i).position.y = newPosY;

				if (aPlayers->at(i).id == clientId)
				{
					clientPosition.x = newPosX;
					clientPosition.y = newPosY;
				}
			}
		}
	}

	else if (packetType == DISCONNECT)
	{
		// Remove the discconected player
		int removableId = -1;
		packet >> removableId;

		for (int i = 1; i < aPlayers->size(); i++)
		{
			if (aPlayers->at(i).id == removableId)
			{
				aPlayers->erase(aPlayers->begin() + i);
			}

		}

		std::cout << "Jugador desconectado | ID desconexion: " << removableId << std::endl;
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
				firstPacket >> clientId;
				firstPacket >> clientPosition.x;
				firstPacket >> clientPosition.y;

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
					sf::Packet movementPacket;
					int posX = clientPosition.x - 1;

					movementPacket << POSITION;
					movementPacket << posX;
					movementPacket << clientPosition.y;

					sock.send(movementPacket, IP_SERVER, PORT_SERVER);
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					sf::Packet movementPacket;
					int posX = clientPosition.x + 1;

					movementPacket << POSITION;
					movementPacket << posX;
					movementPacket << clientPosition.y;

					sock.send(movementPacket, IP_SERVER, PORT_SERVER);
				}
				else if (event.key.code == sf::Keyboard::Up)
				{
					sf::Packet movementPacket;
					int posY = clientPosition.y - 1;

					movementPacket << POSITION;
					movementPacket << clientPosition.x;
					movementPacket << posY;

					sock.send(movementPacket, IP_SERVER, PORT_SERVER);
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					sf::Packet movementPacket;
					int posY = clientPosition.y + 1;

					movementPacket << POSITION;
					movementPacket << clientPosition.x;
					movementPacket << posY;			

					sock.send(movementPacket, IP_SERVER, PORT_SERVER);
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
			rectAvatar.setPosition(sf::Vector2f(aPlayers[i].position.x, aPlayers[i].position.y));
			window.draw(rectAvatar);
		}

		window.display();
	}

	sf::Packet disconnect;
	disconnect << DISCONNECT;
	sock.send(disconnect, IP_SERVER, PORT_SERVER);
}

int main()
{
	std::vector<Player> aPlayers;	// List with all the players that are online
	sock.setBlocking(false);
	DibujaSFML(aPlayers);
	return 0;
}