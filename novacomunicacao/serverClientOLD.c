#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <netdb.h>
#include <strings.h>

#define SERVERPORT 8083
#define CLIENTPORT 8084
#define MAX 80
#define SA struct sockaddr

void func(int sockfd)
  {
      char buff[MAX];
      int n;
      for (;;) {
          bzero(buff, sizeof(buff));
          printf("Enter the string : ");
          n = 0;
          while ((buff[n++] = getchar()) != '\n')
              ;
          write(sockfd, buff, sizeof(buff));
          bzero(buff, sizeof(buff));
          read(sockfd, buff, sizeof(buff));
          printf("From Server : %s", buff);
          if ((strncmp(buff, "exit", 4)) == 0) {
              printf("Client Exit...\n");
              break;
          }
      }
  }

int main(){
  char listaDeInstrucoes[40][40] = {
    {"^17,1,2,1,2405263753"}, {"24,1,55852,1,0,3351562173"},
    {"^17,1,3,2,2405264010"}, {"24,1,56572,2,0,3301427134"},
    {"^17,1,4,3,2405264267"}, {"24,1,56742,3,0,3334850494"},
    {"^17,1,5,4,2405264524"}, {"24,1,56972,4,0,3368667070"},
    {"^17,1,6,5,2405264781"}, {"24,1,57142,5,0,3234318271"},
    {"^17,1,7,6,2405265038"}, {"24,1,57332,6,0,3267872703"},
    {"^17,1,8,7,2405265295"}, {"24,1,57612,7,0,3318138815"},
    {"^17,1,9,8,2405265552"}, {"24,1,57832,8,0,3351889855"},
    {"^17,1,10,9,2405274773"}, {"24,1,57832,9,0,3351889855"},
    {"^17,1,11,10,2408159629"}, {"24,1,57832,10,0,3351889855"},
    {"^17,1,12,11,2408159631"}, {"24,1,57832,11,0,3351889855"},
    {"^17,1,13,12,2408159633"}, {"24,1,57832,12,0,3351889855"},
    {"^17,1,14,13,2408159635"}, {"24,1,57832,13,0,3351889855"},
    {"^17,1,15,14,2408159637"}, {"24,1,57832,14,0,3351889855"},
    {"^15,1,16,,,7,3146355353"}, {"24,1,57832,14,0,3351889855"}
  };

  //Criando um servidor
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  char buffer[1024];
  socklen_t tamanhoADDR;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVERPORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  listen(server_fd, 5);
  printf("[OUVINDO] Porta número: %d\n", SERVERPORT);
  client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &tamanhoADDR);
  printf("[CONECTOU] Conectou ao writer!\n");

  //Conectar no servidor
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
      printf("socket creation failed...\n");
      exit(0);
  }
  else
      printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // connect the client socket to server socket
  if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
      != 0) {
      printf("connection with the server failed...\n");
      exit(0);
  }
  else
      printf("connected to the server..\n");
  

  while(1){
    printf("VOLTEI PRO COMEÇO!\n");
    while(1){
      memset(buffer, '\0', sizeof(buffer));
      recv(client_fd, buffer, 1024, 0);
      
      if(strlen(buffer) > 5){
        break;
      }
    }
    dataRecev = strtok(buffer, ";");
    printf("[WRITER] %s\n", dataRecev);
    //strcpy(dataSend,"Sem instruções");
    for(int i = 0; i < 20; i++) {
      if(!strcmp(dataRecev, listaDeInstrucoes[i])){
        
        strcpy(dataSend, listaDeInstrucoes[i+1]);
        printf("[READER DENTRO]: %s\n", dataSend);
      }
      printf("%d - %s\n", i, dataSend);
    }
    printf("[READER FORA]: %s", dataSend);
    printf("STRLEN DATASEND: %ld", strlen(dataSend));
    //send(sock, dataSend, strlen(dataSend), 0);
  }

  close(server_fd);

  return 0;
} 
/*

def server():
  portServer = 8083
  hostServer=socket.gethostname()
  server_socket = socket.socket()
  server_socket.bind((hostServer,portServer))

  server_socket.listen(2)
  conn_server,address = server_socket.accept()

  sleep(5)

  portClient = 8084
  hostClient = socket.gethostname()
  client_socket = socket.socket()
  client_socket.connect((hostClient, portClient))
  
  try:    
    while True:
      while True:
        data = conn_server.recv(1024).decode()
        if data:
          break
      data = data.split(";")[0]
      print("[WRITER]: " + str(data))
      dataSend = "Sem instruções"
      for instrucao in listaDeInstrucoes:
        if instrucao[0] == data:
          dataSend = instrucao[1]
      print("[READER]: " + str(dataSend))
      client_socket.send(dataSend.encode())

  except KeyboardInterrupt:
    print ('KeyboardInterrupt exception is caught')

  finally: 
    conn_server.close()


if __name__ == "__main__":
  #CLIENTPORT = int(sys.argv[1])
  server()

*/