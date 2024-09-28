#include <iostream>
#include <sys/socket.h> // Socket functions
#include <netinet/in.h> // Internet address structures
#include <arpa/inet.h>  // IP address conversion functions
#include <unistd.h>     // POSIX operating system API, including socket closure
#include <cstring>      // string manipulation functions

using namespace std;

class SimpleServer
{
private:
    int serverSocket;              // Server socket file descriptor
    struct sockaddr_in serverAddr; // Server address structure
    string message;                // Message to send/receive
    bool running;                  // Server running status

public:
    // Constructor: Initializes the server socket and configures the address
    SimpleServer(int port) : running(true)
    {
        // Create a TCP socket (AF_INET: IPv4, SOCK_STREAM: TCP)
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0)
        {
            cerr << "Error establishing the server socket" << endl;
            exit(1);
        }

        // Configure server address: IPv4, bind to any interface, specify port
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port); // Convert port to network byte order
    }

    ~SimpleServer() { closeSocket(); }

    // Bind the server socket to the specified address and port
    void bindSocket()
    {
        if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            cerr << "Error binding the server socket" << endl;
            close(serverSocket);
            exit(1);
        }
    }

    // Start listening for incoming client connections
    void startListening()
    {
        if (listen(serverSocket, 5) < 0)
        {

            // Listen with a backlog of 5 connections
            cerr << "Error listening on the server socket" << endl;
            closeSocket(); // Properly close the socket before exiting
            exit(1);
        }
    }

    // Accept a client connection and handle communication
    void acceptConnection()
    {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        // Accept a client connection, return a new socket for communication
        int acceptedClientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (acceptedClientSocket < 0)
        {
            cerr << "Error accepting connection" << endl;
            closeSocket();
            exit(1);
        }
        cout << "Client connected" << endl;

        // Communication loop with the client
        char buffer[1024] = {0}; // Buffer for client data
        while (running)
        {
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer before reading

            int byteRead = recv(acceptedClientSocket, buffer, sizeof(buffer) - 1, 0);
            if (byteRead > 0)
            {
                buffer[byteRead] = '\0'; // Null-terminate the received data
                cout << buffer << endl;

                // Handle client request to close the connection
                if (strcmp(buffer, "quit()") == 0)
                {
                    cout << "Client requested to close the connection" << endl;
                    break;
                }

                // Get server response from user input and send to the client
                cout << "Enter response to client: ";
                getline(cin, message);

                send(acceptedClientSocket, message.c_str(), message.length(), 0);
            }
            else if (byteRead == 0)
            {
                cout << "Client disconnected" << endl;
                break;
            }
            else
            {
                cerr << "Error reading from client" << endl;
                break;
            }
        }

        // Close the client socket after communication ends
        if (acceptedClientSocket >= 0)
        {

            close(acceptedClientSocket);
            cout << "Client connection closed" << endl;
        }
    }

    bool isRunning() const
    {
        return running;
    }

    // Stop the server
    void stopServer()
    {
        running = false;
        cout << "Server stopped" << endl;
    }

    // Close the server socket
    void closeSocket()
    {
        if (serverSocket >= 0)
        {
            close(serverSocket);
            serverSocket = -1; // Mark the socket as closed
            cout << "Server socket closed" << endl;
        }
    }
};

int main(int argc, char *argv[])
{
    cout << "=== Step 1: Creating the server ===" << endl;
    SimpleServer server(9999); // Create a server object on port 9999

    cout << "=== Step 2: Binding the server socket ===" << endl;
    server.bindSocket(); // Bind the server socket to the address

    cout << "=== Step 3: Listening on the server socket ===" << endl;
    server.startListening(); // Start listening for incoming connections

    while (server.isRunning()) // Server runs indefinitely, accepting connections
    {
        cout << "=== Step 4: Accepting a client connection ===" << endl;
        server.acceptConnection();
    }

    server.stopServer();  // Stop the server
    server.closeSocket(); // Close the server socket
    return 0;
}

// compile the code using the following command:
// 1. g++ -o server server.cpp
// 2. ./server
// The server will start listening on port 9999 and accept incoming client connections. The server will receive messages from the client, display them, and send a response back to the client. The server will continue to run until the client sends the message "quit()" to close the connection. The server can be stopped by entering any input in the server terminal.