#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#define MAX_TRIES 6

char *wordList[] = {"pneumoultramicroscopicossilicovulcanoconiótico", "servidor", "banana", "cavalo"};

char *getRandomWord() {
    int randomIndex = rand() % (sizeof(wordList) / sizeof(wordList[0]));
    return wordList[randomIndex];
}

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void *handle_client(void *client) {
    int client_socket = *((int *)client);

    //buffer to get the messages
    char buf[1024] = {0};
    int bytes_read = 0;

    //get a random word from the list
    char *wordToGuess = getRandomWord();
    int wordLength = strlen(wordToGuess);

    char *currentWordState = (char *)malloc(wordLength + 1);
    //memset sets all the word as '_'
    memset(currentWordState, '_', wordLength);
    currentWordState[wordLength] = '\0';

    //Define auxiliar variables
    int tries = 0;
    bool gameOver = false;
    bool correctGuess = false;

    while (!gameOver) {
        // Send the current state of the word to the client
        sprintf(buf, "Word: %s\nTries Left: %d\nEnter a letter to guess: ", currentWordState, MAX_TRIES - tries);
        send(client_socket, buf, strlen(buf), 0);
        // Receive a letter guess from the client
        memset(buf, 0, sizeof(buf));
        bytes_read = recv(client_socket, buf, sizeof(buf) - 1, 0);
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

    send(client_socket, buf, strlen(buf), 0);

    // Close connection
    close(client_socket);

    // Free memory
    free(currentWordState);

    pthread_exit(NULL);
}

int main() {
    // Provides a seed value for rand
    srand(time(NULL));

    struct sockaddr_rc loc_addr = {0}, rem_addr = {0};
    int server_socket, client_socket, opt = 1;
    socklen_t client_addr_len;
    pthread_t thread;

    // Allocate socket/ fml/semantics/protocol
    server_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (server_socket == -1) {
        error("Socket creation failed");
    }

    // Bind socket to port 1 of the first available local Bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY; // Bluetooth address - indicates that any local Bluetooth adapter is acceptable
    loc_addr.rc_channel = (uint8_t)1; // Setting port number

    // Set socket option for reusing address and port
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        error("Setsockopt failed");
    }

    // Associates a local address with a socket
    if (bind(server_socket, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) == -1) {
        error("Bind failed");
    }

    // Put socket into listening mode
    if (listen(server_socket, 1) == -1) {
        error("Listen failed");
    }

    printf("Hangman server is waiting for connections...\n");

    while (1) {
        client_addr_len = sizeof(rem_addr);
        // Accept one connection
        client_socket = accept(server_socket, (struct sockaddr *)&rem_addr, &client_addr_len);
        if (client_socket == -1) {
            error("Accept failed");
        }

        printf("Accepted a connection\n");

        if (pthread_create(&thread, NULL, handle_client, (void *)&client_socket) != 0) {
            perror("Thread creation failed");
        }
    }

    close(server_socket);
    return 0;
}

