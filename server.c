#define WIN32_LEAN_AND_MEAN

// Add a comment explaining why each library is needed

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

//Add a comment explaining what each #define line is used for
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8001"


//Add a comment explaining what this struct is used for
struct threadinfo {
    SOCKET ClientSocket;
    int id;
};



//Add a comment explaining what this function does
DWORD WINAPI handleClient(LPVOID IpParameter) {
    
    //Add a comment indicating what each variable is used for
    struct threadinfo* temp = (struct threadinfo*)IpParameter;
    int iSendResult;
    int iResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    SOCKET ClientSocket = temp->ClientSocket;
    int id = temp->id;
    int random;
    char response[DEFAULT_BUFLEN] = "!quit!"; 

    printf("Connecting Client %d.\n", id);
    char welcomeMessage[DEFAULT_BUFLEN];
    sprintf_s(welcomeMessage, sizeof(welcomeMessage), "Client %d, Thank you for connecting", id);

    //Add a comment describing what the next 8 lines of code do
    iSendResult = send(ClientSocket, welcomeMessage, strlen(welcomeMessage), 0);
    if (iSendResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    //Add a comment describing what this loop does
    do {
        memset(recvbuf, 0, recvbuflen); //needed to fix buffer content
        //What does the following line do?
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        
        //When does this if statement trigger?
        if (iResult > 0) {

            //Describe the following 5 lines of code.
            printf("Recieved: %s\n", recvbuf);
            if (strcmp(recvbuf, "quit") == 0) {
                printf("Disconnecting Client %d", id);
                break;
            }

            //Describe the following 12 lines of code.
            random = 2 + (rand() % 9); 
            char randomstr[3];
            sprintf_s(randomstr, 3, "%d", random);
            printf("random %s\n", randomstr);
            Sleep(100);
            iResult = send(ClientSocket, randomstr, strlen(randomstr), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            
            
            //Describe the following 14 lines of code
            for (int i = 0; i < random; i++) {
                if (strcmp(recvbuf, "test\n") == 0) {
                    printf("Yes");
                }
                Sleep(100);
                iSendResult = send(ClientSocket, recvbuf, strlen(recvbuf), 0);

                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                    return 1;
                }
            }
            
            
        }
        //Under what circumstances does this branch trigger?
        else if (iResult == 0) {
            printf("Connection closed\n");
        }
        //Describe what this branch does. Under what circumstances does this branch trigger? 
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    //Describe the following 8 lines of code
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    closesocket(ClientSocket);


    return 0;
}

int main(void)
{

    //Describe what each variable does
    int id = 0;
    WSADATA wsaData;
    int iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL;
    struct addrinfo hints;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    
    
    //Describe what the following 5 lines of code do.
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    //Describe what the following 5 lines of code do
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Describe what the following 6 ines of code do
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

   //Describe what the following 7 lines of code do
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //Describe what the following 8 lines of code do
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    //Why is this line needed?
    freeaddrinfo(result);

    //Describe what the following 7 lines of code do
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("server is listening\n");

       
    //What does this loop do?
    while (1)
    {    
        //Describe what the following 7 lines of code do
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        //Describe what the following 12 lines of code do
        HANDLE handle;
        struct threadinfo temp;
        temp.ClientSocket = ClientSocket;
        temp.id = id;
        handle = CreateThread(NULL, 0, handleClient, &temp, 0, NULL);
        if (handle == NULL) {
            printf("Thread was unable to be made\n");
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
        id += 1;
    }
}