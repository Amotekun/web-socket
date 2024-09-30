# Simple Client-Server Model in C++

A robust and scalable implementation of a client-server communication model using C++. This project demonstrates foundational aspects of socket programming, including the creation and management of TCP connections, and extends to handling multiple client connections concurrently through multithreading using pthreads. Ideal for learning and extending to more complex networked applications.

# Table of Contents

- Key Features
- Directory Structure
- Prerequisites
- How to Compile and Run
- Single-Threaded Version
- Multi-Threaded Version
- Understanding Multithreading with pthreads
- Usage
- Enhancements
  > > License....

## Key Features

- **Socket Programming**: Uses POSIX sockets (`AF_INET`, `SOCK_STREAM`) to create a TCP server and client.
- **Bidirectional Communication**: Enables sending and receiving messages between client and server.
- **Single-Threaded and Multi-Threaded Implementations**: Provides both basic and advanced versions to handle client connections.
- **Concurrent Client Handling**: (Multi-Threaded) Supports multiple clients simultaneously using pthreads.
- **Robust Error Handling**: Comprehensive checks for socket operations including creation, binding, listening, accepting, sending, and receiving.
- **Scalable Architecture:**: Easily extendable to accommodate more clients and functionalities.

## How to Compile and Run

### Single-Threaded version

```zsh
This version handles one client at a time without using multithreading.

Navigate to the Server Directory:

cd single-threaded/server
Compile the Server Code: g++ -o server server.cpp -std=c++17
Run the Server: ./server

Open a New Terminal and Navigate to the Client Directory:

cd single-threaded/client
Compile the Client Code: g++ -o client client.cpp -std=c++17
Run the Client: ./client

Interaction: Type messages in the client terminal. The server will receive and can respond. To disconnect, type quit() or exit() in the client
```

### Multi-Threaded Implementations

```zsh
This version allows the server to handle multiple clients concurrently using pthreads.

Navigate to the Server Directory:

cd Multi-threaded/server
Compile the Server Code: g++ -o server server.cpp -std=c++17 -pthread
Run the Server: ./server

Open Multiple Terminals for Clients and Navigate to the Client Directory:

cd Multi-threaded/client
Compile the Client Code: g++ -o client client.cpp -std=c++17 -pthread
Run the Client in Each Terminal: ./client

Interaction:Each client can send messages independently. The server can respond to each client individually. To disconnect a client, type quit() or exit() in the client terminal.

```

## Understanding Multithreading with pthreads

### Why Use Multithreading?

In a single-threaded server, handling multiple clients requires sequential processing, which can lead to delays and poor scalability. Multithreading allows the server to handle multiple clients simultaneously, improving responsiveness and efficiency.

### How it works in this project

1. Create Connection Handling:

- When a client connects, the server accepts the connection and spawns a new thread dedicated to that client.

- Each thread manages the communication (sending and receiving messages) with its respective client.

2. Thread Creation:

- Utilizes std::thread from the C++ Standard Library, which internally uses pthreads on Unix-like systems.

- Threads are detached to allow independent execution without blocking the main server thread.

3. Synchronization:

- Mutexes (std::mutex) are used to protect shared resources, such as the list of connected clients (clientSockets), ensuring thread-safe operations.

- Atomic Flags (std::atomic<bool>) control the running state, preventing race conditions when multiple threads modify shared variables.

### Benefits

- Scalability: Easily accommodates multiple clients without significant performance degradation.

- Responsiveness: Clients receive timely responses without waiting for other clients' interactions to complete.

- Resource Efficiency: Threads share the same memory space, making communication between them faster and more efficient compared to separate processes.

## Example interactions (Multi-Threaed Version)

### Server Terminal

```zsh
Server listening on port 9999
Waiting for client connections...
Client connected from 127.0.0.1:54321 [Socket ID: 4]
Waiting for client connections...
Client connected from 127.0.0.1:54322 [Socket ID: 5]
Client [4]: Hello Server!
>>> Hi Client 4!
Client [5]: Hey Server!
>>> Hi Client 5!
Client [4]: How are you?
>>> I'm good, thank you!
Client [5]: What's up?
>>> All good here!
```

### Client Terminal

```zsh
Client Teminal 1:

Connected to server.
>>> Hello Server!
Server: Hi Client 4!
>>> How are you?
Server: I'm good, thank you!
>>> quit()
Closing connection.
Connection closed.
```

```zsh
Client Terminal 2:
Connected to server.
>>> Hey Server!
Server: Hi Client 5!
>>> What's up?
Server: All good here!
>>> exit()
Closing connection.
Connection closed.
```

## Enhancements

To further improve and extend the functionality of your client-server model, consider implementing the following features:

1. Authentication Mechanisms:
   Introduce user authentication to ensure that only authorized clients can connect to the server.

2. Encryption and Security:
   Implement SSL/TLS to encrypt data transmission, protecting against eavesdropping and man-in-the-middle attacks.

3. Message Broadcasting:
   Allow the server to broadcast messages to all connected clients simultaneously, facilitating group chats or announcements.

4. GUI Integration:
   Develop graphical user interfaces for both the client and server to enhance user experience.

5. Asynchronous I/O:
   Utilize asynchronous I/O operations to improve scalability and performance, especially under high load.

6. Logging and Monitoring:
   Implement comprehensive logging to track server activities, client interactions, and potential errors for easier debugging and maintenance.

7. Configuration Files:
   Allow server and client configurations (like IP addresses and ports) to be specified through external configuration files for flexibility.
8. Error Recovery:
   Develop mechanisms to handle and recover from errors gracefully without crashing the server or clients.
