#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
#elif _WIN32_WINNT < 0x0600
    #undef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
#endif

#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#include "RECEIVER/RECEIVER_TEMPLATE.cpp"
#include "SENDER/SENDER_TEMPLATE.cpp"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

HANDLE hConsole;

void clearScreen() {
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

int main(){

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    clearScreen();

    // Pobierz  NR PORTU
    string PORT;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    cout << "Podaj numer portu na ktorym bedziemy nasluchiwac: ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
    getline(cin, PORT);

    RECEIVER_TEMP receiver;
    thread receiverThread(&RECEIVER_TEMP::_start, &receiver, stoi(PORT));

    // Wykonuj dalej kod 
    receiverThread.detach();

    // Pobierz  IP
    string IP;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    cout << "Podaj numer IP, gdzie bedziemy wysylac dane: ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
    getline(cin, IP);
    // Pobierz  NR PORTU
    string PORT_;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    cout << "Podaj numer portu, gdzie bedziemy wysylac dane: ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
    getline(cin, PORT_);

    SENDER_TEMP sender;
    sender._startupWinsock();
    sender._createHintStructure(stoi(PORT_), IP);

    while (true){
        string input;
        cout << ">: ";
        getline(std::cin, input);
        sender._send(input);
    }

    sender._shutdown();

}