# Bluetooth-Socket
Implementing a Bluetooth socket in C
        Link for the video https://drive.google.com/drive/u/1/folders/1zBjldi3ISgL7iKhhF2b_ot-IWbrbkxTd

## Group members
        Pedro Arthur do Prado Françoso - 12547301
        Carlos Nery Ribeiro - 12547698
        Erik Melges - 12547399
        Fernando Clarindo Cristóvão - 12547573

## Setting up Bluetooth Address:
In order to implement the bluetooth socket, you'll need two computers with linux distributions. In our case, we used a Arch as server host and an ubunto as client.
Run the hciconfig command in terminal to see what is your bluetooth address, look for a line similar to "BD Address: XX:XX:XX:XX:XX:XX". Or for a more general approach, run bluetoothctl and type show, the controller is your Bluetooth address.
Now, in the client code you'll have to change the dest variable with your bluetooth address.

## Pairing server and client:
First of all, make sure your bluetooth is turned on; for that, in the server computer follow this steps:
- Open a terminal an run bluetoothctl;
- type power on;
- type agent on;
- type default-agent;
- type discoverable on;
- type exit;

Follow the same steps in your client computer till the discoverable part, then, instead type scan on to search for available devices. Now, on the client, use 'pair <server_address>' in the bluetoothctl to initiate pairing, note that you have to replace the <server_address> with the actual Bluetooth Address of the server. Be aware that you might need to enter a PIN code in both devices for confirmation.

## Initialize connection 
Now that everything has been set up, compile the server.c and client.c codes with the following command:

        gcc -o client client.c  -lbluetooth
        gcc -o server server.c  -lbluetooth

Perfect! Now you just have to run the server and client executables in the respective machines(./server and ./client) and have fun.

Remember to just type just one letter at a time.
