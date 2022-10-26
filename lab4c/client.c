#include <WinSock2.h>
#include <stdio.h>
#include <string.h>
#define MAX_CHARS 200

// Store confirmation from server
char feedback[512];

char *get_cmd(){
    printf(">> ");
    char *cmd = (char *)malloc(MAX_CHARS);
    fgets(cmd, MAX_CHARS, stdin);
    return cmd;
}

int main(int argc, char *argv[])
{
	// Winsock DLL
	WSADATA Winsockdata;

    // Configure Server address
	struct sockaddr_in TCPServerAdd;
 	
	// Initiate Winsock DLL
	int iWsastartup = WSAStartup(MAKEWORD(2, 2), &Winsockdata);
	if (iWsastartup != 0)
	{
		printf("WSAStartup Failed\n");
		return 1;
	}

	// Fill the address structure
	TCPServerAdd.sin_family = AF_INET;
	TCPServerAdd.sin_addr.s_addr  = inet_addr("10.1.0.116");
	TCPServerAdd.sin_port = htons(8001);

	// Socket Creation
	int TCPClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(TCPClientSocket == INVALID_SOCKET)
	{
		printf("Client Socket Creation failed!\n");
		return 1;
	}

	// Connect
	int iConnect = connect(TCPClientSocket, (SOCKADDR *) &TCPServerAdd, sizeof(TCPServerAdd));
	if(iConnect == SOCKET_ERROR)
	{
		printf("Connection failed!\n");
		return 1;
	}

	// Get user input, send it to the server and receive feedback
	while(1){
        char* cmd = get_cmd();

		if(strcmp(cmd, "exit\n") == 0) break;
        if(strcmp(cmd, "\n") == 0) continue;

		// Send package to server
		int iSend = send(TCPClientSocket, cmd, strlen(cmd), 0);
		if(iSend == SOCKET_ERROR)
		{
			printf("Sending Data failed!\n");
			return 1;
		}

		// Receive data from server
		int iRecv = recv(TCPClientSocket, feedback, 512, 0);
		if (iRecv == SOCKET_ERROR)
		{
			printf("Feedback failed\n");
			return 1;
		}
		
		printf("%s\n", feedback);
		free(cmd);
	}

    // Close the client socket
	int iClose = closesocket(TCPClientSocket);

    // Winsock DLL cleanup
	int WsaCleanup = WSACleanup();

	return 0;
}
