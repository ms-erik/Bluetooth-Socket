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
      printf("Welcome to Hangman\n");
      char guess[2];
      while(1){
          //receive info from server 
          char message[1024];
          memset(message, 0, sizeof(message)-1);
          status = recv(s, message, sizeof(message)-1,0);
          if(status <=0){
              printf("Server disconnected");
              break; 
          }
          //Display game info from server 
          printf("%s", message);

          //if gameOver, exit loop
          if(strstr(message, "Game Over") != NULL){
              break;
          }

          //if the game is stille going, prompt the user to guess a letter
          printf("Enter a letter guess: ");
          fgets(guess, sizeof(guess), stdin);

          //send the letter guess to the server 
          status = write(s, guess, strlen(guess));
          if(status < 0){
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
