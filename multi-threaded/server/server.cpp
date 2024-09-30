// SimpleServer.cpp
// A C++ server program enabling simultaneous communication with multiple clients.

#include <iostream>
#include <sys/socket.h> // Socket functions
#include <netinet/in.h> // Internet address structures
#include <arpa/inet.h>  // IP address conversion functions
#include <unistd.h>     // POSIX API for closing sockets
#include <atomic>       // Atomic variables for thread-safe operations
#include <cstring>      // String manipulation functions
#include <mutex>        // Mutex for synchronizing access to shared resources
#include <thread>       // Multi-threading support
#include <vector>       // Dynamic array for managing client sockets

using namespace std;

class SimpleServer
{
private:
    int serverSocket;          // Server socket descriptor
    sockaddr_in serverAddr;    // Structure to hold server address information
    atomic<bool> running;      // Flag to control the server's running state
    vector<int> clientSockets; // List of connected client socket descriptors
    mutex clientsMutex;        // Mutex to protect access to clientSockets

public:
    /**
     * @brief Constructor to initialize the server with the specified port.
     *
     * @param port The port number on which the server will listen for connections.
     */
    SimpleServer(int port) : running(true)
    {
        // Create a TCP socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0)
        {
            cerr << "Failed to create socket." << endl;
            exit(EXIT_FAILURE);
        }

        // Configure server address structure
        serverAddr.sin_family = AF_INET;         // IPv4
        serverAddr.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
        serverAddr.sin_port = htons(port);       // Convert port to network byte order
    }

    /**
     * @brief Binds the server socket to the configured address and port.
     */
    void bindSocket()
    {
        if (::bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            cerr << "Failed to bind socket." << endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief Starts listening for incoming client connections.
     */
    void startListening()
    {
        if (listen(serverSocket, 5) < 0)
        { // Listen with a backlog of 5
            cerr << "Failed to listen on socket." << endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        cout << "Server listening on port " << ntohs(serverAddr.sin_port) << endl; // Display listening port
    }

    /**
     * @brief Handles communication with a connected client.
     *
     * @param clientSocket The socket descriptor for the connected client.
     */
    void handleClient(int clientSocket)
    {
        // Local flag to control this client's communication
        atomic<bool> clientRunning(true);

        /**
         * @brief Lambda function to receive messages from the client.
         *
         * @param socket The client's socket descriptor.
         */
        auto receiveMessages = [&](int socket)
        {
            char buffer[1024]; // Buffer to store incoming messages
            while (clientRunning)
            {
                memset(buffer, 0, sizeof(buffer));                           // Clear the buffer
                int bytesRead = recv(socket, buffer, sizeof(buffer) - 1, 0); // Receive data

                if (bytesRead > 0)
                {
                    buffer[bytesRead] = '\0';                                // Null-terminate the received string
                    cout << "Client [" << socket << "]: " << buffer << endl; // Display client message

                    // Check for exit commands
                    if (strcmp(buffer, "quit()") == 0 || strcmp(buffer, "exit()") == 0)
                    {
                        cout << "Client [" << socket << "] requested to close the connection." << endl;
                        clientRunning = false; // Stop communication loop
                        break;
                    }
                }
                else if (bytesRead == 0)
                {
                    cout << "Client [" << socket << "] disconnected." << endl;
                    clientRunning = false; // Stop communication loop
                    break;
                }
                else
                {
                    cerr << "Error reading from client [" << socket << "]." << endl;
                    clientRunning = false; // Stop communication loop
                    break;
                }
            }
        };

        /**
         * @brief Lambda function to send messages to the client.
         *
         * @param socket The client's socket descriptor.
         */
        auto sendMessages = [&](int socket)
        {
            string message;
            while (clientRunning)
            {
                cout << ">>> ";        // Prompt for server input
                getline(cin, message); // Read server input

                // Check for exit commands
                if (message == "quit()")
                {
                    clientRunning = false; // Stop communication loop
                }

                // Send the message to the client
                if (send(socket, message.c_str(), message.length(), 0) < 0)
                {
                    cerr << "Failed to send message to client [" << socket << "]." << endl;
                    clientRunning = false; // Stop communication loop
                    break;
                }

                // If exit command was sent, terminate the loop
                if (!clientRunning)
                {
                    break;
                }
            }
        };

        // Launch separate threads for sending and receiving messages concurrently
        thread recvThread(receiveMessages, clientSocket);
        thread sendThread(sendMessages, clientSocket);

        // Wait for both threads to finish execution
        recvThread.join();
        sendThread.join();

        // Close the client socket after communication ends
        close(clientSocket);
        cout << "Client [" << clientSocket << "] disconnected." << endl;
    };

    /**
     * @brief Continuously accepts incoming client connections and spawns a thread for each client.
     */
    void acceptConnections()
    {
        while (running)
        {
            cout << "Waiting for client connections..." << endl;
            sockaddr_in clientAddr;                   // Structure to hold client address
            socklen_t clientLen = sizeof(clientAddr); // Size of client address structure

            // Accept a new client connection
            int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientLen);
            if (clientSocket < 0)
            {
                cerr << "Error accepting client." << endl;
                continue;
            }

            // Display client connection details
            cout << "Client connected from " << inet_ntoa(clientAddr.sin_addr)
                 << ":" << ntohs(clientAddr.sin_port) << endl;

            // Add the new client socket to the list of active clients
            {
                lock_guard<mutex> lock(clientsMutex);
                clientSockets.push_back(clientSocket);
            }

            // Create a detached thread to handle client communication
            thread clientThread(&SimpleServer::handleClient, this, clientSocket);
            clientThread.detach();
        }
    }

    /**
     * @brief Shuts down the server by stopping the accept loop and closing the server socket.
     */
    void serverShutdown()
    {
        running = false;     // Stop the server loop
        close(serverSocket); // Close the server socket
        cout << "Server shutdown." << endl;
    }

    /**
     * @brief Destructor to ensure all client sockets and the server socket are closed.
     */
    ~SimpleServer()
    {
        for (int client : clientSockets)
        {
            close(client); // Close each client socket
        }
        close(serverSocket); // Close the server socket
    }
};

int main()
{
    SimpleServer server(9999); // Initialize server on port 9999

    server.bindSocket();        // Bind the server socket to the address
    server.startListening();    // Start listening for connections
    server.acceptConnections(); // Begin accepting client connections
    server.serverShutdown();    // Shutdown server (unreachable in current setup)

    return 0;
}
