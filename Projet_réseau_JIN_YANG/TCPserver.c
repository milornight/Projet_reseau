#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 

#define MAX 1024 
#define PORT 8080 
#define SA struct sockaddr 
  
// fonction de communication entre le client et le serveur 
void commu(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 

    // communication sans arrêt
    for (;;) { 
        bzero(buff, MAX);  //initialiser le buff en zéro
  
        // lire le message de client et copier dans buff
        read(sockfd, buff, sizeof(buff)); 

        // afficher le message dans le terminal
        printf("Message du client: %s\t Réponse : ", buff); 

        bzero(buff, MAX);  //réinitialiser
        n = 0; 

        // copier le message de serveur dans le buff
        while ((buff[n++] = getchar()) != '\n'); 
  
        // Envoie ce message à client
        write(sockfd, buff, sizeof(buff)); 
  
        // si le message contient "exit" alors le serveur termine la connection
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Serveur Exit...\n"); 
            break; 
        } 
    } 
} 

int ajout(int id_client, int id_compte, int password, float somme)
{
    return -1;
}


int retrait(int id_client, int id_compte, int password, float somme)
{
    return -1;
}


float solde(int id_client, int id_compte, int password)
{
    float solde;
    return solde;
}


string operation(int id_client, int id_compte, int password)
{   
    string s;
    return s;
} 
  
 
int main() 
{ 
    int sockfd, connfd, lenght; 
    struct sockaddr_in servaddr,client; 
  
    // creation de socket et vérification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("Echec de la création du socket \n"); 
        exit(0); 
    }else{
        printf("La création du Socket a réussi.\n"); 
    }
    bzero(&servaddr, sizeof(servaddr));  //initialisation
  
    // distribuer IP, PORT 
    servaddr.sin_family = AF_INET; //utiliser IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Donner un nom à sockfd et verifier
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Echoué: socket bind\n"); 
        exit(0); 
    } 
    else
        printf("Réussi: Socket binded\n"); 
  
    // La serveur est prêt pour écouter et la vérification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Echoué de l'écoute\n"); 
        exit(0); 
    } 
    else
        printf("Server est en train d'écouter\n"); 

    lenght = sizeof(client); 
  
    // Accept la connexion du client et vérification 
    connfd = accept(sockfd, (SA*)&client, &lenght); 
    if (connfd == -1) { 
        printf("Echoue de la connexion du client\n"); 
        exit(0); 
    } 
    else
        printf("La serveur acccept la connexion du client\n"); 
  
    // communication entre serveur et client
    commu(connfd); 
  
    // fermer le socket 
    close(sockfd); 
} 
