import socket
import sys

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

def reader():

  host = socket.gethostname()  # as both code is running on same pc
  port_client = 8085  
  port_server = 8084

  print("-+-+-+ Criando Cliente +-+-+-\n")
  client_socket = socket.socket()  # instantiate
  client_socket.connect((host, port_client))  # connect to the server
  print(f"Me conectei com o writer.py na porta: {port_client}\n")
  
  print("-+-+-+ Criando Servidor +-+-+-\n")
  tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  tcp.bind(('127.0.0.1', port_server))
  tcp.listen(1) 
  conexao, docliente = tcp.accept()
  print(f"O serverClient.c se conectou comigo na porta: {port_server}\n")

  try:
  
    while 1:
      Mensagem_Recebida = conexao.recv(1024).decode()
      #Mensagem recebida do cliente 
      if testa_mensagem != Mensagem_Recebida:
        if Mensagem_Recebida == b"EOT":
          break  
      #aqui verifica se exite mensagem nova  
        print ("Recebi = ",Mensagem_Recebida," , Do cliente", docliente)
    conexao.close()
    

  except KeyboardInterrupt:
    print ('KeyboardInterrupt exception is caught')
    conexao.close()

  finally:
      conexao.close()


if __name__ == '__main__':
  reader()