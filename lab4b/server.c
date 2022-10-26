#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512 // Defines the buffer length
#define DEFAULT_PORT "8001" // Deifnes the port number


// Stores the clint socket information
struct threadinfo {
    SOCKET ClientSocket;
    int id;
};


// Handles data transfer to and from the client socket
DWORD WINAPI handleClient(LPVOID IpParameter) {
    
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

    // Sends the welcome message to the clientf
    iSendResult = send(ClientSocket, welcomeMessage, strlen(welcomeMessage), 0);
    if (iSendResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // Receives data from the client
    do {
        memset(recvbuf, 0, recvbuflen); //needed to fix buffer content
        // Receives data from client
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        
        // If data sent to the client sucessfully
        if (iResult > 0) {

            // Prints the data received from the client and disconnects the client if the receivd string is quit
            printf("Recieved: %s\n", recvbuf);
            if (strcmp(recvbuf, "quit") == 0) {
                printf("Disconnecting Client %d", id);
                break;
            }

            // Generates a random to produce a random string sent to the client
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
            
            
            // Checks a random number of times if the received buffer contains test and prints "Yes"
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
        // If the connection with the client is closed
        else if (iResult == 0) {
            printf("Connection closed\n");
        }
        // If sending data to the client fails
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    // Attemps to close the connection with the client and handles any errors tha may arise
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

    
    
    // Initiate Winsock DLL and handles errors
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // Define the ip address of the server
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Gets ip address information of the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

   // Creates a socket and handles any errors
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Binds the socket with the ip address details
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Free the current client ip address
    freeaddrinfo(result);

    // Configures the server to listen for any connection from a client
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("server is listening\n");

       
    // Loop infinitely and handle client sockets
    while (1)
    {    
        // Accept a connection with a client
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        // Creates a thread with the client socket data 
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