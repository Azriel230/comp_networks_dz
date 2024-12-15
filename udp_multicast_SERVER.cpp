#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_PORT "80"
#pragma warning(disable : 4996)

int __cdecl main(void)
{
    WSADATA wsaData;
    SOCKET Socket = INVALID_SOCKET;
    struct sockaddr_in SocketAddress;
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

    ZeroMemory(&SocketAddress, sizeof(SocketAddress));
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = htons(12345); //Функция htons преобразует u_short из узла в порядок байтов сети TCP/IP (который является большим порядком байтов).
    SocketAddress.sin_addr.s_addr = inet_addr("230.168.11.11"); //fake-ip for subscribe 

    u_int ttl = 1;

    if (setsockopt(Socket, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&ttl, sizeof(ttl)) == SOCKET_ERROR)
    {
        printf("Ошибка setsockopt IP_MULTICAST_TTL: %d\n", WSAGetLastError());
        closesocket(Socket);
        WSACleanup();
        return 1;
    }

    int size_SocketAddress = sizeof(SocketAddress);

    char a[] = "Hello guys!";

    while (true)
    {
        sendto(Socket, a, strlen(a), 0, (SOCKADDR*)&SocketAddress, size_SocketAddress);
        Sleep(1000);
    }

    WSACleanup();

    return 0;
}