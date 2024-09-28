// Include necessary headers for input/output streams, socket programming,
// Internet address structures, IP address conversion, and POSIX I/O functions on Unix-like systems.
#include <iostream>     // For standard input/output operations
#include <sys/socket.h> // For socket functions (socket(), connect(), send(), recv())
#include <netinet/in.h> // For Internet address structures (struct sockaddr_in)
#include <arpa/inet.h>  // For IP address conversion functions (inet_pton())
#include <unistd.h>     // For POSIX operating system API, including socket closure (close())

using namespace std;
class simpleClient
{
private:
    int port;
    string serverIp;
    int clientSocket;
    char buffer[1024]; // Buffer to hold data received from the server
    string message;

public:
    simpleClient(const string &serverIp, int port)
    {
        this->port = port;
        this->serverIp = serverIp;
        clientSocket = -1; // Initialize client socket to -1 to indicate that the socket is not connected
    }

    // Destructor to close the connection when the object is destroyed
    ~simpleClient() { closeConnection(); }

    // Function to connect to the server and create a socket
    bool connectToServer()
    {
        // Create a TCP socket (AF_INET: IPv4, SOCK_STREAM: TCP)
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0)
        {
            cerr << "Couldn't connect to server" << endl;
            return false;
        }

        // Set up the server address structure
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);                          // Convert port to network byte order
        inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr); // Convert IP address from string to binary form

        if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            cerr << "Couldn't connect to server" << endl;
            close(clientSocket);
            return false;
        }

        cout << "Connected to server" << endl;
        return true;
    }

    // Function to send and receive messages from the server
    void communicate()
    {
        while (true)
        {
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer before each use

            cout << "Enter Message for the server (type 'quit()' to disconnect): ";
            getline(cin, message);

            if (message.empty())
            {
                cout << "Message cannot be empty, please enter a valid message" << endl;
                continue;
            }

            // Send message to the server
            send(clientSocket, message.c_str(), message.length(), 0);

            if (message == "quit()")
            {
                cout << "Closing connection" << endl;
                break;
            }

            // Receive a response from server
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0)
            {
                buffer[bytesRead] = '\0';
                cout << "Response from server: " << buffer << endl;
            }
            else if (bytesRead == 0)
            {
                cout << "Connection closed by server: " << endl;
                break;
            }
            else
            {
                cerr << "Error receiving message from server" << endl;
                break;
            }
        }
    }

    // Function to close the connection
    void closeConnection()
    {
        if (clientSocket >= 0)
        {
            close(clientSocket);
            clientSocket = -1;
            cout << "Connection closed" << endl;
        }
    }
};

// Main function to create a client object, connect to the server, communicate, and close the connection
int main()
{
    simpleClient client("127.0.0.1", 9999);
    if (client.connectToServer())
    {
        client.communicate();
        client.closeConnection();
    }

    return 0;
}

// compile the code using the following command:
// 1. g++ -o client client.cpp
// 2. ./client
// Run the server and client in separate terminal windows. The client will connect to the server and send messages to it. The server will receive the messages and send a response back to the client. The client will display the response from the server. To disconnect from the server, type "quit()" as a message. The client will close the connection and exit.