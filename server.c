#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRIES 6

char *wordList[] = {"invadir", "servidor", "bahia"};

char *getRandomWord() {
    int randomIndex = rand() % (sizeof(wordList) / sizeof(wordList[0]));
    return wordList[randomIndex];
}

int main() {
    struct sockaddr_rc loc_addr = {0}, rem_addr = {0};
    char buf[1024] = {0};
    int server_socket, client, bytes_read = 0; // Initialize bytes_read
    socklen_t opt = sizeof(rem_addr);

    // Allocate socket
    server_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Bind socket to port 1 of the first available local Bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t)1;
    if (bind(server_socket, (struct sockaddr *)&loc_addr, sizeof(loc_addr))) {
        perror("Bind failed\n");
        return 1;
    }

    // Put socket into listening mode
    listen(server_socket, 1);

    // Accept one connection
    client = accept(server_socket, (struct sockaddr *)&rem_addr, &opt);

    // Seed the random number generator
    srand(time(NULL));

    char *wordToGuess = getRandomWord();
    int wordLength = strlen(wordToGuess);

    char *currentWordState = (char *)malloc(wordLength + 1);
    memset(currentWordState, '_', wordLength);
    currentWordState[wordLength] = '\0';

    int tries = 0;
    bool gameOver = false;
    bool correctGuess = false;

    while (!gameOver) {
        // Send the current state of the word to the client
        sprintf(buf, "Word: %s\nTries Left: %d\nEnter a letter to guess: ", currentWordState, MAX_TRIES-tries);
        send(client, buf, strlen(buf), 0);
        // Receive a letter guess from the client
        memset(buf,0,sizeof(buf));
        bytes_read = recv(client, buf, sizeof(buf)-1, 0);
        buf[bytes_read] = '\0';

        // Check if the client disconnected
        if (bytes_read <= 0) {
            printf("Client disconnected\n");
            break;
        }

        // Extract the guessed letter
        char guess = buf[0];
        char aux = '_';
        correctGuess = false;

        // Check if the guessed letters are in the word
        for (int i = 0; i < wordLength; i++) {
            if (wordToGuess[i] == guess && currentWordState[i] == aux) {
                currentWordState[i] = guess;
                correctGuess = true;
            }
        }

        if (!correctGuess) {
            tries++;
        }

        // Check if the game is over
        if (strcmp(currentWordState, wordToGuess) == 0 || tries >= MAX_TRIES) {
            gameOver = true;
        }
    }

    // Send the final game result to the client
    if (strcmp(currentWordState, wordToGuess) == 0) {
        strcpy(buf, "Congratulations, you won!\n");
    } else {
        strcpy(buf, "Game Over! The word was: ");
        strcat(buf, wordToGuess);
        strcat(buf, "\n");
    }

    send(client, buf, strlen(buf), 0);

    // Close connection
    close(client);
    close(server_socket);
    free(currentWordState);
    return 0;
}

