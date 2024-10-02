# Client Server socket program

Example of client and server programs.
Client tries to connect using ip address and predefined PORT = `3490` and prints whatever it receives.
Server listens for connections and sends `Hello, world!` when connection is established, then close the connection.

Examples are taken from mighty [C Socket tutorial](https://beej.us/guide/bgnet/html/#client-server-background)

## Build and Run

C++ version 23(pure for testing)

```
  $ mkdir build && cd build
  $ cmake ..
  $ make -j
  $ ./server
  ...
  $ ./client <address> # in another terminal
```