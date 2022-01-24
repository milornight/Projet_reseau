// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <unistd.h>
#include <time.h>

#define PORT	 8080 
#define MAXLINE 1024
#define nbCli 3
struct sockaddr_in servaddr, cliaddr;

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

// Initialisation des paramètres de client
COMPTE p[10];
CLIENT c[nbCli];
time_t rawtime;


int ajout(int client, int compte, char *password, int somme)
{     
    int res = -1;
    for(int k = 0; k<nbCli; k++)
    {
	    if (c[k].id_client==client) // vérifier si ce client existe
	    {
		for (int i = 0; i<10; i++)
		{
		     // vérifier s'il a un compte correspond et le mot de passe est correcte ou pas
		    if(c[k].compte[i].id_compte==compte && (strcmp(c[k].compte[i].password,password)==0))
		    {
		        c[k].compte[i].solde +=somme; //versement
		        res = 0;
		        int j=0;
		        // enregistrer l'opération, la date de cette opération et le montant
		        while (j<10 && c[k].compte[i].operation[j].type !=NULL) 
		        {
		            j++;
		        }
		        // si le tableau de l'opération est plein
		        // on supprime le plus ancien et ajouter le plus récente
		        if(j==10) 
		        {
		            for (int h=0; h<9; h++) 
		            {
		                c[k].compte[i].operation[h]=c[k].compte[i].operation[h+1];
		            }
		            c[k].compte[i].operation[9].type = "Ajout";
		            c[k].compte[i].operation[9].montant = somme;
		            c[k].compte[i].operation[9].date = ctime(&rawtime);
		        }
		        else //vérifier si le tableau d'opération est plein ou pas 
		        {
		            c[k].compte[i].operation[j].type = "Ajout";
		            c[k].compte[i].operation[j].montant = somme;
		            c[k].compte[i].operation[j].date = ctime(&rawtime);
		        }
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
    for(int k = 0; k < nbCli; k++)
    {
	    if (c[k].id_client==client)
	    {
		for (int i = 0; i<10; i++)
		{
		    if(c[k].compte[i].id_compte==compte && (strcmp(c[k].compte[i].password,password)==0))
		    {
		        c[k].compte[i].solde -=somme; //Retrait
		        res = 0;
		        int j=0;
		        while (j<10 && (c[k].compte[i].operation[j].type !=NULL)) 
		        {
		            j++;
		        }
		        
		        if(j==10) 
		        {
		            for (int h=0; h<9; h++) 
		            {
		                c[k].compte[i].operation[h]=c[k].compte[i].operation[h+1];
		            }
		            c[k].compte[i].operation[9].type = "Retrait";
		            c[k].compte[i].operation[9].montant = somme;
		            c[k].compte[i].operation[9].date = ctime(&rawtime);
		        }
		        else 
		        {
		            c[k].compte[i].operation[j].type = "Retrait";
		            c[k].compte[i].operation[j].montant = somme;
		            c[k].compte[i].operation[j].date = ctime(&rawtime);
		        }     
		    }
		}
	    }
    }
    return res;
}


void solde(int client, int compte, char *password, int sockfd)
{
    char str[MAXLINE];
    for(int k = 0; k <nbCli; k++)
    {
	    if(c[k].id_client==client) //vérifier si ce client existe
	    {
		for (int i = 0; i<10; i++) 
		{
		    //vérifier s'il a un compte correspond et le mot de passe est correcte ou pas
		    if(c[k].compte[i].id_compte==compte && (strcmp(c[k].compte[i].password,password)==0))
		    {
		        int cpt=0;
		        //chercher le plus récent opération et ne pas dépasser la taille du tableau
		        while (c[k].compte[i].operation[cpt].type !=NULL && cpt<10) 
		        {
		            cpt++;
		        }
		        //stoker le solde actuel dans le chaine str
		        sprintf(str, "RES_SOLDE:%d\n", c[k].compte[i].solde);
		       
		        //si le tableau n'est pas vide, stoker la dernière opération dans le chaine str2
		        if (cpt!=0){
		            char str2[MAXLINE];
		            sprintf(str2,"Dernier opération : %s %d€ %s\n",c[k].compte[i].operation[cpt-1].type, 
		                    c[k].compte[i].operation[cpt-1].montant, 
		                    c[k].compte[i].operation[cpt-1].date); 
		            strcat(str, str2); //concaténer les 2 chaines
		        }
		        
		        //afficher le solde actuel et la dernière opération
		       sendto(sockfd, str, sizeof(str), MSG_CONFIRM, 
		       	(const struct sockaddr *) &cliaddr, 
		   		sizeof(servaddr));
	   		//printf("\n********SOLDE******\n%s",str);
		    }
		}
	    }
    }
}

void operations(int client, int compte, char *password,int sockfd)
{
    char str[MAXLINE];
    for (int k = 0; k< nbCli; k++)
    {
	    if(c[k].id_client==client) //vérifier si ce client existe
	    {
		for (int i = 0; i<10; i++)
		{
		    //vérifier s'il a un compte correspond et le mot de passe est correcte ou pas
		    if(c[k].compte[i].id_compte==compte && (strcmp(c[k].compte[i].password,password)==0))
		    {
		        // afficher la date, le montant et le type des opérations 
		        // en ordre le plus récent jusqu'à le plus ancien
		        for (int j=9; j>=0; j--)
		        {
		            if (c[k].compte[i].operation[j].type != NULL)
		            {
		                char str2[256];
		                sprintf(str2,"%s - date: %s - montant:%d\n",c[k].compte[i].operation[j].type,
		                                                            c[k].compte[i].operation[j].date,
		                                                            c[k].compte[i].operation[j].montant);
		                strcat(str, str2);                                     
		            }
		        }
		        sendto(sockfd, str, sizeof(str), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
				//printf("\n*****OPÉRATION LISTE*****\n%s",str);
		    }
		} 
	    }
    }   
}

int len, n;

void commu(int sockfd) 
{ 
   char buffer[MAXLINE]; // datagramme de client
   char reponse[MAXLINE];
   int id_client;
   int id_compte;
   char password[10];
   int somme;
   char oper[10];
   
    //n = 0;
    for (;;) { 
	len = sizeof(cliaddr); //len is value/resuslt 
	bzero(buffer,MAXLINE);
	bzero(reponse,MAXLINE);
	id_client = 0;
	id_compte = 0;
	somme = 0;
	//n=0;
	bzero(password,10);
	bzero(oper,10);
	
	// Recevoir le datagram de client et l'afficher
	recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				&len); 
	
	printf("\nClient : %s\n", buffer);
	
	/*
	Traiter la demande de client (datagramme de client)
	    5 types de DEMANDE: AJOUT RETRAIT SOLDE OPERATION et EXIT
	    - si la DEMANDE est AJOUT ou RETRAIT
	    	la forme de demande doit être : 
	        DEMANDE <id_client id_compte password somme> 
	    - si la DEMANDE est SOLDE ou OPRATION
	    	la forme de demande doit être : 
	    	DEMANDE <id_client id_compte password >
	    - si la DEMANDE est EXIT
	    	la forme de demande doit être :
	    	EXIT
	*/
	//on récupérer les infos de la demande
	int total_demande;
 	total_demande = sscanf(buffer,"%s%d%d%s%d",oper,&id_client,&id_compte,password,&somme);
 	printf("\nTotal Info: %d ",total_demande);
 	printf("\nOpération demandée de client: %s ",oper);
	printf("\nID_client: %d ",id_client);
	printf("\nID_compte: %d ",id_compte);
	printf("\nPasseword: %s ",password);
	printf("\nMontant : %d ",somme); 
	if (total_demande == 5){
		if(strncmp("Ajout",oper,5) == 0){
			int res;
			res = ajout(id_client, id_compte,password,somme);
			if(res == -1){
				strcpy(reponse, "KO");
			}else{
				strcpy(reponse, "OK");
			}
			//printf("\n\t*****TEST*****Message sent of AJOUT.\n");
			// Envoyer la réponse de serveur
			sendto(sockfd, reponse, sizeof(reponse), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
		}
		else if(strncmp("Retrait",oper,7) == 0){
			int res;
			res = retrait(id_client, id_compte,password,somme);
			if(res == -1){
				strcpy(reponse, "KO");
			}else{strcpy(reponse, "OK");}
			//printf("\n\t*****TEST*****Message sent of RETRAIT.\n");
			// Envoyer la réponse de serveur
			sendto(sockfd, reponse, sizeof(reponse), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
		} // si operation saisie n'est pas correcte
		else{
			strcpy(reponse, "KO Votre operation n'est pas correcte");
			sendto(sockfd, reponse, sizeof(reponse), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
		}
	}else if (total_demande == 4){
		if(strncmp("Solde",oper,5) == 0){
			solde(id_client,id_compte,password,sockfd);
			//printf("\n\t*****TEST*****Message sent of SOLDE.\n");
		}
		else if(strncmp("Operation",oper,10) == 0){
			operations(id_client,id_compte,password,sockfd);
			//printf("\n\t*****TEST*****Message sent of OPERATION.\n");
		}
		else{
			strcpy(reponse, "KO Votre operation n'est pas correcte");
			sendto(sockfd, reponse, sizeof(reponse), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
		}	
	}else if(total_demande == 1){
		if(strncmp("Yes",oper,3) == 0){
			printf("Client Exit...\n");
			strcpy(reponse, "EXIT");
			sendto(sockfd, reponse, sizeof(reponse), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
			break;
		}else if(strncmp("No",oper,2) == 0){
			strcpy(reponse, "CONTINUE");
			sendto(sockfd, reponse, sizeof(reponse), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
		}else if(strncmp("Exit",oper,4)==0){
			strcpy(reponse, "Vous voulez quitter la session? [Yes / No]");
			sendto(sockfd, reponse, sizeof(reponse), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
		}else{
			strcpy(reponse, "KO Votre demande n'est pas correcte");
			sendto(sockfd, reponse, sizeof(reponse), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
	}else{
		strcpy(reponse, "KO IL MANQUE/IL Y A TROPE DES INFORMATIONS");
		sendto(sockfd, reponse, sizeof(reponse), 
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				sizeof(servaddr));
	}
	}
	//printf("\n\t*****TEST*****Derniere etape de BOUCLE FOR.\n"); 
    }
}

// Driver code 
int main() { 
	/*
		Initialisation ID de client, ID de compte, password et solde
	
	*/
    for (int i=0; i<10; i++)
    {
        p[i].id_compte=100+i;  // IDs de compte sont: 100, 101, 102...
        p[i].password="eise";	// password par defaut : "eise"
        p[i].solde=i*10;	// solde par defaut : 10,20,30...
    }
    for (int j=0; j<nbCli; j++)
    {
    	c[j].id_client = j;
    	for(int k = 0; k<10; k++)
    	{
        	c[j].compte[k]=p[k]; // les infos de ce client prend les infos de compte p
        		//que on vient d'initialiser en dessus
        }
    }
	int sockfd; 
	struct sockaddr_in servaddr, cliaddr; 
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(PORT); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 

	commu(sockfd);
	close(sockfd);
	return 0; 
} 
