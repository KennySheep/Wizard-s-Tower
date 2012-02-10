#include "TCPserver.h"
#include <ws2tcpip.h>


GenTCPServerSocket::GenTCPServerSocket(char *port)
{
    struct addrinfo *result = NULL,
                    hints;
    int iResult, iSendResult;
    
     ListenSocket = INVALID_SOCKET;
     ClientSocket = INVALID_SOCKET;

     WSADATA wsaData;

	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	 if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
	 }	   

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, port, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
    }
	
    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        ServerDisconnect();
    }

    freeaddrinfo(result);
    
	printf("Server is up and running!\n");

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed: %d\n", WSAGetLastError());
		ServerDisconnect();
    }	
}

bool GenTCPServerSocket::AcceptConnection()
{
	if (ListenSocket == INVALID_SOCKET) {
        printf("Server is not in listening mode.\n");
		ServerDisconnect();
		return false;
	}

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
		ServerDisconnect();
		return false;
    }



	return true;
}

bool GenTCPServerSocket::SendData(char *buf, int length, int *sentLength)
{
	if (ClientSocket == INVALID_SOCKET) {
        printf("No client is attached. \n");
        return false;
	}
    *sentLength = send( ClientSocket, buf, length, 0 );
    if (*sentLength == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
		ClientDisconnect();
        return false;
    }

	return true;
}

bool GenTCPServerSocket::RecieveData(char *buf, int length, int *receivedLength)
{
	if (ClientSocket == INVALID_SOCKET) {
        printf("No client is attached. \n");
        return false;
	}
    *receivedLength = recv(ClientSocket, buf, length, 0);
	if (*receivedLength == 0) {
        printf("Connection closing...\n");
		ClientDisconnect();
		return false;
	}
    else  if (*receivedLength < 0) {
        printf("recv failed: %d\n", WSAGetLastError());
        ClientDisconnect();
        return false;
    }

	return true;

}


void GenTCPServerSocket::ClientDisconnect()
{
    closesocket(ClientSocket);
    WSACleanup();
}

void GenTCPServerSocket::ServerDisconnect()
{
    closesocket(ListenSocket);
    WSACleanup();
}