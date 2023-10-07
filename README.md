# Bluetooth Socket Implementation in C

**Video Tutorial Link:** [Video Tutorial](https://drive.google.com/drive/folders/1zBjldi3ISgL7iKhhF2b_ot-IWbrbkxTd?usp=drive_link)

## Group Members
- Pedro Arthur do Prado Françoso - 12547301
- Carlos Nery Ribeiro - 12547698
- Erik Melges - 12547399
- Fernando Clarindo Cristóvão - 12547573

## General Information
- Linux Distribution: Arch Linux
- Compiler: GCC (GNU Compiler Collection) 13.2.1 (20230801)

## Setting Up Bluetooth Address
To implement the Bluetooth socket, you'll need two computers running Linux distributions. In our case, we used Arch Linux as the server host and Ubuntu as the client. Follow these steps:

1. Run the `hciconfig` command in the terminal to find your Bluetooth address. Look for a line similar to "BD Address: XX:XX:XX:XX:XX:XX."

   Alternatively, for a more general approach, run `bluetoothctl` and type `show`. The controller listed is your Bluetooth address.

2. In the client code, replace the `dest` variable with your Bluetooth address.

## Pairing the Server and Client
Before pairing the server and client, ensure that Bluetooth is turned on. Follow these steps on the server computer:

1. Open a terminal and run `bluetoothctl`.
2. Type `power on`.
3. Type `agent on`.
4. Type `default-agent`.
5. Type `discoverable on`.
6. Type `exit`.

Repeat the same steps on your client computer until the "discoverable" part. For the client, instead of "discoverable on," use `scan on` to search for available devices. On the client, initiate pairing with the server using the command 'pair <server_address>' in `bluetoothctl`. Replace `<server_address>` with the actual Bluetooth Address of the server. Note that you might need to enter a PIN code on both devices for confirmation.

## Initializing the Connection
Now that everything has been set up, compile the `server.c` and `client.c` codes with the following commands:

      gcc -o client client.c -lbluetooth
      gcc -o server server.c -lbluetooth

Once compiled, run the server and client executables on their respective machines (./server and ./client) and enjoy your Bluetooth socket connection.  

Remember to input one letter at a time.
