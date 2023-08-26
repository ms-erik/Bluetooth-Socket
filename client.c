#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  struct sockaddr_rc addr = {0};
  int s, status;
  char dest[18] = "B0:A4:60:5F:C1:60";

  // alocate a socket
  s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  // set the connection parameters
  addr.rc_family = AF_BLUETOOTH;
  addr.rc_channel = (uint8_t)1;
  str2ba(dest, &addr.rc_bdaddr);

  // connect to server
  status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

  // send message
  if (status == 0) {
    // status = write(s, "hello!", 6);
    printf("Connected to server\n");
    char message[1024];
    while (1) {
      printf("Entere a message: ");
      fgets(message, sizeof(message), stdin);
      if (strcmp(message, "exit\n") == 0) {
        break;
      }
      status = write(s, message, strlen(message));
      if (status < 0) {
        perror("Write error");
        break;
      }
    }
  } else {
    perror("Connection error");
  }
  close(s);
  return 0;
}
