import socket
import sys



def client(port):
    host=socket.gethostname()
    client_socket = socket.socket()
    client_socket.connect((host, port))

    message = input("----> ")
    while message.upper().strip() != "EOT":
        client_socket.send(message.encode())
        data = client_socket.recv(1024).decode()
        print("Received from server: " + data)
        message = input("----> ")
    client_socket.close()

if __name__ == "__main__":
    port = int(sys.argv[1]) 
    client(port)