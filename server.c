#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include<stdbool.h>
#define MAX 500 

#define SA struct sockaddr 

int MYPORT ;
char my_ip_addr[20];

int counter =0;

int sockfd;
struct sockaddr_in servaddr, cli;   
int client_count =0; 

/*
default data struct for each client
connecting to the server
*/

  
struct client 
{
	int port;
    char ip_addr[20];
	char client_name[20];
	pthread_t tid;
	int sockfd ,connfd, len;
    struct sockaddr_in servaddr, cli; 
	pthread_mutex_t lock;
    pthread_attr_t attr;
    int isValid;
    int isInvolved;
	
}client_struct_variable;

struct client *a[40];


/*
Below is the data struct for
storing the database of the game

*/
struct game_Structure
{
    int game_id;
    char game_members[MAX];
    int isOnGoing;
    int player_count;
}game_struct_variable;

int game_counter = 0;

struct game_Structure *game[10];

/*
Below is the function to check
if a client is involved in any game
If yes, then that cannot deregister the game
*/

void check_for_isInvolved(int current_game_id)
{
    int local_game_player_count = game[current_game_id]->player_count;
    char *current_player_list;
    current_player_list = malloc(sizeof(game[current_game_id]->game_members));
    memcpy(current_player_list,game[current_game_id]->game_members,strlen(game[current_game_id]->game_members)+1);
    printf("%s\n",current_player_list);
    char portStr[] = "port=";
    current_player_list = strstr(current_player_list,portStr);
     
    for(int i = 0; i<local_game_player_count;i++)
    {
        int num;
        char localip_addr[20];
        char localname[10];
        sscanf(current_player_list,"port=%d ip_addr=%s name=%s ",&num,&localip_addr,&localname);
        int flag = 0;
        
        for(int j = 0 ;j< game_counter;j++)
        {
            if((game[j]->isOnGoing == 1) && j != current_game_id)
            {
                if(strstr(game[j]->game_members, localname) != NULL)
                {
                    flag = 1;

                }
            }
             
        }

        if(flag == 0)
        {
            //printf("find its array\n");
            for(int k= 0; k< counter;k++)
            {
                //printf("comparing %s with %s\n",localname,a[k]->client_name);
                if(a[k]->isValid == 1)
                {
                    if(strstr(a[k]->client_name, localname) != NULL)
                    {
                        a[k]->isInvolved = 0;
                        //printf("it worked\n");
                        
                    }
                }
                
            }
        }
        
        
        current_player_list++;
        current_player_list = strstr(current_player_list,portStr);
        bzero(localname,sizeof(localname));
        bzero(localip_addr,sizeof(localip_addr));
    }
}

/*
Handler functon for Server connection with individual client
*/


void *tFunc(struct client *currentPointer)
{
    //printf("inside tfunc function for client %s \n",currentPointer->client_name);
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

/*
Below function is the extension for extension to the above handler
function
The below funcion actually transfer data
to the client whenever it asks for a service

*/


void func(struct client *currentPointer) 
{ 
    printf("inside func function for client  \n");
    char buff[MAX]; 
    int n; 
    counter++;
    
    printf("\n \ncreating thread %d \n",counter);
    char append[2];
    a[counter] = malloc(sizeof(struct client));
    pthread_create(&a[counter]->tid,&a[counter]->attr,tFunc,a[counter]);
    
    for (;;) 
    { 
        bzero(buff, MAX); 
  	
        read(currentPointer->connfd, buff, sizeof(buff)); 
        if ((strncmp(buff, "exit", 4)) == 0) 
        { 
            printf("Server closing connection for %s since %s left...\n",currentPointer->client_name,currentPointer->client_name); 
            return ; 
        }

        /*
        if the wants to start a game, then below command are 
        executed. It sends a list of valid clients/players
        that are currently online in the system
        */

        else if ((strncmp(buff, "start_game", 4)) == 0) 
        { 
            
            game[game_counter] = malloc(sizeof(struct game_Structure));
            game[game_counter]->game_id = game_counter;
            game[game_counter]->isOnGoing = 1;

            int local_player_count;
            sscanf(buff,"start_game count=%d",&local_player_count);
            //printf("%d and counter %d and game counter %d\n",local_player_count,counter,game_counter );
            if(((counter - local_player_count) >=1 ) )
            {
                int local_count =0 ;
                for(int i = 0; i< counter; i++)
                {
                    if((a[i]->isValid == 1) && (strcmp(a[i]->client_name,currentPointer->client_name) != 0) )
                    {
                        if(local_count<local_player_count)

                        {
                            local_count++;
                        }

                    }
                }
                game[game_counter]->player_count = local_count;
                bzero(buff, sizeof(buff));
                sprintf(buff,"connect game_id=%d player=%d ",game_counter,local_count);
                char game_info_string[MAX];

                for(int i = 0; i< counter;i++)
                {
                    if((a[i]->isValid == 1) && (strcmp(a[i]->client_name,currentPointer->client_name) != 0) )
                    {
                        char append[90];
                        
                        sprintf(append,"port=%d ip_addr=%s name=%s ",a[i]->port,a[i]->ip_addr,a[i]->client_name);
                        
                        strcat(buff,append);
                        
                        a[i]->isInvolved = 1;
                    }
                }
                
                sprintf(game[game_counter]->game_members,"caller_name=%s player_list: %s",currentPointer->client_name,buff+17);
                printf("game info buff %s and actual data\n",buff,game[game_counter]->game_members);
                printf("Sending connect command '%s' to %s...\n",buff,currentPointer->client_name);
                game_counter++;
                
            }
            else
            {
                bzero(buff, sizeof(buff));
                strcpy(buff,"insufficient players");
                printf("number of players are not sufficient\n");
            }
        }

        /*
        When a client tries to query the list players
        the server send it a list of all active players

        */

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
                
                }
            }
            printf("sending player list %s\n",buff);
        }

        /*
        the below command are executed when
        a client tries to register with the server/manager
        */
        else if ((strncmp(buff, "my_port", 6)) == 0) 
        { 
            int local_port_numer;
            char local_client_name[10];
            char local_client_ip[20];
            sscanf(buff,"my_port=%d my_ipaddr=%s my_name=%s",&local_port_numer,&local_client_ip,&local_client_name);
            for(int i = 0;i<counter;i++)
            {
                if(a[i]->isValid == 1)
                {
                    if(strncmp(a[i]->client_name,local_client_name,10) == 0)
                    {
                        printf("client name is not unique\n");
                        bzero(buff, MAX);
                        sprintf(buff,"not registered");
                        write(currentPointer->connfd, buff, sizeof(buff));
                        currentPointer->isValid = 0;
                        pthread_exit(NULL);
                    }
                }
            }
            currentPointer->isValid = 1;
            currentPointer->port = local_port_numer;
            strcpy(currentPointer->ip_addr,local_client_ip);
            strcpy(currentPointer->client_name,local_client_name);
            printf("Client's port number is  %d ,ip address is %s and name is %s...\n",currentPointer->port,currentPointer->ip_addr,currentPointer->client_name);
            bzero(buff, MAX);
            sprintf(buff,"registered");            
        }

        /*
        When a client tries to deregister itself
        the server check if it is involved in some
        game, if not then the server allows the 
        client to exit the system
        */
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
        /*
        The below function is send the list of 
        all active game to the client
        */

        else if((strncmp(buff,"query_games",11))==0)
        {
            
            bzero(buff, MAX);
            int local_game_count =0 ;
            for(int i =0; i<game_counter;i++)
            {
                if(game[i]->isOnGoing == 1)
                {
                    local_game_count++;

                }
            }
            char game_count_append_String[30];
            sprintf(game_count_append_String,"game_count=%d ",local_game_count); 
            strcat(buff,game_count_append_String);
            for(int i =0; i<game_counter;i++)
            {
                if(game[i]->isOnGoing == 1)
                {
                    char append[40];

                    sprintf(append,"game_id=%d ",game[i]->game_id);
                    strcat(buff,append);
                    strcat(buff,game[i]->game_members);
                    strcat(buff," ");

                }
            }
            
        }

        /*
        the below command is the end identifier
        When a caller finds a winner of the game
        it informs the server by this end identifier
        the server then updates it databases accordingly
        */

        else if((strncmp(buff,"end_game",8))==0)
        {
            int local_game_id;
            sscanf(buff,"end_game=%d",&local_game_id);
            game[local_game_id]->isOnGoing =0;
            check_for_isInvolved(local_game_id);
            bzero(buff, MAX);
            continue;
        }

        else
        {
            continue;
        }

         
        n = 0; 
        printf("Sending msg to  %s\n",currentPointer->client_name );
        write(currentPointer->connfd, buff, sizeof(buff)); 
 	
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Self connection closed for %s...\n",currentPointer->client_name); 
            break;             
        }

        else if (strncmp("close", buff, 5) == 0) 
        { 
            printf("Server totally closing down\n");             
            exit(0);
        }
    }
    pthread_exit(NULL);
} 



int create_thread(int fcount)
{
    //printf("\n \ncreating thread %d \n",fcount);
    char append[2];
    a[fcount] = malloc(sizeof(struct client));
    strcpy(a[fcount]->client_name,"Client");
    sprintf(append,"%d",fcount);
    strcat(a[fcount]->client_name,append);
    pthread_create(&a[fcount]->tid,&a[fcount]->attr,tFunc,a[fcount]);
    pthread_exit(NULL);
    return ;

}

/*
Below is the main, which setups the server
and runs in listening mode

*/

void main()
{
    printf("Please enter this machine(Server) IP address\n");
    scanf("%s",&my_ip_addr);
    printf("Please enter this machine(Server) port number\n");
    scanf("%d",&MYPORT);
    //MYPORT = 8080;
    //strcpy(my_ip_addr,"127.0.0.1");
    //printf("counter value =%d\n",counter);
	//printf("in main function\n");
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
     
    a[counter] = malloc(sizeof(struct client));



    char append[2];
    strcpy(a[counter]->client_name,"Client");
    sprintf(append,"%d",counter);
    strcat(a[counter]->client_name,append);
    pthread_create(&a[counter]->tid,&a[counter]->attr,tFunc,a[counter]);
    sleep(20);
    pthread_exit(NULL);

}

