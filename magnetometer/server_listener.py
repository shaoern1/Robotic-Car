import socket

# Define the server address and port
HOST = '0.0.0.0'  # Listen on all available network interfaces
PORT = 4242       # Port to listen on

# Create a TCP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen(5)  # Allow a backlog of 5 connections
print(f"Server is listening on port {PORT}...")

try:
    while True:
        # Wait for a client connection
        client_socket, client_address = server_socket.accept()
        print(f"Connection established with {client_address}")
        
        while True:
            # Receive and send data (example interaction)
            data = client_socket.recv(1024).decode('utf-8')
            if not data:
                break
            print(f"Received from client: {data}")
        
        # Close the client connection
        client_socket.close()
        print(f"Connection with {client_address} closed")
except KeyboardInterrupt:
    print("\nServer shutting down.")
finally:
    server_socket.close()
