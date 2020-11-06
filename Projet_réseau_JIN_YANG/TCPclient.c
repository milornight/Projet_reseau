#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 

#define MAX 1024 
#define PORT 8080 
#define SA struct sockaddr 

void commu(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 

    for (;;) { 
        bzero(buff, sizeof(buff)); 
        printf("Taper le message : "); 

        //écrit le message du client dans le buff
        n = 0; 
        while ((buff[n++] = getchar()) != '\n'); 
        write(sockfd, buff, sizeof(buff)); 

        //réinitialisation, lire le message du serveur et l'affichier 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        printf("Message Serveur : %s", buff); 

        //vérification de l'existence de exit dans le message
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        } 
    } 
} 
  
int main() 
{ 
    int sockfd; 
    struct sockaddr_in cliaddr; 
  
    // creation de socket et vérification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("Echec de la création du socket\n"); 
        exit(0); 
    }else{
        printf("La création du Socket a réussi.\n"); 
    }
    bzero(&cliaddr, sizeof(cliaddr)); //initialisation
  
    // distribuer IP, PORT 
    cliaddr.sin_family = AF_INET; 
    cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    cliaddr.sin_port = htons(PORT); 
  
    // la connexion du client socket avec serveur socket et vérification
    if (connect(sockfd, (SA*)&cliaddr, sizeof(cliaddr)) != 0) { 
        printf("Echoué de la connexion \n"); 
        exit(0); 
    } 
    else
        printf("La connexion a réussi \n"); 
  
    // communication
    commu(sockfd); 
  
    // fermer le socket 
    close(sockfd); 
} 