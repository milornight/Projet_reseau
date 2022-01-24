// Client side implementation of UDP client-server model 
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
struct sockaddr_in	 servaddr;

 void commu(int sockfd) 
{ 
    int n; 
    char buffer[MAXLINE]; 
    char demande_client[MAXLINE];
	int res = 0;
    for (;;) { 
    	n = 0;
    	// mise en zero la demande de client et la réponse de serveur
    	bzero(demande_client,MAXLINE);
	bzero(buffer,MAXLINE);
	printf("\n\t*****Envoyer un datagram******\n");
	while ((demande_client[n++]= getchar()) != '\n');
	demande_client[n]='\0';
	sendto(sockfd, demande_client, sizeof(demande_client), MSG_CONFIRM, 
		(const struct sockaddr *) &servaddr, sizeof(servaddr));
	//printf("\n*****TEST*****Message sent from Client : %s\n", demande_client); 
	//printf("****Test*********");
	recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, 
			(struct sockaddr *) &servaddr, NULL);
	printf("\nServer : %s\n", buffer);		
	if(strncmp("EXIT",buffer,4) == 0){
		printf("___AU REVOIR___");
		break;
	}	
    }
}    


// Driver code 
int main() { 
	int sockfd; 
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 

	commu(sockfd);
	// fermeture le socket
	close(sockfd); 
	return 0; 
} 
