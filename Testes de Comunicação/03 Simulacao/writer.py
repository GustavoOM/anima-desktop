import socket
from time import sleep
listaDeInstrucoes = [
  ['^17,1,2,1,2405263753', "24,1,55852,1,0,3351562173"],
  ['^17,1,3,2,2405264010', "24,1,56572,2,0,3301427134"],
  ['^17,1,4,3,2405264267', "24,1,56742,3,0,3334850494"],
  ['^17,1,5,4,2405264524', "24,1,56972,4,0,3368667070"],
  ['^17,1,6,5,2405264781', "24,1,57142,5,0,3234318271"],
  ['^17,1,7,6,2405265038', "24,1,57332,6,0,3267872703"],
  ['^17,1,8,7,2405265295', "24,1,57612,7,0,3318138815"],
  ['^17,1,9,8,2405265552', "24,1,57832,8,0,3351889855"],
  ['^17,1,10,9,2405274773', "24,1,57832,9,0,3351889855"],
  ['^17,1,11,10,2408159629', "24,1,57832,10,0,3351889855"],
  ['^17,1,12,11,2408159631', "24,1,57832,11,0,3351889855"],
  ['^17,1,13,12,2408159633', "24,1,57832,12,0,3351889855"],
  ['^17,1,14,13,2408159635', "24,1,57832,13,0,3351889855"],
  ['^17,1,15,14,2408159637', "24,1,57832,14,0,3351889855"],
  ['^15,1,16,,,7,3146355353', "24,1,57832,14,0,3351889855"]
]

def writer():
    host = socket.gethostname()  # as both code is running on same pc
    port_client = 8083
    port_server = 8085

    print("-+-+-+ Criando Cliente +-+-+-\n")
    client_socket = socket.socket()  # instantiate
    client_socket.connect((host, port_client))  # connect to the server
    print(f"Me conectei com o serverClient.c na porta: {port_client}\n")

    print("-+-+-+ Criando Servidor +-+-+-\n")
    tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
    tcp.bind((host,port_server))
    tcp.listen(1) 
    conexao, docliente = tcp.accept()
    print(f"O reader.py se conectou comigo na porta: {port_server}\n")


    message = listaDeInstrucoes[0][0]
    while 1:
        print('Sending to server: ' + message)  # show in terminal
        client_socket.send(message.encode())  # send message
        data = client_socket.recv(1024).decode() # receive response
        sleep(1)
        print('Received from server: ' + data)  # show in terminal
        message = ""
        for i in range(len(listaDeInstrucoes)-1):
            if listaDeInstrucoes[i][1].strip() in data.strip():
                message = listaDeInstrucoes[i+1][0]
        
    client_socket.close()  # close the connection


if __name__ == '__main__':
    writer()
