#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <time.h>

#define MAX 1024 
#define PORT 8080 
#define SA struct sockaddr
#define Somme

typedef struct
{
    int jour;
    int mois;
    int annee;
}DATE;

typedef struct 
{
    char type;
    float montant;
    DATE date;
}OPERATION;

typedef struct
{
    int id_client;
    int id_compte;
    string password;
    float solde;
    OPERATION operation[10];
}CLIENT;

CLIENT cli[MAX];

int ajout(int client, int compte, int password, float somme)
{
    int res = -1;
    for (int i = 0; i<MAX; i++)
    {
        if (cli[i].id_client==client && cli[i].id_compte==compte && cli[i].passeword==password)
        {
            cli[i].solde +=somme;
            res = 1;
            break;
        }
    }
    return res;
}

int retrait(int client, int compte, string password, float somme)
{
    int res = -1;
    for (int i = 0; i<MAX; i++)
    {
        if (cli[i].id_client==client && cli[i].id_compte==compte && cli[i].passeword==password)
        {
            cli[i].solde -=somme;
            res = 1;
            break;
        }
    }
    return res;
}

float solde(int client, int compte, string password)
{
    for (int i=0; i<MAX; i++)
    {
        if(cli[i].id_client==client && cli[i].id_compte==compte && cli[i].passeword==password)
        {
            return cli[i].solde;
        }
    }
    return -1;
}

int operations(int client, int compte, string password)
{
    int res = -1;
    for (int i=0; i<MAX; i++)
    {
        if(cli[i].id_client==client && cli[i].id_compte==compte && cli[i].passeword==password)
        {
            for (int j=0; j<10; j++)
            {
                printf("%s  date: %d/%d/%d  solde:%f\n",cli[i]->operation[j]->type,
                                                        cli[i]->operation[j]->date->jour,
                                                        cli[i]->operation[j]->date->mois,
                                                        cli[i]->operation[j]->date->annee,
                                                        cli[i]->operation[j]->montant);
            }
            res = 1;
            break;
        }
    }
    return res;
}


// fonction de communication entre le client et le serveur 
void commu(int sockfd) 
{ 
    char reponse[MAX];
    int n; 
    char operation[MAX];
    char opere[4] = {"Ajout","Retrait","Solde","Operation"};
    char id_client[MAX];
    char id_compte[MAX];
    char password[MAX];
    char somme[MAX];

    // communication sans arrêt
    for (;;) { 
        bzero(reponse, MAX);  //initialiser le buff en zéro
        bzero(operation, MAX);  //initialiser l'opération en zéro
        
        strcpy(reponse,"Saisir l'un des opérations suivantes : \nAjout\nRetrait\nSolde\nOperations\n");
        write(sockfd,reponse,sizeof(reponse));
        bzero(reponse, MAX);  //initialiser le buff en zéro
        
        // lire l'operation de client et copier dans buff
        read(sockfd, operation, sizeof(operation)); 
        // afficher l'opération dans le terminal
        printf("Opération du client: %s\t", operation);

        for (int i = 0; i < 4; i++){
            if(*operation == opere[i]){
                strcpy(reponse,"Saisir votre id de client :");
                //copier le message de serveur dans la réponse et envoie au client
                write(sockfd,reponse,sizeof(reponse));
            }
        }
        if(!reponse)
        {
            strcpy(reponse,"KO");
            write(sockfd,reponse,sizeof(reponse));
        }

        
        
        //printf("**********TEST********Reponse de serveur est : %s\n",reponse);

        //bzero(id_client, MAX);  //réinitialiser
       // n = 0; 

        // copier le message de serveur dans le buff
       // while ((id_client[n++] = getchar()) != '\n'); 
  
        // Envoie ce message à client
       //write(sockfd, buff, sizeof(buff)); 
  
        // si le message contient "exit" alors le serveur termine la connection
        if (strncmp("exit", reponse, 4) == 0) { 
            printf("Serveur Exit...\n"); 
            break; 
        } 
    } 
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
