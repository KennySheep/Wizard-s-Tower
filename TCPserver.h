#pragma comment(lib, "ws2_32.lib")
#pragma once
#include <winsock2.h>
#include <stdio.h>

class GenTCPServerSocket
{
   public:
      GenTCPServerSocket(char *port);

      ~GenTCPServerSocket() 
	  { 
		  ClientDisconnect(); 
		  ServerDisconnect(); 
	  }

      bool AcceptConnection();

      bool SendData(char *buf, int bufLength, int *sentLength);
      bool RecieveData(char *buf, int bufLength, int *receivedLength);

      void ClientDisconnect();
      void ServerDisconnect();

      bool isConnected() { return (ListenSocket != INVALID_SOCKET); }

      SOCKET GetServerSocket() { return ListenSocket; }
      SOCKET GetClientSocket() { return ClientSocket; }

   private:
      SOCKET ListenSocket;
      SOCKET ClientSocket;

};