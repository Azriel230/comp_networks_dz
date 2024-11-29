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

    if ((Socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        printf("socket error%d\n", WSAGetLastError());
        return 0;
    }

    ZeroMemory(&SocketAddress, sizeof(SocketAddress));
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = htons(12345); //‘ункци€ htons преобразует u_short из узла в пор€док байтов сети TCP/IP (который €вл€етс€ большим пор€дком байтов).
    SocketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    std::cout << "Enter a line: ";
    std::cin >> buffer;
    std::cout << std::endl;

    iResult = sendto(Socket, (const char*)buffer, strlen(buffer), 0, (SOCKADDR*)&SocketAddress, sizeof(SocketAddress));
    if (iResult < 0) {
        printf("send error %d\n", WSAGetLastError());
    }

    std::cout << "Sent: " << buffer << std::endl;

    int size_SocketAddress = sizeof(SocketAddress);
    iResult = recvfrom(Socket, (char*)buffer, DEFAULT_BUFLEN, 0, (SOCKADDR*)&SocketAddress, &size_SocketAddress);
    if (iResult > 0) {
        buffer[iResult] = '\0';
        std::cout << "Server: " << buffer << std::endl;
    }
    else
    {
        printf("recv error%d\n", WSAGetLastError());
    }

    closesocket(Socket);
    WSACleanup();
	return 0;
}