import socket
import sys
from time import sleep

listaDeInstrucoes = [
  ['17,1,2,1,2405263753', "24,1,55852,1,0,3351562173"],
  ['17,1,3,2,2405264010', "24,1,56572,2,0,3301427134"],
  ['17,1,4,3,2405264267', "24,1,56742,3,0,3334850494"],
  ['17,1,5,4,2405264524', "24,1,56972,4,0,3368667070"],
  ['17,1,6,5,2405264781', "24,1,57142,5,0,3234318271"],
  ['17,1,7,6,2405265038', "24,1,57332,6,0,3267872703"],
  ['17,1,8,7,2405265295', "24,1,57612,7,0,3318138815"],
  ['17,1,9,8,2405265552', "24,1,57832,8,0,3351889855"]
]

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
    print("[WRITER] Message sent: " + str(data))
    dataSend = "Sem instruções"
    for instrucao in listaDeInstrucoes:
      if instrucao[0] == data:
        dataSend = instrucao[1]

    conn.send(dataSend.encode())
  conn.close()


if __name__ == "__main__":
  port = int(sys.argv[1])
  server(port)

