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
#include <ctype.h>

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

COMPTE p[10];

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
            if(c.compte[i].id_compte==compte && (strcmp(c.compte[i].password,password)==0))
            {
                c.compte[i].solde +=somme; //versement
                res = 0;
                int j=0;
                while (j<10 && c.compte[i].operation[j].type !=NULL) //enregistrer l'opération, la date de cette opération et le montant
                {
                    j++;
                }
                if(j==10) //si il est plein, on supprime le plus ancien et ajouter le plus récente
                {
                    for (int k=0; k<9; k++) 
                    {
                        c.compte[i].operation[k]=c.compte[i].operation[k+1];
                    }
                    c.compte[i].operation[9].type = "Ajout";
                    c.compte[i].operation[9].montant = somme;
                    c.compte[i].operation[9].date = ctime(&rawtime);
                }

                else //vérifier si le tableau d'opération est plein ou pas 
                {
                    c.compte[i].operation[j].type = "Ajout";
                    c.compte[i].operation[j].montant = somme;
                    c.compte[i].operation[j].date = ctime(&rawtime);
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
            if(c.compte[i].id_compte==compte && (strcmp(c.compte[i].password,password)==0))
            {
                c.compte[i].solde -=somme; //versement
                res = 0;
                int j=0;
                while (j<10 && (c.compte[i].operation[j].type !=NULL)) //enregistrer l'opération, la date de cette opération et le montant
                {
                    j++;
                }
                if(j==10) //si il est plein, on supprime le plus ancien et ajouter le plus récente
                {
                    for (int k=0; k<9; k++) 
                    {
                        c.compte[i].operation[k]=c.compte[i].operation[k+1];
                    }
                    c.compte[i].operation[9].type = "Retrait";
                    c.compte[i].operation[9].montant = somme;
                    c.compte[i].operation[9].date = ctime(&rawtime);
                }

                else //vérifier si le tableau d'opération est plein ou pas 
                {
                    c.compte[i].operation[j].type = "Retrait";
                    c.compte[i].operation[j].montant = somme;
                    c.compte[i].operation[j].date = ctime(&rawtime);
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
            if(c.compte[i].id_compte==compte && (strcmp(c.compte[i].password,password)==0))
            {
                int cpt=0;
                while (c.compte[i].operation[cpt].type !=NULL) //chercher le plus récent opération
                {
                    cpt++;
                }
                //afficher le montant actuel et la date du dernier opération
                if (cpt!=0)
                    printf("Dernier opération : %s %d€ %s \n",c.compte[i].operation[cpt-1].type, c.compte[i].operation[cpt-1].montant, c.compte[i].operation[cpt-1].date); 
                printf("Solde:%d \n",c.compte[i].solde); 
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
            if(c.compte[i].id_compte==compte && (strcmp(c.compte[i].password,password)==0))
            {
                //afficher le date, le montant et le type des opérations en ordre le plus récent jusqu'à le plus ancien
                for (int j=9; j>=0; j--)
                {
                    if (c.compte[i].operation[j].type != NULL)
                        printf("%s  date: %s  montant:%d \n",c.compte[i].operation[j].type,
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
    char password[10];
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
        sscanf(demande,"%d",&id_client);
        printf("Client connecté %d\n",id_client);
        bzero(demande, MAX);

        //capter le ID du compte
        strcpy(reponse,"Entré Id du compte : ");
        write(sockfd,reponse,sizeof(reponse));
        bzero(reponse, MAX);  
        read(sockfd, demande, sizeof(demande));
        sscanf(demande,"%d",&id_compte);
        printf("compte connecté : %d\n",id_compte);
        bzero(demande, MAX);

        //capter le mot de passe
        strcpy(reponse,"Entré le mot de passe : ");
        write(sockfd,reponse,sizeof(reponse));
        bzero(reponse, MAX);  
        read(sockfd, demande, sizeof(demande)); 
        //strcpy(password, demande);
        int n=0;
        while(isalpha(demande[n]) || isdigit(demande[n]))
        {
            password[n]=demande[n];
            n++;
        }
        password[n]='\0';
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
            sscanf(demande,"%d",&somme);
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
            sscanf(demande,"%d",&somme);
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

    for (int i=0; i<10; i++)
    {
        p[i].id_compte=i;
        p[i].password="jin";
        p[i].solde=i*10;
    }

    c.id_client=1;
    for (int i=0; i<10; i++)
    {
        c.compte[i]=p[i];
    }
    
  
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
