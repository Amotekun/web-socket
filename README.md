# Simple Client-Server Model in C++

This project is a basic implementation of a client-server communication model using C++. It demonstrates foundational aspects of socket programming, including how to create and manage TCP connections for networked applications on Unix-like platforms.

## Key Features

- **Socket Programming**: Uses POSIX sockets (`AF_INET`, `SOCK_STREAM`) to create a TCP server and client.
- **Network Communication**: The client and server exchange messages, demonstrating how to send and receive data over TCP.
- **Error Handling**: Robust error checking for socket operations such as creation, binding, and data transmission.
- **Building Blocks for Concurrency**: This implementation can be extended to handle multiple client connections concurrently.

## How to Compile and Run

### Prerequisites

Make sure you have **g++** installed on your system. On macOS or Linux, you can install it with:

```zsh
# For macOS (using Homebrew)
brew install gcc

# For Linux (Debian-based systems)
sudo apt-get install g++

Compile the server code, Open a terminal (in zsh shell) and navigate to the directory where server.cpp is located. Use the following command to compile the server code:

g++ -o server server.cpp: (This will create an executable file named server.)

Run the server executable:

./server


Compile the client code in a separate terminal (in zsh shell), navigate to the directory where client.cpp is located. Use the following command to compile the client code:

g++ -o client client.cpp:  (This will create an executable file named client.)

Run the client executable:

./client

Make sure to run the client in a separate terminal from the server. The client will connect to the server and allow you to send messages to it. The server will receive the messages and respond back to the client. To disconnect from the server, type "quit()" as a message. The client will then close the connection and exit.


```
