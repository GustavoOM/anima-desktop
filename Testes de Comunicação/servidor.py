import socket
import sys

MEU_IP = '127.0.0.1'
MINHA_PORTA = int(sys.argv[1])  

tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# socket.AF_INET = INET (exemplo IPv4)sockets, #socket.SOCK_STREAM=usaremos TCP

#x = 1
testa_mensagem = ''
MEU_SERVIDOR = (MEU_IP, MINHA_PORTA) 
tcp.bind(MEU_SERVIDOR)

tcp.listen(1) 

conexao, docliente =tcp.accept()
print ("o cliente = ", docliente, " se conectou")
#pega o ip do cliente que conectou

try:
 
  while 1:
    Mensagem_Recebida = conexao.recv(1024)
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
