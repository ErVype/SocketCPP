#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 65432
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "Errore WSAStartup: " << result << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET){
        std::cerr << "Errore nella creazione del socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        std::cerr << "Errore nella connessione al server: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connessione al server stabilita.\n";

    char buffer[BUFFER_SIZE];
    std::string message;

    while(true) {
        std::cout << "Inserire il messaggio da inviare (oppure 'exit' per terminare): ";
        std::getline(std::cin, message);
        
        if (message == "exit")
            break;
        
        send(clientSocket, message.c_str(), message.length(), 0);
        
        ZeroMemory(buffer, BUFFER_SIZE);

        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived > 0){
            std::cout << "Risposta del server: " << buffer << std::endl;
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
