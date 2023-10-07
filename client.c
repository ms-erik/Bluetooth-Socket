#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    struct sockaddr_rc addr = {0};
    int s, status;
    char dest[18] = "B0:A4:60:5F:C1:60";

    // Allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Set the connection parameters
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t)1;
    str2ba(dest, &addr.rc_bdaddr);

    // Connect to the server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    // Send message
    if (status == 0) {
        printf("Welcome to Hangman\n");
        char guess[2];
        while (1) {
            // Receive info from server
            char message[1024];
            memset(message, 0, sizeof(message));

            // Receive data from the server
            status = recv(s, message, sizeof(message) - 1, 0);
            message[status] = '\0';

            if (status <= 0) {
                printf("Server disconnected\n");
                break;
            }

            // Display game info from server
            printf("%s", message);

            // Check if the game is over
            if (strstr(message, "Game Over") != NULL) {
                break;
            }

            // If the game is still going, prompt the user to guess a letter
            fgets(guess, sizeof(guess), stdin);
            guess[strcspn(guess, "\n")] = '\0';

            int c;
            while((c = getchar()) != '\n' && c != EOF) {}

            // Send the letter guess to the server
            send(s, guess, strlen(guess), 0);
            if (status < 0) {
                perror("Write error");
                break;
            }
        }
    } else {
        perror("Connection error");
    }

    // Close the socket
    close(s);

    return 0;
}

