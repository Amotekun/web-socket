// SimpleClient.cpp
// A C++ client program enabling simultaneous sending and receiving of messages to a server.

#include <iostream>
#include <cstring>
#include <atomic>
#include <sys/socket.h> // Socket functions
#include <netinet/in.h> // Internet address structures
#include <arpa/inet.h>  // IP address conversion functions
#include <unistd.h>     // POSIX API for closing sockets
#include <thread>       // Multi-threading support

using namespace std;

class SimpleClient
{
private:
    int clientSocket;       // Socket descriptor for the client
    sockaddr_in serverAddr; // Structure to hold server address information
    string serverIp;        // Server IP address
    int port;               // Server port number
    atomic<bool> running;   // Atomic flag to control the communication loop

public:
    /**
     * @brief Constructor to initialize client with server IP and port.
     *
     * @param serverIp IP address of the server to connect to.
     * @param port Port number of the server.
     */
    SimpleClient(const string &serverIp, int port)
        : serverIp(serverIp), port(port), running(true), clientSocket(-1) {}

    /**
     * @brief Establishes a connection to the server.
     *
     * @return true if connection is successful, false otherwise.
     */
    bool connectToServer()
    {
        // Create a TCP socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0)
        {
            cerr << "Couldn't create socket." << endl;
            return false;
        }

        // Configure server address structure
        serverAddr.sin_family = AF_INET;                            // IPv4
        serverAddr.sin_port = htons(port);                          // Convert port to network byte order
        inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr); // Convert IP from text to binary

        // Attempt to connect to the server
        if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            cerr << "Couldn't connect to server." << endl;
            close(clientSocket);
            return false;
        }

        cout << "Connected to server." << endl;
        return true;
    }

    /**
     * @brief Continuously sends messages to the server until a quit command is issued.
     */
    void sendMessages()
    {
        string message;
        while (running)
        {
            cout << ">>> ";        // Prompt for user input
            getline(cin, message); // Read user input

            // Check for exit commands
            if (message == "quit()" || message == "exit()")
            {
                cout << "Closing connection." << endl;
                running = false;
            }

            // Send the message to the server
            if (send(clientSocket, message.c_str(), message.size(), 0) < 0)
            {
                cerr << "Failed to send message." << endl;
                running = false;
                break;
            }

            // If exit command was sent, terminate the loop
            if (!running)
            {
                break;
            }
        }
    }

    /**
     * @brief Continuously receives messages from the server.
     */
    void receiveMessages()
    {
        char buffer[1024];
        while (running)
        {
            memset(buffer, 0, sizeof(buffer));                              // Clear the buffer
            int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1); // Receive data

            if (bytesRead > 0)
            {
                buffer[bytesRead] = '\0'; // Null-terminate the received string
                cout << buffer << endl;   // Display server message
                fflush(stdout);           // Ensure output is displayed immediately
            }
            else if (bytesRead == 0)
            {
                cout << "Server disconnected." << endl;
                running = false; // Stop communication loop
                break;
            }
            else
            {
                cerr << "Error receiving data." << endl;
                running = false; // Stop communication loop
                break;
            }
        }
    }

    /**
     * @brief Initiates communication by starting send and receive threads.
     */
    void communicate()
    {
        // Launch threads for sending and receiving messages concurrently
        thread sendThread(&SimpleClient::sendMessages, this);
        thread receiveThread(&SimpleClient::receiveMessages, this);

        // Wait for both threads to finish execution
        sendThread.join();
        receiveThread.join();
    }

    /**
     * @brief Closes the client socket connection.
     */
    void closeConnection()
    {
        if (clientSocket >= 0)
        {
            close(clientSocket); // Close the socket
            clientSocket = -1;   // Reset socket descriptor
            cout << "\nConnection closed." << endl;
        }
    }

    /**
     * @brief Destructor to ensure the socket is closed upon object destruction.
     */
    ~SimpleClient()
    {
        close(clientSocket);
    }
};

int main()
{
    // Initialize client with server IP and port
    SimpleClient client("127.0.0.1", 9999);

    // Attempt to connect to the server
    if (client.connectToServer())
    {
        client.communicate();     // Start communication if connection is successful
        client.closeConnection(); // Close connection after communication ends
    }

    return 0;
}
