#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <clocale>
#include <iostream>


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "80"

int __cdecl main(int argc, char** argv)
{
    system("chcp 65001");
    WSADATA wsaData; //структурка для инициализации
    SOCKET ConnectSocket = INVALID_SOCKET; //сокет для подключения
    struct addrinfo* result = NULL, //для хранения сведений об адресе узла
        * ptr = NULL, 
        hints; //информация об адресе узла

    const char* sendbuf = "GET / HTTP/1.1\r\nHost: pmk.tversu.ru\r\nConnection: close\r\n\r\n";
    char recvbuf[DEFAULT_BUFLEN]; //получаем результат запроса
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); //инициализирует все для работы с сокетами
    if (iResult != 0) 
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC; //тип Ip протокола 
    hints.ai_socktype = SOCK_STREAM; //для получения http
    hints.ai_protocol = IPPROTO_TCP; //тип протокола сокета

    iResult = getaddrinfo("pmk.tversu.ru", DEFAULT_PORT, &hints, &result); //получение инфы с сервера
    if (iResult != 0) 
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
    {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0); //посылае запрос (GET)
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    iResult = shutdown(ConnectSocket, SD_SEND); //отключает отправку или получение в сокете
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }


    do {
        iResult = recv(ConnectSocket, recvbuf, sizeof(recvbuf), 0); //получаем результат (HTML)
        if (iResult > 0) {

            printf("%s", recvbuf);
        }
        else if (iResult == 0) {
            printf("Connection closed\n");
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
        }
    } while (iResult > 0);

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}