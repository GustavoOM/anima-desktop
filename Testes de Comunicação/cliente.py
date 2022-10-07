###############cliente TCP###########

#!/usr/bin/python3
import socket

IP_Servidor = '127.0.0.1'             
# Endereco IP do Servidor

PORTA_Servidor = 8000
# Porta em que o servidor estara ouvindo

tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# socket.AF_INET = INET (exemplo IPv4)sockets, #socket.SOCK_STREAM=usaremos TCP

DESTINO = (IP_Servidor, PORTA_Servidor) 
#destino(IP + porta)

tcp.connect(DESTINO) 
# inicia a conexao TCP
while 1:
 Mensagem = input()   
 # Mensagem recebera dados do teclado
 
 tcp.send(bytes(Mensagem,"utf8"))
 # enviar a mensgem para o destinoda conexao(IP + porta)   
 #bytes(Mensagem,"utf8") = converte tipo  str para byte    
tcp.close()
# finalizar o socket     
