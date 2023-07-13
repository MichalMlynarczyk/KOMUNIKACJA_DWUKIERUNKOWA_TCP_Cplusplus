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
    ** -- KLASA RECEIVER TEMP -- **

    -> ODBIERANIE DANYCH NA DANYM PORCIE <-

*/

class RECEIVER_TEMP{
    public:

    WSADATA WSDATA;
    WORD VER;
    SOCKET LISTENING;
    sockaddr_in HINT;
    sockaddr_in CLIENT;
    SOCKET CLIENTSOCKET;

    bool stopFlag = false;                      // DO ZAKOŃCZENIA DZIAŁANIE OBIEKTU

    //////////////////////////////////////////////
    //////////////////////////////////////////////

    void _start(int port_number){
        _startupWinsock();
        _createHintStructure(port_number);
        _receiveData();
        _shutdown();
    }

    void _stop(){
        stopFlag = true;                        // ZATRZYMANIE
    }

    //////////////////////////////////////////////
    //////////////////////////////////////////////

    void _startupWinsock(){
        VER = MAKEWORD(2, 2);

        int wsOk = WSAStartup(VER, &WSDATA);
        if (wsOk != 0){
            cerr << "Can't Initialize winsock! Quitting" << endl;
        }
    }

    void _createHintStructure(int port_number){
        LISTENING = socket(AF_INET, SOCK_STREAM, 0);
        if (LISTENING == INVALID_SOCKET){
            cerr << "Can't create a socket! Quitting" << endl;
        }

        HINT.sin_family = AF_INET;
        HINT.sin_port = htons(port_number);
        HINT.sin_addr.S_un.S_addr = INADDR_ANY; 

        bind(LISTENING, (sockaddr*)&HINT, sizeof(HINT));

        listen(LISTENING, SOMAXCONN);

        int clientSize = sizeof(CLIENT);

        CLIENTSOCKET = accept(LISTENING, (sockaddr*)&CLIENT, &clientSize);

        char host[NI_MAXHOST];		// Client's remote name
        char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

        ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
        ZeroMemory(service, NI_MAXSERV);

        if (getnameinfo((sockaddr*)&CLIENT, sizeof(CLIENT), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0){
           // cout << host << " connected on port " << service << endl;
        }
        else{
            inet_ntop(AF_INET, &CLIENT.sin_addr, host, NI_MAXHOST);
            cout << host << " connected on port " <<
                ntohs(CLIENT.sin_port) << endl;
        }

        closesocket(LISTENING);
    }

    void _receiveData(){
        char buf[4096];  

        while (true && !stopFlag)
        {
            ZeroMemory(buf, 4096);

            // Wait for client to send data
            int bytesReceived = recv(CLIENTSOCKET, buf, 4096, 0);
            if (bytesReceived == SOCKET_ERROR)
            {
                cerr << "Error in recv(). Quitting" << endl;
                break;
            }

            if (bytesReceived == 0)
            {
                cout << "Client disconnected " << endl;
                break;
            }

            clearCurrentLine();
            cout << "<: " << string(buf, 0, bytesReceived) << endl;
            cout << ">: ";

            // Echo message back to client
            send(CLIENTSOCKET, buf, bytesReceived + 1, 0);
        
        }
    }

    void _shutdown(){
        closesocket(CLIENTSOCKET);
        WSACleanup();
    }

    void clearCurrentLine() {
        std::cout << "\033[2K\r";
        std::cout.flush();
    }

};

// int main (){
//     RECEIVER_TEMP receiver;
//     receiver._start(5100);
// }