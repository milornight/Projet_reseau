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

                //Boucle pour chercher la position du dernier opérations
                int j=0;
                while (j<10 && c.compte[i].operation[j].type !=NULL) 
                {
                    j++;
                }

                if(j==10) //le tableau est plein
                {   
                    //on supprime le plus ancien et ajouter le plus récente opération
                    for (int k=0; k<9; k++) 
                    {
                        c.compte[i].operation[k]=c.compte[i].operation[k+1];
                    }
                    c.compte[i].operation[9].type = "Ajout";
                    c.compte[i].operation[9].montant = somme;
                    c.compte[i].operation[9].date = ctime(&rawtime);
                }
                else //le tableau d'opération n'est pas plein, on ajoute l'opération actuel dans le tableau
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
                c.compte[i].solde -=somme; //Retrait
                res = 0;
                int j=0;
                while (j<10 && (c.compte[i].operation[j].type !=NULL)) 
                {
                    j++;
                }
                
                if(j==10) 
                {
                    for (int k=0; k<9; k++) 
                    {
                        c.compte[i].operation[k]=c.compte[i].operation[k+1];
                    }
                    c.compte[i].operation[9].type = "Retrait";
                    c.compte[i].operation[9].montant = somme;
                    c.compte[i].operation[9].date = ctime(&rawtime);
                }
                else 
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


void solde(int client, int compte, char *password, int sockfd)
{
    char str[256];
    if(c.id_client==client) //vérifier si ce client existe
    {
        for (int i = 0; i<10; i++) 
        {
            //vérifier s'il a un compte correspond et le mot de passe est correcte ou pas
            if(c.compte[i].id_compte==compte && (strcmp(c.compte[i].password,password)==0))
            {
                int cpt=0;
                //chercher le plus récent opération et ne pas dépasser la taille du tableau
                while (c.compte[i].operation[cpt].type !=NULL && cpt<10) 
                {
                    cpt++;
                }
                //stoker le solde actuel dans le chaine str
                sprintf(str, "Solde:%d\n", c.compte[i].solde);
                //si le tableau n'est pas vide, stoker la dernière opération dans le chaine str2
                if (cpt!=0){
                    char str2[256];
                    sprintf(str2,"Dernier opération : %s %d€ %s\n",c.compte[i].operation[cpt-1].type, 
                                                                            c.compte[i].operation[cpt-1].montant, 
                                                                            c.compte[i].operation[cpt-1].date); 
                    strcat(str, str2); //concaténer les 2 chaines
                }
                //afficher le solde actuel et la dernière opération
                write(sockfd,str,sizeof(str));
            }
        }
    }
}

void operations(int client, int compte, char *password,int sockfd)
{
    char str[MAX];
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
                    {
                        char str2[256];
                        sprintf(str2,"%s  date: %s  montant:%d \n",c.compte[i].operation[j].type,
                                                                    c.compte[i].operation[j].date,
                                                                    c.compte[i].operation[j].montant);
                        strcat(str, str2);                                     
                    }
                }

                write(sockfd,str,sizeof(str));
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
            //capter le montant qu'on veut verser
            bzero(demande, MAX);
            strcpy(reponse,"Entré le montant : ");
            write(sockfd,reponse,sizeof(reponse));
            bzero(reponse, MAX);  
            read(sockfd, demande, sizeof(demande)); 
            sscanf(demande,"%d",&somme);
            bzero(demande, MAX);
            int Res;
            Res = ajout(id_client,id_compte,password,somme);

            //vérifier si l'opération demandé a bien déroulé
            if(Res==-1)
            {
                strcpy(reponse,"KO");
                write(sockfd,reponse,sizeof(reponse));
                read(sockfd, demande, sizeof(demande));  
            }else{
                strcpy(reponse,"OK");
                write(sockfd,reponse,sizeof(reponse)); 
                read(sockfd, demande, sizeof(demande)); 
            }
        }
        
        if (strncmp("Retrait", demande, 7) == 0)
        {
            //capter le montant qu'on veut retirer
            bzero(demande, MAX);
            strcpy(reponse,"Entré le montant : ");
            write(sockfd,reponse,sizeof(reponse));
            bzero(reponse, MAX);  
            read(sockfd, demande, sizeof(demande)); 
            sscanf(demande,"%d",&somme);
            bzero(demande, MAX);
            int Res;
            Res = retrait(id_client,id_compte,password,somme);
            
            if(Res==-1)
            {
                strcpy(reponse,"KO");
                write(sockfd,reponse,sizeof(reponse));
                read(sockfd, demande, sizeof(demande));  
            }else{
                strcpy(reponse,"OK");
                write(sockfd,reponse,sizeof(reponse));
                read(sockfd, demande, sizeof(demande));  
            }
        }
        
        if (strncmp("Solde", demande, 5) == 0)
        {
            solde(id_client,id_compte,password, sockfd);
            read(sockfd, demande, sizeof(demande)); 
        }
        
        if(strncmp("Operations", demande, 10) == 0)
        {
            operations(id_client,id_compte,password,sockfd);
            read(sockfd, demande, sizeof(demande)); 
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

    //initialisation du client et ses comptes
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
