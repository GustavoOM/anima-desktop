#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> // inet_addr()
#include <strings.h> // bzero()
#include <unistd.h> // read(), write(), close()


#define SERVERPORT 8083
#define CLIENTPORT 8084
#define MAX 80
#define SA struct sockaddr

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

  printf("-+-+-+ Criando Servidor +-+-+-\n");
  //Criando um servidor
  int sockfd, connfd, len;
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
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERVERPORT);
  
  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
      printf("socket bind failed...\n");
      exit(0);
  }
  else
      printf("Socket successfully binded..\n");
  
  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
      printf("Listen failed...\n");
      exit(0);
  }
  else
      printf("Server listening..\n");
  len = sizeof(cli);
  
  // Accept the data packet from client and verification
  connfd = accept(sockfd, (SA*)&cli, &len);
  if (connfd < 0) {
      printf("server accept failed...\n");
      exit(0);
  }
  else
      printf("server accept the client...\n");
  
  int k = 0;
  for(int i = 0;i < 72000; i++){
    for(int j = 0;j < 72000; j++){
      k = i + j;
    }
  }

  printf("-+-+-+ Criando Cliente +-+-+-\n");

  //Criando Cliente
  int sockfd_c, connfd_c;
  struct sockaddr_in servaddr_c, cli_c;

  // socket create and verification
  sockfd_c = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_c == -1) {
      printf("socket creation failed...\n");
      exit(0);
  }
  else
      printf("Socket successfully created..\n");
  bzero(&servaddr_c, sizeof(servaddr_c));

  // assign IP, PORT
  servaddr_c.sin_family = AF_INET;
  servaddr_c.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr_c.sin_port = htons(CLIENTPORT);

  // connect the client socket to server socket
  if (connect(sockfd_c, (SA*)&servaddr_c, sizeof(servaddr_c)) != 0) {
      printf("connection with the server failed...\n");
      exit(0);
  }
  else
      printf("connected to the server..\n");

  char buffer[1024];
  char dataRecev[1024];
  while(1){
    printf("VOLTEI PRO COMEÇO!\n");
    while(1){
      memset(buffer, '\0', sizeof(buffer));
      recv(sockfd_c, buffer, 1024, 0);
      
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

  
}
