#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <unistd.h>

#define MAX 1024 
#define PORT 8080 
#define SA struct sockaddr 

typedef struct 
{
    char *type;
    float montant;
    char *date;
}OPERATION;

typedef struct
{
    int id_client;
    int id_compte;
    char *password;
    float solde;
    OPERATION operation[10];
}CLIENT;


void commu(int sockfd) 
{ 
    char reponse[MAX]; 
    int n; 
    char demande[MAX];
    bzero(reponse, sizeof(reponse)); 
    for (;;) { 
        read(sockfd,reponse,sizeof(reponse));
        printf("Message serveur : %s\n",reponse );

        //écrit l'opération du client dans la opération
        n = 0; 
        while ((demande[n++] = getchar()) != '\n');
        write(sockfd, demande, sizeof(demande)); 

        //réinitialisation, lire le message du serveur et l'affichier 
        //bzero(reponse, sizeof(reponse)); 
        read(sockfd, reponse, sizeof(reponse)); 
        printf("Message Serveur : %s\n", reponse); 

        //vérification de l'existence de exit dans le message
        if ((strncmp(reponse, "exit", 4)) == 0) { 
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