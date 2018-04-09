
#include <SFML\Network.hpp>
#include <iostream>
#include <PlayerInfo.h>
#include <PacketType.h>

int playersId = -1;

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
					playersId++;
					aPlayers.push_back(PlayerInfo(playersId, ipRem, portRem));

					std::cout << "Jugador guardado | ID - " << playersId << " | IP - " << aPlayers[aPlayers.size() - 1].getIp() << " | PORT - " << aPlayers[aPlayers.size() - 1].getPort() << std::endl;

					sf::Packet pckSend;
					pckSend << WELCOME;
					pckSend << playersId;
					pckSend << aPlayers[aPlayers.size() - 1].getPositionX();
					pckSend << aPlayers[aPlayers.size() - 1].getPositionY();

					sock.send(pckSend, ipRem, portRem);

					int playersSize = aPlayers.size();
					for (int i = 0; i < playersSize; i++)
					{
						sf::Packet pckSend;
						pckSend << PLAYER;
						pckSend << aPlayers[playersSize - 1].getId();;
						pckSend << aPlayers[playersSize - 1].getPositionX();
						pckSend << aPlayers[playersSize - 1].getPositionY();
						/*pckSend << color;
						pckSend << color;
						pckSend << color;*/

						pckSend << playersSize;	// To check if the client has all the players

						sock.send(pckSend, aPlayers[i].getIp(), aPlayers[i].getPort());
					}
				}

				// Sync the missing players
				else if (packetType == RESEND)
				{
					for (int i = 0; i < aPlayers.size(); i++)
					{
						if (aPlayers[i].getPort() != portRem)
						{
							sf::Packet pckSend;
							pckSend << SYNC;
							pckSend << aPlayers[i].getId();
							pckSend << aPlayers[i].getPositionX();
							pckSend << aPlayers[i].getPositionY();
							/*pckSend << color;
							pckSend << color;
							pckSend << color;*/

							sock.send(pckSend, ipRem, portRem);
						}
					}
				}

				else if (packetType == DISCONNECT)
				{
					std::cout << "Jugador desconectado | Eliminando jugador..." << std::endl;

					for (int i = 0; i < aPlayers.size(); i++)
					{
						if (aPlayers[i].getPort() == portRem)
						{
							sf::Packet pckSend;
							int id = aPlayers[i].getId();
							pckSend << DISCONNECT;
							pckSend << id;
							aPlayers.erase(aPlayers.begin() + i);

							for (int j = 0; j < aPlayers.size(); j++)
								sock.send(pckSend, aPlayers[j].getIp(), aPlayers[j].getPort());

							std::cout << "Jugador con ID " << id << " eliminado." << std::endl;
							break;
						}
					}
				}

				else if (packetType == POSITION)
				{
					int posX;
					int posY;

					pck >> posX;
					pck >> posY;

					for (int i = 0; i < aPlayers.size(); i++)
					{
						if (aPlayers[i].getPort() == portRem)
						{
							if ((posX >= 200 && posX <= 600) && (posY >= 0 && posY <= 540))
							{
								std::cout << "Jugador " << aPlayers[i].getId() << " puede moverse" << std::endl;

								aPlayers[i].position.x = posX;
								aPlayers[i].position.y = posY;

								sf::Packet pckSend;
								pckSend << POSITION;
								pckSend << aPlayers[i].getId();
								pckSend << aPlayers[i].position.x;
								pckSend << aPlayers[i].position.y;

								for (int j = 0; j < aPlayers.size(); j++)
									sock.send(pckSend, aPlayers[j].getIp(), aPlayers[j].getPort());

								break;
							}
						}
					}
				}
			}
			clock.restart();
		}
	} while (true);

	sock.unbind();
	return 0;
}