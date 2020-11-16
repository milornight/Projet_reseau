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
#define nbCli 2


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


CLIENT cli[nbCli];
time_t rawtime;

int ajout(int client, int compte, char *password, float somme)
{
    for (int i = 0; i<nbCli; i++)
    {
        if (cli[i].id_client==client && cli[i].id_compte==compte && cli[i].password==password)
        {
            cli[i].solde +=somme;
            
            for (int j=0; j<10; j++)
            {
                if (cli[i].operation[j].type =='\0')
                {
                    cli[i].operation[j].type = "Ajout";
                    cli[i].operation[j].montant = cli[i].solde;
                    cli[i].operation[j].date = ctime(&rawtime);
                    break;
                }else{
                    if(j==9)
                    {
                        for (int k=0; k<9; k++)
                        {
                            cli[i].operation[k]=cli[i].operation[k+1];
                        }
                        cli[i].operation[9].type = "Ajout";
                        cli[i].operation[9].montant = cli[i].solde;
                        cli[i].operation[9].date = ctime(&rawtime);
                    }
                }
            }
            return 1;
        }
    }
    return -1;
}

int retrait(int client, int compte, char *password, float somme)
{
    for (int i = 0; i<nbCli; i++)
    {
        if (cli[i].id_client==client && cli[i].id_compte==compte && cli[i].password==password)
        {
            cli[i].solde -=somme;
            for (int j=0; j<10; j++)
            {
                if (cli[i].operation[j].type =='\0')
                {
                    cli[i].operation[j].type = "Retrait";
                    cli[i].operation[j].montant = cli[i].solde;
                    cli[i].operation[j].date = ctime(&rawtime);
                    break;
                }else{
                    if(j==9)
                    {
                        for (int k=0; k<9; k++)
                        {
                            cli[i].operation[k]=cli[i].operation[k+1];
                        }
                        cli[i].operation[9].type = "Retrait";
                        cli[i].operation[9].montant = cli[i].solde;
                        cli[i].operation[9].date = ctime(&rawtime);
                    }
                }
            }
            return 1;
        }
    }
    return -1;
}

void solde(int client, int compte, char *password)
{
    for (int i=0; i<nbCli; i++)
    {
        if(cli[i].id_client==client && cli[i].id_compte==compte && cli[i].password==password)
        {
            int cpt=0;
            if (cli[i].operation[cpt].type !='\0')
            {
                cpt++;
            }
            printf("solde:%f , date:%s ", cli[i].solde,cli[i].operation[cpt-1].date);
        }
    }
}

void operations(int client, int compte, char *password)
{
    for (int i=0; i<nbCli; i++)
    {
        if(cli[i].id_client==client && cli[i].id_compte==compte && cli[i].password==password)
        {
            for (int j=9; j>=0; j--)
            {
                printf("%s  date: %s  solde:%f\n",cli[i].operation[j].type,
                                                    cli[i].operation[j].date,
                                                    cli[i].operation[j].montant);
            }
            break;
        }
    }
}


// fonction de communication entre le client et le serveur 
void commu(int sockfd) 
{ 
    char reponse[MAX];
    int n; 
    char demande[MAX];
    int id_client;
    int id_compte;
    char *password;
    float somme;

    // communication sans arrêt
    for (;;) { 
        bzero(reponse, MAX);  //initialiser le buff en zéro
        bzero(demande, MAX);  //initialiser l'opération en zéro
        
        strcpy(reponse,"Saisir l'un des opérations suivantes : \nAjout\nRetrait\nSolde\nOperations\n");
        write(sockfd,reponse,sizeof(reponse));
        bzero(reponse, MAX);  //initialiser le buff en zéro
        
        // lire l'operation de client et copier dans buff
        read(sockfd, demande, sizeof(demande)); 
        // afficher l'opération dans le terminal
        printf("Opération du client: %s\t", demande);

        if (strncmp("Ajout", demande, 5) == 0)
        {
            char *pch;
            int Res;
            pch = strtok(demande, " ");
            id_client=(int)pch[1];
            id_compte=(int)pch[2];
            password=&pch[3];
            somme=(float)pch[4];
            Res = ajout(id_client,id_compte,password,somme);
            if(Res==-1)
            {
                strcpy(reponse,"KO");
                write(sockfd,reponse,sizeof(reponse)); 
            }else{
                strcpy(reponse,"OK");
                write(sockfd,reponse,sizeof(reponse)); 
            }
        }
        
        if (strncmp("Retrait", demande, 7) == 0)
        {
            char *pch;
            int Res;
            pch = strtok(demande, " ");
            id_client=(int)pch[1];
            id_compte=(int)pch[2];
            password=&pch[3];
            somme=(float)pch[4];
            Res = retrait(id_client,id_compte,password,somme);
            if(Res==-1)
            {
                strcpy(reponse,"KO");
                write(sockfd,reponse,sizeof(reponse)); 
            }else{
                strcpy(reponse,"OK");
                write(sockfd,reponse,sizeof(reponse)); 
            }
        }
        
        if (strncmp("Solde", demande, 5) == 0)
        {
            char *pch;
            int Res;
            pch = strtok(demande, " ");
            printf ("%s", pch);
            id_client=(int)pch[1];
            id_compte=(int)pch[2];
            password=&pch[3];
            solde(id_client,id_compte,password);
        }
        
        if(strncmp("Operations", demande, 10) == 0)
        {
            char *pch;
            int Res;
            pch = strtok(demande, " ");
            id_client=(int)pch[1];
            id_compte=(int)pch[2];
            password=&pch[3];
            operations(id_client,id_compte,password);
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

    CLIENT JIN = {1,1,"jin",29.2,{}};
    CLIENT YANG = {2,1,"yang",27.3,{}};
    cli[nbCli]=(JIN,YANG);
  
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
