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

def client(port):
    host=socket.gethostname()
    client_socket = socket.socket()
    client_socket.connect((host, port))

    message = input("----> ")

    while message.upper().strip() != "EOT":
        sleep(1)
        client_socket.send(message.encode())
        data = client_socket.recv(1024).decode()
        message = ""
        print("[READER] Message received: " + data) 
        for i, instrucao in enumerate(listaDeInstrucoes):
            if instrucao[1] == data:
                if len(listaDeInstrucoes) > i+1:
                    message = listaDeInstrucoes[i+1][0]  
                break   
        if message == "":
            message = input("----> ")
    client_socket.close()

if __name__ == "__main__":
    port = int(sys.argv[1]) 
    client(port)
