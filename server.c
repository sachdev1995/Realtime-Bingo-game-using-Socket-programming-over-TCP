#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define MAX 500 
//#define PORT 8080 
#define SA struct sockaddr 

int MYPORT ;
char my_ip_addr[10];
int counter =0;

int sockfd;
struct sockaddr_in servaddr, cli;   
int client_count =0; 
  
struct client 
{
	int port;
    char ip_addr[10];
	char client_name[20];
	char incoming_msg[MAX];
	char outgoing_msg[MAX];
	pthread_t tid;
	int sockfd ,connfd, len;
    struct sockaddr_in servaddr, cli; 
	pthread_mutex_t lock;
    pthread_attr_t attr;
    int isValid;
    int isInvolved;
	
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
    //strcpy(a[counter]->client_name,"Client");
    //sprintf(append,"%d",counter);
    //strcat(a[counter]->client_name,append);

    
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
            printf("%d and counter %d\n",local_player_count,counter );
            if(((counter - local_player_count) >=1 ) )
            {
                int local_count =0 ;
                for(int i = 0; i< counter;i++)
                {
                    if((a[i]->isValid == 1) && (strcmp(a[i]->client_name,currentPointer->client_name) != 0) )
                    {
                        if(local_count<local_player_count)

                        {
                            local_count++;
                        }

                    }
                }
                bzero(buff, sizeof(buff));
                sprintf(buff,"connect player=%d ",local_count);

                for(int i = 0; i< counter;i++)
                {
                    if((a[i]->isValid == 1) && (strcmp(a[i]->client_name,currentPointer->client_name) != 0) )
                    {
                        char append[30];
                        sprintf(append,"port=%d ip_addr=%s name=%s ",a[i]->port,a[i]->ip_addr,a[i]->client_name);
                        strcat(buff,append);
                    }
                }

                printf("Sending connect command '%s' to %s...\n",buff,currentPointer->client_name);
                
            }
            else
            {
                bzero(buff, sizeof(buff));
                strcpy(buff,"insufficient players");
                printf("number of players are not sufficient\n");
                    
                //bzero(buff, MAX);
                //sprintf(buff,"connect player=2 port=8081 port=8082\n");
            }
            

            
        }
        else if((strncmp(buff,"query_players",12))==0)
        {
            bzero(buff, sizeof(buff));
            int local_player_count = 0;
            for (int i =0 ;i< counter;i++)
            {
                if(a[i]->isValid ==1)
                    local_player_count++;

            }
            sprintf(buff,"players=%d ",local_player_count);
            for(int i = 0; i< counter;i++)
            {
                if(a[i]->isValid ==1){
                char append[90];
                sprintf(append,"port=%d ip_addr=%s name=%s ",a[i]->port,a[i]->ip_addr,a[i]->client_name);
                strcat(buff,append);
                //printf("valid pin of name %s  i =%d and pin = %d\n",a[i]->client_name,i,a[i]->isValid);
                }
            }
            printf("sending player list %s\n",buff);

        }
        else if ((strncmp(buff, "my_port", 6)) == 0) 
        { 
            int local_port_numer;
            char local_client_name[10];
            char local_client_ip[10];
            sscanf(buff,"my_port=%d my_ipaddr=%s my_name=%s",&local_port_numer,&local_client_ip,&local_client_name);
            for(int i = 0;i<counter;i++)
            {
                if(a[i]->isValid == 1){
                if(strncmp(a[i]->client_name,local_client_name,10) == 0)
                {
                    printf("client name is not unique\n");
                    bzero(buff, MAX);
                    sprintf(buff,"not registered");
                    write(currentPointer->connfd, buff, sizeof(buff));
                    currentPointer->isValid = 0;

                    
                    
                    pthread_exit(NULL);
                }}
            }
            currentPointer->isValid = 1;
            currentPointer->port = local_port_numer;
            strcpy(currentPointer->ip_addr,local_client_ip);
            strcpy(currentPointer->client_name,local_client_name);
            //sscanf(buff,"my_port=%d my_ipaddr=%s my_name=%s",&currentPointer->port,&currentPointer->ip_addr,&currentPointer->client_name);
            printf("Client's port number is  %d ,ip address is %s and name is %s...\n",currentPointer->port,currentPointer->ip_addr,currentPointer->client_name);
            bzero(buff, MAX);
            sprintf(buff,"registered");
            

            
        }
        else if((strncmp(buff,"deregister",9))==0)
        {
            if(currentPointer->isInvolved == 1)
            {
                bzero(buff, MAX);
                sprintf(buff,"fail"); 
            }
            else 
            {
                bzero(buff, MAX);
                sprintf(buff,"sucess");
                currentPointer->isValid = 0;
            }
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
    //printf(" enter ipaddress of this machine\n");
    //scanf("%s",&my_ip_addr);
    //printf("enter port number of this machine\n");
    //scanf("%d",&MYPORT);
    MYPORT = 8080;
    strcpy(my_ip_addr,"127.0.0.1");

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
    servaddr.sin_addr.s_addr = inet_addr(my_ip_addr); 
    servaddr.sin_port = htons(MYPORT); 
  
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) 
    { 
        printf("socket bind failed as server port is not free, please try again after sometime  ...\n"); 
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

