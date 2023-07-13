#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
#elif _WIN32_WINNT < 0x0600
    #undef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
#endif

#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

/*
    ** -- KLASA SENDER TEMP -- **

    -> WYSYŁANIE DANYCH NA DANY PORT I IP <-

*/

class SENDER_TEMP{
    public:

    // ZMIENNE
    WSADATA DATA;
    WORD VER;
    sockaddr_in HINT;
    SOCKET SOCK;

    //////////////////////////////////////////////
    //////////////////////////////////////////////

    void _start(int port_number, string ip_number){
        _startupWinsock();
        _createHintStructure(port_number, ip_number);
        _send("message");
        _shutdown();
    }

    //////////////////////////////////////////////
    //////////////////////////////////////////////

    void _startupWinsock(){
        VER = MAKEWORD(2, 2);
        int wsResult = WSAStartup(VER, &DATA);
        SOCK = socket(AF_INET, SOCK_STREAM, 0);
    }

    void _createHintStructure(int port_number, string ip_number){
        HINT.sin_family = AF_INET;
        HINT.sin_port = htons(port_number);
        inet_pton(AF_INET, ip_number.c_str(), &HINT.sin_addr);

        int connResult = connect(SOCK, (sockaddr*)&HINT, sizeof(HINT));
        if (connResult == SOCKET_ERROR){
            cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
            closesocket(SOCK);
            WSACleanup();
        }
    }

    void _send(string message){
        char buf[4096];

        int sendResult = send(SOCK, message.c_str(), message.size() + 1, 0);
        
        if (sendResult != SOCKET_ERROR)
        {
            // Wait for response
            ZeroMemory(buf, 4096);
            int bytesReceived = recv(SOCK, buf, 4096, 0);
            if (bytesReceived > 0)
            {
               // cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
            }
        }
    }
    
    void _shutdown(){
        closesocket(SOCK);
        WSACleanup();
    }

};

// int main() {
//     SENDER_TEMP sender_temp;
//     sender_temp._startupWinsock();
//     sender_temp._createHintStructure(5100, "192.168.1.26");

//     while (true){
//         string input;
//         cout << ">: ";
//         getline(std::cin, input);
//         sender_temp._send(input);
//     }

//     sender_temp._shutdown();

// }