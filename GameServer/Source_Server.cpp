
#include <SFML\Network.hpp>
#include <iostream>
#include <PlayerInfo.h>
#include <PacketType.h>

int main()
{
	std::vector<PlayerInfo> aPlayers;	// Players that "connected" to the server.
	sf::UdpSocket sock;
	sf::Socket::Status status = sock.bind(50000);

	if (status != sf::Socket::Done)
	{
		std::cout << "Error al vincular\n";
		system("pause");
		exit(0);
	}

	sf::Clock clock;

	do
	{
		if (clock.getElapsedTime().asMilliseconds() > 200)
		{
			sf::Packet pck;
			sf::IpAddress ipRem; unsigned short portRem;
			status = sock.receive(pck, ipRem, portRem);

			if (status == sf::Socket::Done)
			{
				int packetType;
				pck >> packetType;

				if (packetType == HELLO)
				{
					std::cout << "Nueva conexion con cliente. | Guardando jugador..." << std::endl;
					aPlayers.push_back(PlayerInfo(aPlayers.size(), ipRem, portRem));
					std::cout << "Jugador guardado | ID - " << aPlayers.size() - 1 << " | IP - " << aPlayers[aPlayers.size()-1].getIp() << " | PORT - " << aPlayers[aPlayers.size()-1].getPort() << std::endl;

					int id = aPlayers.size() - 1;

					sf::Packet pckSend;
					pckSend << WELCOME;
					pckSend << id;
					
					sock.send(pckSend, ipRem, portRem);

					for (int i = 0; i < aPlayers.size(); i++)
					{
						sf::Packet pckSend;
						pckSend << PLAYER;
						pckSend << id;
						pckSend << 200 * id;
						pckSend << 280;

						sock.send(pckSend, aPlayers[i].getIp(), aPlayers[i].getPort());
					}
				}

				/*
				int pos;
				pck >> pos;

				if (pos == -1)
				{
				break;
				}

				std::cout << "Se intenta la pos " << pos << std::endl;

				if (pos >= 200 && pos <= 600)
				{
				std::cout << "Se confirma la pos " << pos << std::endl;
				sf::Packet pckSend;
				pckSend << pos;
				sock.send(pckSend, ipRem, portRem);

				}
				*/
			}
			clock.restart();
		}
	} while (true);

	sock.unbind();
	return 0;
}