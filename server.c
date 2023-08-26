#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  struct sockaddr_rc loc_addr = {0}, rem_addr = {0};
  char buf[1024] = {0};
  int server_socket, client, bytes_read;
  socklen_t opt = sizeof(rem_addr);

  // alocate socket
  server_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  // bind socket to port 1 of the first avaiable
  // local  bluetooth adapter
  loc_addr.rc_family = AF_BLUETOOTH;
  loc_addr.rc_bdaddr = *BDADDR_ANY;
  loc_addr.rc_channel = (uint8_t)1;
  if (bind(server_socket, (struct sockaddr *)&loc_addr, sizeof(loc_addr))) {
    perror("Bind falied\n");
    return 1;
  }

  // put socket into listening mode
  listen(server_socket, 1);

  // acept one connection
  client = accept(server_socket, (struct sockaddr *)&rem_addr, &opt);

  ba2str(&rem_addr.rc_bdaddr, buf);
  fprintf(stderr, "accepted connection from %s\n", buf);
  memset(buf, 0, sizeof(buf));

  // read data from client
  while ((bytes_read = read(client, buf, sizeof(buf))) > 0) {
    printf("received %s\n", buf);
    memset(buf, 0, sizeof(buf));
  }
  // close connection
  close(client);
  close(server_socket);
  return 0;
}
