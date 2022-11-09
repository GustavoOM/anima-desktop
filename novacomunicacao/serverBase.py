import socket
import sys
from time import sleep

def server(port):
  host=socket.gethostname()
  server_socket = socket.socket()
  server_socket.bind((host,port))

  server_socket.listen(2)
  conn,address = server_socket.accept()
  print("Connection from: " + str(address))

  while True:
    sleep(1)
    data = conn.recv(1024).decode()
    if not data:
      break
    print("From connected user: " + str(data))
    data = input("--->")
    conn.send(data.encode())
  conn.close()


if __name__ == "__main__":
  port = int(sys.argv[1])
  server(port)

