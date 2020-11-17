#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

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
    int id_compte;
    char *password;
    float solde;
    OPERATION operation[10];
}COMPTE;

typedef struct
{
    int id_client;
    COMPTE *compte;
}CLIENT;


void commu(int sockfd) 
{ 
    char reponse[MAX]; 
    int n; 
    char demande[MAX];

    bzero(reponse, sizeof(reponse));  //initialiser le réponse du serveur en zéro

    for(;;){ 
        read(sockfd,reponse,sizeof(reponse)); //lire la réponse du serveur
        printf("Message serveur : %s\n",reponse ); //afficher la réponse du serveur

        //écrit la demande du client
        n = 0; 
        while ((demande[n++] = getchar()) != '\n');
        write(sockfd, demande, sizeof(demande)); 

        if ((strncmp(reponse, "Exit", 4)) == 0) 
        { 
            printf("Client Exit...\n"); 
            break; 
        } 
        //réinitialisation, lire le message du serveur et l'affichier 
        bzero(reponse, sizeof(reponse)); 

        //vérification de l'existence de exit dans le message
        
    } 
} 

  
int main() 
{ 
    int sockfd; 
    struct sockaddr_in cliaddr; 
  
    // creation de socket et vérification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
    { 
        printf("Echec de la création du socket\n"); 
        exit(0); 
    }
    else
    {
        printf("La création du Socket a réussi.\n"); 
    }
    bzero(&cliaddr, sizeof(cliaddr)); //initialisation
  
    // distribuer IP, PORT 
    cliaddr.sin_family = AF_INET; 
    cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    cliaddr.sin_port = htons(PORT); 
  
    // la connexion du client socket avec serveur socket et vérification
    if (connect(sockfd, (SA*)&cliaddr, sizeof(cliaddr)) != 0) 
    { 
        printf("Echoué de la connexion \n"); 
        exit(0); 
    } 
    else
    {
        printf("La connexion a réussi \n"); 
    }

    // communication entre le serveur et le client
    commu(sockfd); 
  
    // fermer le socket 
    close(sockfd); 
} 