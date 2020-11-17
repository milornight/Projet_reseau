#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define MAX 1024 
#define PORT 8080 
#define SA struct sockaddr
//#define nbCli 2


typedef struct 
{
    char *type;
    int montant;
    char *date;
}OPERATION;

typedef struct
{
    int id_compte;
    char *password;
    int solde;
    OPERATION operation[10];
}COMPTE;

typedef struct
{
    int id_client;
    COMPTE compte[10];
}CLIENT;


CLIENT c;
//CLIENT cli[nbcli];
time_t rawtime;

int ajout(int client, int compte, char *password, int somme)
{     
    int res = -1;
    if (c.id_client==client) //vérifier si ce client existe
    {
        for (int i = 0; i<10; i++)
        {
             //vérifier s'il a un compte correspond et le mot de passe est correcte ou pas
            if(c.compte[i].id_compte==compte && c.compte[i].password==password)
            {
                c.compte[i].solde +=somme; //versement
                res = 0;
            }
            for (int j=0; j<10; j++) //enregistrer l'opération, la date de cette opération et le montant
            {
                if (c.compte->operation[j].type =='\0') //vérifier si le tableau d'opération est plein ou pas 
                {
                    c.compte->operation[j].type = "Ajout";
                    c.compte->operation[j].montant = c.compte[i].solde;
                    c.compte->operation[j].date = ctime(&rawtime);
                    break;
                }
                else if(j==9) //si il est plein, on supprime le plus ancien et ajouter le plus récente
                {
                    for (int k=0; k<9; k++) 
                    {
                        c.compte->operation[k]=c.compte->operation[k+1];
                    }
                    c.compte->operation[9].type = "Ajout";
                    c.compte->operation[9].montant = c.compte[i].solde;
                    c.compte->operation[9].date = ctime(&rawtime);
                }
            }
        }
    }
    return res;
}

//La même manipulation que ajout, juste change le type en Retrait
int retrait(int client, int compte, char *password, int somme)
{     
    int res = -1;
    if (c.id_client==client)
    {
        for (int i = 0; i<10; i++)
        {
            if(c.compte[i].id_compte==compte && c.compte[i].password==password)
            {
                c.compte[i].solde +=somme;
                res = 0;
            }
            for (int j=0; j<10; j++)
            {
                if (c.compte->operation[j].type =='\0')
                {
                    c.compte->operation[j].type = "Retrait";
                    c.compte->operation[j].montant = c.compte[i].solde;
                    c.compte->operation[j].date = ctime(&rawtime);
                    break;
                }
                else if(j==9)
                {
                    for (int k=0; k<9; k++)
                    {
                        c.compte->operation[k]=c.compte->operation[k+1];
                    }
                    c.compte->operation[9].type = "Retrait";
                    c.compte->operation[9].montant = c.compte[i].solde;
                    c.compte->operation[9].date = ctime(&rawtime);
                }
            }
        }
    }
    return res;
}


void solde(int client, int compte, char *password)
{
    if(c.id_client==client) //vérifier si ce client existe
    {
        for (int i = 0; i<10; i++) 
        {
             //vérifier s'il a un compte correspond et le mot de passe est correcte ou pas
            if(c.compte[i].id_compte==compte && c.compte[i].password==password)
            {
                int cpt=0;
                while (c.compte[i].operation[cpt].type !='\0') //chercher le plus récent opération
                {
                    cpt++;
                }
                //afficher le montant actuel et la date du dernier opération
                printf("solde:%d , date:%s ", c.compte[i].solde,c.compte[i].operation[cpt-1].date); 
            }
        }
    }
}

void operations(int client, int compte, char *password)
{
    if(c.id_client==client) //vérifier si ce client existe
    {
        for (int i = 0; i<10; i++)
        {
            //vérifier s'il a un compte correspond et le mot de passe est correcte ou pas
            if(c.compte[i].id_compte==compte && c.compte[i].password==password)
            {
                //afficher le date, le montant et le type des opérations en ordre le plus récent jusqu'à le plus ancien
                for (int j=9; j>=0; j--)
                {
                    printf("%s  date: %s  solde:%d \n",c.compte[i].operation[j].type,
                                                    c.compte[i].operation[j].date,
                                                    c.compte[i].operation[j].montant);
                }
            }
        } 
    }   
}


// fonction de communication entre le client et le serveur 
void commu(int sockfd) 
{ 
    char reponse[MAX]; 
    char demande[MAX];
    int id_client;
    int id_compte;
    char *password;
    int somme;

    // communication sans arrêt
    for(;;){ 
        bzero(reponse, MAX);  //initialiser le réponse du serveur en zéro
        bzero(demande, MAX);  //initialiser la demande d'opération en zéro
        
        //capter le Id du client
        strcpy(reponse,"Entré Id du client : ");
        write(sockfd,reponse,sizeof(reponse));
        bzero(reponse, MAX);  //initialiser le buff en zéro
        read(sockfd, demande, sizeof(demande)); // lire le message de client et copier dans buff
        id_client=(int)demande;
        bzero(demande, MAX);

        //capter le ID du compte
        strcpy(reponse,"Entré Id du compte : ");
        write(sockfd,reponse,sizeof(reponse));
        bzero(reponse, MAX);  
        read(sockfd, demande, sizeof(demande)); 
        id_compte=(int)demande;
        bzero(demande, MAX);

        //capter le mot de passe
        strcpy(reponse,"Entré le mot de passe : ");
        write(sockfd,reponse,sizeof(reponse));
        bzero(reponse, MAX);  
        read(sockfd, demande, sizeof(demande)); 
        password=demande;
        bzero(demande, MAX);

        //capter la demande des opérations
        strcpy(reponse,"Saisir l'un des opérations suivantes : \nAjout\nRetrait\nSolde\nOperations\n");
        write(sockfd,reponse,sizeof(reponse));
        bzero(reponse, MAX);  
        read(sockfd, demande, sizeof(demande));    // lire l'operation de client et copier dans buff
        // afficher l'opération dans le terminal
        printf("Opération du client: %s\t", demande);

        //tester les opération demandé
        if (strncmp("Ajout", demande, 5) == 0)
        {
            bzero(demande, MAX);
            strcpy(reponse,"Entré le montant : ");
            write(sockfd,reponse,sizeof(reponse));
            bzero(reponse, MAX);  
            read(sockfd, demande, sizeof(demande)); 
            somme=(int)demande;
            int Res;
            
            Res = ajout(id_client,id_compte,password,somme);

            //vérifier si l'opération demandé a bien déroulé
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
            bzero(demande, MAX);
            strcpy(reponse,"Entré le montant : ");
            write(sockfd,reponse,sizeof(reponse));
            bzero(reponse, MAX);  
            read(sockfd, demande, sizeof(demande)); 
            somme=(int)demande;
            int Res;
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
            solde(id_client,id_compte,password);
        }
        
        if(strncmp("Operations", demande, 10) == 0)
        {
            operations(id_client,id_compte,password);
        }

        //vérification de l'existence de exit dans le message
        if ((strncmp(reponse, "exit", 4)) == 0) 
        { 
            printf("Client Exit...\n"); 
            break;
        } 
    } 
} 


int main() 
{ 
    int sockfd, connfd, lenght; 
    struct sockaddr_in servaddr,client;

    COMPTE p1={1,"jin1",24,{}};
    COMPTE p2={2,"jin2",100,{}};
    COMPTE p[10]={p1,p2};
    CLIENT JIN = {1,p};
  
    // creation de socket et vérification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
    { 
        printf("Echec de la création du socket \n"); 
        exit(0); 
    }
    else
    {
        printf("La création du Socket a réussi.\n"); 
    }
    bzero(&servaddr, sizeof(servaddr));  //initialisation
  
    // distribuer IP, PORT 
    servaddr.sin_family = AF_INET; //utiliser IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
  
    // Donner un nom à sockfd et verifier
    if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
    { 
        printf("Echoué: socket bind\n"); 
        exit(0); 
    } 
    else
    {
        printf("Réussi: Socket binded\n"); 
    }

    // La serveur est prêt pour écouter et la vérification 
    if (listen(sockfd, 100) != 0) 
    { 
        printf("Echoué de l'écoute\n"); 
        exit(0); 
    } 
    else
    {
        printf("Server est en train d'écouter\n"); 
    }

    lenght = sizeof(struct sockaddr_in);
  
    // Accept la connexion du client et vérification 
    connfd = accept(sockfd, (struct sockaddr*)&client, (socklen_t*)&lenght); 
    if (connfd == -1) 
    { 
        printf("Echoue de la connexion du client\n"); 
        exit(0); 
    } 
    else
    {
        printf("La serveur acccept la connexion du client\n"); 
    }

    // communication entre serveur et client
    commu(connfd); 
  
    // fermer le socket 
    close(sockfd); 
} 
