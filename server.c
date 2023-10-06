#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_TRIES 6

char *wordList[] = {"invadir", "servidor", "bahia"};

char* getRandomWord(){
    srand(time(NULL));
    int randomIndex = rand() % (sizeof(wordList)/ sizeof(wordList[0]));
    return wordList[randomIndex];
}

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

  //seed the random number generator
  srand(time(NULL));

  char* wordToGuess = getRandomWord();
  int wordLength = strlen(wordToGuess);

  char* currentWordState = (char*)malloc(wordLength+1);
  memset(currentWordState, '_', wordLength);
  currentWordState[wordLength] = '\0';

  int tries =0;
  bool gameOver = false;

  /*
  ba2str(&rem_addr.rc_bdaddr, buf);
  fprintf(stderr, "accepted connection from %s\n", buf);
  memset(buf, 0, sizeof(buf));

  // read data from client
  while ((bytes_read = read(client, buf, sizeof(buf))) > 0) {
    printf("received %s\n", buf);
    memset(buf, 0, sizeof(buf));
  }
  */
  while(!gameOver){
    //send the current state of the word and the number of tries left
    sprintf(buf, "Word: %s\nTries Left: %d\n", currentWordState, MAX_TRIES - tries);
    send(client, buf, strlen(buf), 0);
    if(bytes_read <=0){
        printf("Client disconnected\n");
        break;
    }
    buf[bytes_read] = '\0';
    
    //check if the guessed letters are in the word
    char guess = buf[0];
    bool correctGuess = false;
    for(int i=0; i< wordLength; i++){
        if(wordToGuess[i] == guess && currentWordState[i] == '_'){
            currentWordState[i] == guess;
            correctGuess = true;
        }
    }
    if(!correctGuess){
        tries++;
    }
    //check if the game is gameOver 
    if(strcmp(currentWordState, wordToGuess)==0 || tries >= MAX_TRIES){
        gameOver = true;
    }
  }

  //send the final game result to the client
  if(strcmp(currentWordState, wordToGuess) ==0){
      strcpy(buf, "Congratulations, you won!\n");
  }else{
      strcpy(buf, "Game Over! the word was: ");
      strcat(buf, wordToGuess);
      strcat(buf, "\n");
  }

  send(client, buf, strlen(buf), 0);
  // close connection
  close(client);
  close(server_socket);
  free(currentWordState);
  return 0;
}
