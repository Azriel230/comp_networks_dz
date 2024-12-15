#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma warning(disable : 4996)

#define DEFAULT_BUFLEN 512
//#define DEFAULT_PORT "80" //27015

int __cdecl main(int argc, char** argv)
{
	WSADATA wsaData;
    SOCKET Socket = INVALID_SOCKET;  
    struct sockaddr_in SocketAddress;
    char buffer[DEFAULT_BUFLEN];
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    if ((Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf("socket error%d\n", WSAGetLastError());
        return 0;
    }

    struct ip_mreq m_group; //Multicast group
    m_group.imr_multiaddr.s_addr = inet_addr("230.168.11.11");
    m_group.imr_interface.s_addr = INADDR_ANY; 

    ZeroMemory(&SocketAddress, sizeof(SocketAddress));
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = htons(12345); //Функция htons преобразует u_short из узла в порядок байтов сети TCP/IP (который является большим порядком байтов).
    SocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    u_int flag = 1;
    if (setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(flag)) < 0) //reuse port and addr
    {
        printf("Ошибка установки опции SO_REUSEADDR\n");
        closesocket(Socket);
        WSACleanup();
        return 1;
    }

    if (bind(Socket, (struct sockaddr*)&SocketAddress, sizeof(SocketAddress)) == SOCKET_ERROR) {
        printf("Ошибка bind: %d\n", WSAGetLastError());
        closesocket(Socket);
        WSACleanup();
        return 1;
    }

    if (setsockopt(Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&m_group, sizeof(m_group)) == SOCKET_ERROR) //add client to group
    {
        printf("Ошибка setsockopt IP_ADD_MEMBERSHIP: %d\n", WSAGetLastError());
        closesocket(Socket);
        WSACleanup();
        return 1;
    }


    int size_SocketAddress = sizeof(SocketAddress);

    while (true)
    {
        iResult = recvfrom(Socket, (char*)buffer, DEFAULT_BUFLEN, 0, NULL, NULL);
        if (iResult > 0)
        {
            buffer[iResult] = '\0';
            std::cout << "Server: " << buffer << std::endl;
        }
        else
        {
            printf("recv error%d\n", WSAGetLastError());
        }
    }

    closesocket(Socket);
    WSACleanup();
	return 0;
}