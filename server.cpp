#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORTA 8080

int main() {
  int meuSocket, clienteSocket;
  sockaddr_in addr, clienteAddr;
  socklen_t clienteAddrLen = sizeof(clienteAddr);

  meuSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (meuSocket == -1) {
    perror("Socket creation failed\n");
    return 1;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORTA);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(meuSocket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("Bind failed\n");
    return 1;
  }

  if (listen(meuSocket, 5) == -1) {
    perror("Listen failed\n");
    return 1;
  }

  printf("Server is listening for connections...\n");

  clienteSocket =
      accept(meuSocket, (struct sockaddr *)&clienteAddr, &clienteAddrLen);
  if (clienteSocket == -1) {
    perror("Accept failed\n");
    return 1;
  }

  printf("Connection accepted from %s:%d\n", inet_ntoa(clienteAddr.sin_addr),
         ntohs(clienteAddr.sin_port));

  // Now you can use clienteSocket to communicate with the client

  // Close the client socket when done
  close(clienteSocket);

  // Close the server socket
  close(meuSocket);

  return 0;
}
