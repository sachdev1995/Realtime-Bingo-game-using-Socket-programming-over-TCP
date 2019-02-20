#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define MAX 80 
//#define PORT 8080 
#define SA struct sockaddr 
int port = 8080;
int counter =0;
int dummy= 1;

int sockfd;
struct sockaddr_in servaddr, cli;   
int client_count =0; 
  
struct client 
{

	int port;
	char client_name[20];
	char incoming_msg[MAX];
	char outgoing_msg[MAX];
	pthread_t tid;
	int sockfd ,connfd, len;
    struct sockaddr_in servaddr, cli; 
	pthread_mutex_t lock;
    pthread_attr_t attr;
	
}client_struct_variable;

struct client *a[10];

void *tFunc(struct client *currentPointer)
{
    printf("inside tfunc function for client %s \n",currentPointer->client_name);
    currentPointer->sockfd = sockfd;

    
    currentPointer->len = sizeof(currentPointer->cli); 
  
    currentPointer->connfd = accept(currentPointer->sockfd, (SA*)&currentPointer->cli, &currentPointer->len); 
    if (currentPointer->connfd < 0) 
    { 
        printf("server acccept failed for %s...\n",currentPointer->client_name); 
        pthread_exit(NULL); 
    } 
    else
    {
        printf("server acccept the client for %s...\n",currentPointer->client_name); 
        
    }
 
    func(currentPointer); 

    close(currentPointer->connfd); 
}

void func(struct client *currentPointer) 
{ 
    printf("inside func function for client  \n");
    char buff[MAX]; 
    int n; 
    counter++;
    
    printf("\n \ncreating thread %d \n",counter);
    char append[2];

     a[counter] = malloc(sizeof(struct client));



        
    //a[counter]->port = 8080;
    strcpy(a[counter]->client_name,"Client");
    sprintf(append,"%d",counter);
    strcat(a[counter]->client_name,append);

    
    pthread_create(&a[counter]->tid,&a[counter]->attr,tFunc,a[counter]);
    
    for (;;) 
    { 
        bzero(buff, MAX); 
  	
        read(currentPointer->connfd, buff, sizeof(buff)); 
        //printf("From %s: %s\t ",currentPointer->client_name, buff); 
        if ((strncmp(buff, "exit", 4)) == 0) 
        { 
            printf("Server closing connection for %s since %s left...\n",currentPointer->client_name,currentPointer->client_name); 
            return ; 
        }
        else if ((strncmp(buff, "start_game", 4)) == 0) 
        { 
                

            int local_player_count;
            sscanf(buff,"start_game count=%d",&local_player_count);
            printf("%d\n",local_player_count );
            bzero(buff, sizeof(buff));
            sprintf(buff,"connect player=%d ",local_player_count);

            for(int i = 0; i< local_player_count;i++)
            {
                char append[15];
                sprintf(append,"port=%d ",a[i]->port);
                strcat(buff,append);
            }

            printf("Sending connect command '%s' to %s...\n",buff,currentPointer->client_name);
            //bzero(buff, MAX);
            //sprintf(buff,"connect player=2 port=8081 port=8082\n");
            

            
        }
        else if ((strncmp(buff, "my_port", 6)) == 0) 
        { 
            sscanf(buff,"my_port=%d",&currentPointer->port);
            printf("Client's port number is  %d...\n",currentPointer->port);
            bzero(buff, MAX);
            sprintf(buff,"port number registered\n");
            

            
        }
        else
        {
            continue;
        }

        //bzero(buff, MAX); 
        n = 0; 
        
        //while ((buff[n++] = getchar()) != '\n') ;
             
        printf("Sending msg to  %s\n  : ",currentPointer->client_name );
       write(currentPointer->connfd, buff, sizeof(buff)); 
 	
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Self connection closed for %s...\n",currentPointer->client_name); 
            break; 
            //exit(0);
        }
        else if (strncmp("close", buff, 5) == 0) 
        { 
            printf("Server totally closing down\n"); 
            //break; 
            exit(0);
        } 


    } 

    pthread_exit(NULL);
} 



int create_thread(int fcount)
{
    printf("\n \ncreating thread %d \n",fcount);
    char append[2];

     a[fcount] = malloc(sizeof(struct client));



        
    //a[fcount]->port = 8080;
    strcpy(a[fcount]->client_name,"Client");
    sprintf(append,"%d",fcount);
    strcat(a[fcount]->client_name,append);
    

    pthread_create(&a[fcount]->tid,&a[fcount]->attr,tFunc,a[fcount]);
    pthread_exit(NULL);

    return ;

}

void query_players()
{
    int i = 0;
    while(a[i+1]!= NULL)
    {
        printf("%s is connected to port %d\n",a[i]->client_name,a[i]->port);
        i++;
    }
}

void main()
{
    printf("counter value =%d\n",counter);
	printf("in main function\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
    { 
        printf("socket creation failed  ...\n"); 
        return; 
    } 
    else
        printf("Socket successfully created  ..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
    
    
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port); 
  
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) 
    { 
        printf("socket bind failed  ...\n"); 
        pthread_exit(NULL);  
        //return;
    } 
    else
        printf("Socket successfully binded  ..\n");

    if ((listen(sockfd, 5)) != 0) 
    { 
        printf("Listen failed  ...\n"); 
        pthread_exit(NULL); 
    } 
    else
        printf("Server listening  ..\n"); 

    //create_thread(counter);

     a[counter] = malloc(sizeof(struct client));



    char append[2];    
    //a[counter]->port = 8080;
    strcpy(a[counter]->client_name,"Client");
    sprintf(append,"%d",counter);
    strcat(a[counter]->client_name,append);
    pthread_create(&a[counter]->tid,&a[counter]->attr,tFunc,a[counter]);

    
    sleep(20);
    
    query_players();
    pthread_exit(NULL);

}

