#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 65432
#define BUFFER_SIZE 1024

void handleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    while(true) {
        ZeroMemory(buffer, BUFFER_SIZE);
        bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);

        if(bytesReceived == SOCKET_ERROR) {
            std::cerr << "Errore nella ricezione dei dati: " << WSAGetLastError() << std::endl;
            break;
        }

        if(bytesReceived == 0) {
            std::cout << "Il client ha chiuso la connessione.\n";
            break;
        }

        std::cout << "Messaggio ricevuto dal client: " << buffer << std::endl;

        send(clientSocket, buffer, bytesReceived, 0);
    }
    
    closesocket(clientSocket);
}

int main(){
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (result != 0){
        std::cerr << "Errore WSAStartup: " << result << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket == INVALID_SOCKET){
        std::cerr << "Errore nella creazione del socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if(bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        std::cerr << "Errore nel bind: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if(listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Errore nella listen: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1; 
    }
    
    std::vector<std::thread> clientThreads;

    while(true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

        if(clientSocket == INVALID_SOCKET) {
            std::cerr << "Errore nell'accept: " << WSAGetLastError << std::endl;
            continue;
        }

        std::cout << "Client connesso.\n";

        clientThreads.emplace_back(std::thread(handleClient, clientSocket));
    }

    for(auto& t : clientThreads){
        if(t.joinable()){
            t.join();
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
