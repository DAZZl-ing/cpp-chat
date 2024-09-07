#include <iostream>
#include <stdio.h>
#include <thread>
#include <WinSock2.h>
#include <cstring>
#include <string>
#include <sstream>
using namespace std;

#pragma comment(lib, "ws2_32")

#define PORT 4578
#define PACKET_SIZE 1024
#define SERVER_IP "192.168.1.207"

void ReceiveMessages(SOCKET hSocket) {
    char cBuffer[PACKET_SIZE] = {};
    while (true) {
        int receivedBytes = recv(hSocket, cBuffer, PACKET_SIZE, 0);
        if (receivedBytes == SOCKET_ERROR) {
            std::cerr << "Error receiving data" << std::endl;
            break;
        }
        else if (receivedBytes == 0) {
            std::cout << "Server closed the connection" << std::endl;
            break;
        }
        else {
            cBuffer[receivedBytes] = '\0';
            std::cout << cBuffer << std::endl;
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET hSocket;
    hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN tAddr = {};
    tAddr.sin_family = AF_INET;
    tAddr.sin_port = htons(PORT);
    tAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));

    thread recvThread(ReceiveMessages, hSocket);

    string nick;
    cout << "닉네임을 입력하세요. : ";
    getline(cin, nick);

    string chat;

    char cMsg[1024];

    string currentnick = nick;

    while (true) {
        cout << nick << " : ";
        getline(cin, chat);
        strcpy(cMsg, chat.c_str());

        char message[1044];
        bool changeNickname = false;

        // /nickname 변경할 닉네임
        
        if (strncmp(cMsg, "/nickname", 9) == 0) {
            cout << "변경할 닉네임을 입력하세요 : ";
            string beforenick = nick;
            
            getline(cin, nick);

            if (!nick.empty()) {
                ostringstream oss;
                oss << "닉네임이 '" << beforenick << "' 에서 '" << nick << "'으로 변경되었습니다.";

                string change = oss.str();

                strcpy(message, change.c_str());

                changeNickname = true;
            }
        }

        if (!changeNickname) {
            int M = 0;
            for (int i = 0; i < strlen(nick.c_str()); i++) {
                message[M] = nick.c_str()[i];
                M++;
            }

            message[M] = ' ';
            M++;
            message[M] = ':';
            M++;
            message[M] = ' ';
            M++;

            for (int i = 0; i < strlen(cMsg); i++) {
                message[M] = cMsg[i];
                M++;
            }

            message[M] = '\0';
        }

        send(hSocket, message, strlen(message), 0);
    }

    closesocket(hSocket);

    WSACleanup();

    return 0;
}