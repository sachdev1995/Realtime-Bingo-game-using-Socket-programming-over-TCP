// Write CPP code here 
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <pthread.h>
#include <unistd.h>
#include<time.h> 
#include<stdbool.h>
#define MAX 500 
#define SA struct sockaddr

int ServerPORT ;
int Server_ip_addr[20];

int serverSockfd, serverConnfd; 
struct sockaddr_in servaddr, cli;

int cardNumber =0; 
int MYPORT;
char my_ip_addr[20];
char my_name[10];
int current_game_id;
int isCurrentWinnerFound = 0;
int isSelfCaller = 0;
pthread_mutex_t lock;

int isPlayerRegistered = 0;

/*
Below is the structure to be used by all the clients
*/

struct ClientStruct 
{
    int port;
    char ip_addr[20];
    int ClientAcceptConserverSockfd;
    int ClientAcceptConserverConnfd;
    int len;
    struct sockaddr_in ClientAcceptConservaddr, ClientAcceptConcli;
    pthread_t tid;
    int ClientCreateConSockfd, ClientCreateConConnfd, ClientCreateConlen; 
    struct sockaddr_in ClientCreateConservaddr, ClientCreateConcli; 
    char client_name[20];
    int bingoCard[25];
    int game_id;
    int self_count ;
    int test_number;
}someVariable;

int createrCount = 0;
struct ClientStruct *createrThreadStruct[20];

int accepterCount =0;
struct ClientStruct *accepterThreadStruct[20];

/*
Below function creates the bingo card 
as per the requirement.
For 'free' data in the card, I have used
the value as 100
*/

void generateBingoCard  (int array[])
{

    printf("new card generated is \n");

    int tempBingoCard[5][5];
    
    int k = 0;



    int row, col, value, i;
    bool check[76];    
    for(i = 0; i < 76; ++i)  
        check[i] = false;
    
    bool test; 

    for(col = 0; col < 5; ++ col) 
    {
        for(row = 0; row < 5; ++ row)
        {
            if(row == 2 && col == 2)
            {
                tempBingoCard[row][col] = 100; 
                continue;   
            }
            test = true;
            do
            {
                value = rand()%15 + 1;
                value = value + (col*15);
                if(check[value] == false) 
                {
                    tempBingoCard[row][col] = value;
                    check[value] = true;
                    test = false;
                }

            }while(test == true);
        }
    }

    for(int i=0; i< 5;i++)
    {
        for (int j = 0;j<5; j++)
        {
            printf("%d ",tempBingoCard[i][j]);
            array[k++] = tempBingoCard[i][j];
        }
        printf("\n");
    }
    


}

/*
void func(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    for (;;) 
    { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
        write(sockfd, buff, sizeof(buff)); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client self Exit...\n"); 
            break; }
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        printf("From Server : %s", buff); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit since server left...\n"); 
            break; 
        }
        else if (strncmp("close", buff, 4) == 0) 
        { 
            printf("All clients closing down since server closed down"); 
            //break; 
            exit(0); 
        }
    } 
} 

*/  

/*
Below is Thread function used in P2P mode
The function parameter is the client structure
which is created by caller player
for individual other players
*/

void ClientAcceptConFunc(struct ClientStruct *currentPointer)

{ 
    currentPointer->ClientAcceptConserverSockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (currentPointer->ClientAcceptConserverSockfd == -1) 
    { 
        printf("socket creation failed for P2p client...\n"); 
        pthread_exit(NULL); 
    }

    else
        printf("Socket successfully created for P2p client..\n");

    bzero(&currentPointer->ClientAcceptConservaddr, sizeof(currentPointer->ClientAcceptConservaddr)); 
    currentPointer->ClientAcceptConservaddr.sin_family = AF_INET;
    currentPointer->ClientAcceptConservaddr.sin_addr.s_addr = inet_addr(currentPointer->ip_addr); 
    currentPointer->ClientAcceptConservaddr.sin_port = htons(currentPointer->port);
    //printf("port number to be connected is %d\n",currentPointer->port);
    //printf("ip addr to be connected is %s\n",currentPointer->ip_addr);
    //printf("client name to be connected is %s\n",currentPointer->client_name);

    if (connect(currentPointer->ClientAcceptConserverSockfd, (SA*)&currentPointer->ClientAcceptConservaddr, sizeof(currentPointer->ClientAcceptConservaddr)) != 0) 
    { 
        printf("connection with the server failed for P2p client...\n"); 
        pthread_exit(NULL); 
    } 
    else
        printf("connected to the server P2P..\n"); 
    
    char buff[MAX]; 
    sprintf(buff,"Initiate"); 
    int n; 

    /*
    Below the commands which are exchanged 
    between the caller and other players
    for eg. when a player asks for bingo card
    after the game is started, the caller sends 
    him a unique bingo card.
    Similarly other commands are also 
    exchaged

    */

    for (;;) 
    { 
        n = 0;    
        write(currentPointer->ClientAcceptConserverSockfd, buff, sizeof(buff)); 
        if ((strncmp(buff, "exit", 4)) == 0) 
        { 
            printf("Client self Exit P2P...\n"); 
            break; 
        }

        bzero(buff, sizeof(buff)); 
        read(currentPointer->ClientAcceptConserverSockfd, buff, sizeof(buff)); 
        
        if ((strncmp(buff, "get_card", 8)) == 0) 
        { 
            isCurrentWinnerFound = 0;
            bzero(buff, sizeof(buff));
            strcat(buff,"bingo_card ");
            strcat(buff,"data ");
            int arr[25];
            pthread_mutex_lock(&lock);
             generateBingoCard(arr);
             pthread_mutex_unlock(&lock);
            //printf("%s outside for loop\n",buff);
            sprintf(buff,"bingo_card %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ",arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6],arr[7],arr[8],arr[9],arr[10],arr[11],arr[12],arr[13],arr[14],arr[15],arr[16],arr[17],arr[18],arr[19],arr[20],arr[21],arr[22],arr[23],arr[24]);
            printf("Sending over the card...%s\n",buff);
            continue;
            
        }

        if ((strncmp(buff, "get_number", 10)) == 0) 
        { 
            
            sleep(2);
            if( isCurrentWinnerFound == 0)
            {
                bzero(buff, sizeof(buff));
                int upper = 100, lower = 1;
                int num = (rand() % (upper - lower + 1)) + lower;
                printf("Number requested,Sending number %d \n ",num);//currently here put num here
                sprintf(buff,"New_number_is %d",num);
                cardNumber++;
            }
            else
            {
                bzero(buff, sizeof(buff));
                sprintf(buff,"close");
                write(currentPointer->ClientAcceptConserverSockfd, buff, sizeof(buff));
                close(currentPointer->ClientAcceptConserverSockfd);

            }    
            continue;
        }
        /*
        when a winner is found in game,
        that corresponding player informs the caller.
        Caller now informs the server that
        the winner for the game is found and
        the game has ended.
        The server will then update its database accordingly

        */

        if ((strncmp(buff, "I_am_winner", 10)) == 0)
        {
            printf("winner found\n");
            bzero(buff, sizeof(buff));
            isCurrentWinnerFound = 1;
            isSelfCaller = 0;
            sprintf(buff,"close");
            write(currentPointer->ClientAcceptConserverSockfd, buff, sizeof(buff));
            close(currentPointer->ClientAcceptConserverSockfd);
            bzero(buff, sizeof(buff)); 
            sprintf(buff,"end_game=%d",current_game_id); 

            write(serverSockfd, buff, sizeof(buff)); 
            pthread_exit(NULL);
            return   ;
        }
    } 
    
    close(currentPointer->ClientAcceptConserverSockfd);

}

int ClientCreateConSockfd, ClientCreateConConnfd, ClientCreateConlen; 
struct sockaddr_in ClientCreateConservaddr, ClientCreateConcli;

/*
Below is the handler function 
for inidivial players (not the caller)

*/

void createrHandlerFunction(struct ClientStruct *currentPointer)
{
    //printf("Inside Handler function\n");
    currentPointer->ClientCreateConSockfd = ClientCreateConSockfd;
    currentPointer->ClientCreateConlen = sizeof(currentPointer->ClientCreateConcli);
    currentPointer->ClientCreateConConnfd = accept(currentPointer->ClientCreateConSockfd, (SA*)&ClientCreateConcli, &currentPointer->ClientCreateConlen); 
    
    if (currentPointer->ClientCreateConConnfd < 0) { 
        printf("server acccept failed for P2P client Server...\n"); 
        pthread_exit(NULL);
    } 
    else
        printf("server acccept the client for P2P client Server...\n"); 

    createrTransferHandlerFunct(currentPointer);
    close(currentPointer->ClientCreateConConnfd); ///to check if doesnt work put ClientCreateConSockfd

}

/*
the below function is the extension of the above function
the below function actually exchanges data between a caller and itself(player)
*/

void createrTransferHandlerFunct(struct ClientStruct *currentPointer)
{
    
    printf("Inside transfer handler function\n");
    char buff[MAX]; 
    int n;
    createrCount++;
    printf("creating a creater thread here %d\n",createrCount);
    char append[2];
    currentPointer->test_number = 36;
    createrThreadStruct[createrCount] = malloc(sizeof(struct ClientStruct));
    strcpy(createrThreadStruct[createrCount]->client_name,"Client");
    sprintf(append,"%d",createrCount);
    strcat(createrThreadStruct[createrCount]->client_name,append);

    pthread_create(&createrThreadStruct[createrCount]->tid,NULL,createrHandlerFunction,createrThreadStruct[createrCount]);

    for (;;) 
    { 
        bzero(buff, MAX);
        read(currentPointer->ClientCreateConConnfd, buff, sizeof(buff));
        //printf("From P2P client: %s\n", buff);

        if((strncmp(buff, "Initiate", 7)) == 0)
        {
            printf("Received %s command \n",buff);

            bzero(buff, MAX);
            sprintf(buff,"get_card");
            printf("Sending %s command back\n",buff);
        }

        else if ((strncmp(buff, "bingo_card", 7)) == 0) { 
            printf("adding values to the card...\n");
            sscanf(buff,"bingo_card %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ",&currentPointer->bingoCard[0],&currentPointer->bingoCard[1],&currentPointer->bingoCard[2],&currentPointer->bingoCard[3],&currentPointer->bingoCard[4],&currentPointer->bingoCard[5],&currentPointer->bingoCard[6],&currentPointer->bingoCard[7],&currentPointer->bingoCard[8],&currentPointer->bingoCard[9],&currentPointer->bingoCard[10],&currentPointer->bingoCard[11],&currentPointer->bingoCard[12],&currentPointer->bingoCard[13],&currentPointer->bingoCard[14],&currentPointer->bingoCard[15],&currentPointer->bingoCard[16],&currentPointer->bingoCard[17],&currentPointer->bingoCard[18],&currentPointer->bingoCard[19],&currentPointer->bingoCard[20],&currentPointer->bingoCard[21],&currentPointer->bingoCard[22],&currentPointer->bingoCard[23],&currentPointer->bingoCard[24]); 
            for (int i = 0 ;i < 25; i++)
            {
                if(i%5 == 0)
                    printf("\n");
                printf("%d ",currentPointer->bingoCard[i] );
            }
            printf("\n");
            bzero(buff, sizeof(buff));
            sprintf(buff,"get_number");
            
            
        }

        else if  (strncmp(buff,"New_number_is",10)== 0)  
        {
            currentPointer->self_count++;
            printf("Number received is %s\n",buff);
            int num;
            sscanf(buff,"New_number_is %d",&num);
            bzero(buff, sizeof(buff));
            if((num == currentPointer->test_number))
            {
                if(currentPointer->self_count >15)
                {
                sprintf(buff,"I_am_winner ");
                printf("I am the winner of the game\n");
                }
                else
                {
                    currentPointer->test_number++;
                    sprintf(buff,"get_number");

                }
            }
            else
            {
                    
                sprintf(buff,"get_number");
                    
            }      

            
        }
        else if (strncmp("close", buff, 4) == 0) 
        { 
            printf("All clients closing down since server closed down\n"); 
            bzero(buff,sizeof(buff));
            pthread_exit(NULL);
        }

        n = 0;

        write(currentPointer->ClientCreateConConnfd, buff, sizeof(buff));
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    }
  pthread_exit(NULL);

}

/*
the below is the handler function 
for a client to behave as server
i.e it will put the current instance
of client in listening mode so that
the other players can connect to it 
in case a game needs to start
*/

void ClientCreateConnectFunc (void)
{
    ClientCreateConSockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (ClientCreateConSockfd == -1) { 
        printf("socket creation failed for P2P client Server...\n"); 
        pthread_exit(NULL);
    } 
    else
        printf("Socket successfully created for P2P client Server..\n"); 
    bzero(&ClientCreateConservaddr, sizeof(ClientCreateConservaddr));
    ClientCreateConservaddr.sin_family = AF_INET; 
    ClientCreateConservaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    ClientCreateConservaddr.sin_port = htons(MYPORT);

    if ((bind(ClientCreateConSockfd, (SA*)&ClientCreateConservaddr, sizeof(ClientCreateConservaddr))) != 0) { 
        printf("socket bind failed for P2P client Server...\n"); 
        pthread_exit(NULL);
    } 
    else
        printf("Socket successfully binded for P2P client Server..\n"); 
  
    if ((listen(ClientCreateConSockfd, 5)) != 0) { 
        printf("Listen failed for P2P client Server...\n"); 
        pthread_exit(NULL);
    } 
    else
        printf("Server listening for P2P client Server..\n"); 

    createrThreadStruct[createrCount] = malloc(sizeof(struct ClientStruct));
    char append[2];
    sprintf(append,"%d",createrCount);
    pthread_create(&createrThreadStruct[createrCount]->tid,NULL,createrHandlerFunction,createrThreadStruct[createrCount]);
    pthread_exit(NULL);

}

/*
Below is the handler function so 
that the current instance of client
can interact with the server(manager)
It will perform the operations such as registeration.
Also, the function handles the commands which 
the user enter and accordingly contacts server 
for fetching data
*/

void ServerConnectionFunc (void)
{    
    printf("Please enter the Server Ip address for future connections\nPlease note the data wont be verified for error correction, hence enter the data correctly\n");
    scanf("%s",&Server_ip_addr);
    printf("Please enter the port number of the server\n");
    scanf("%d",&ServerPORT);
    printf("Please enter valid commands for the game for eg. register \n");
    int n; 
    char buff[MAX]; 
    char userinput[MAX];
    for (;;) 
    { 
        n = 0; 
        bzero(buff, sizeof(buff));
        while ((userinput[n++] = getchar()) != '\n') ;
        /*
        When the current instance of the client
        tries to register with server below command 
        will be executed
        */

        if ((strncmp(userinput, "register", 7)) == 0) 
        { 
            char localname[10];
            char localip_addr[20];
            int localport_number;
             sscanf(userinput,"register %s %s %d",&localname,&localip_addr,&localport_number);
             printf("entered data is name=%s, ip_addres=%s and port=%d\n",localname,localip_addr,localport_number);
             if (((strncmp(localname, my_name, 7)) == 0) || (((strncmp(localip_addr, my_ip_addr, 10)) == 0)) || localport_number == MYPORT)
             {
                printf("registeration failed, please use unique data or this client has already been registered\n");
                continue;
             }
             else
             {
                strcpy(my_name,localname);
                strcpy(my_ip_addr, localip_addr);
                MYPORT = localport_number;
                //ServerPORT = 8080;
                //strcpy(Server_ip_addr,"127.0.0.1");
                printf("Client registering with Server\n");

                serverSockfd = socket(AF_INET, SOCK_STREAM, 0); 
                if (serverSockfd == -1) { 
                    printf("socket creation failed for connecting to main Server...\n"); 
                    pthread_exit(NULL); 
                } 
                else
                    printf("Socket successfully created for connecting to main server..\n"); 

                bzero(&servaddr, sizeof(servaddr));
                servaddr.sin_family = AF_INET; 
                servaddr.sin_addr.s_addr = inet_addr(Server_ip_addr);
                servaddr.sin_port = htons(ServerPORT);

                if (connect(serverSockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
                    printf("connection with the MAIN server failed...\n"); 
                    pthread_exit(NULL); 
                } 
                else
                    printf("connected to the MAIN server..\n"); 
                }

                bzero(buff, sizeof(buff)); 
                sprintf(buff,"my_port=%d my_ipaddr=%s my_name=%s",MYPORT,my_ip_addr,my_name);

                write(serverSockfd, buff, sizeof(buff));
                sleep(1);
                bzero(buff, sizeof(buff)); 
                read(serverSockfd, buff, sizeof(buff));
                if ((strncmp(buff, "registered", 9)) == 0) 
                { 
                    printf("Client successfully registered\n");
                    isPlayerRegistered = 1;
                    pthread_t ClientCreateConTID;
                    pthread_create(&ClientCreateConTID,NULL, ClientCreateConnectFunc,NULL);  //currently here
                }
                else
                {
                    printf("Client connected to server but not successfully registered\n");
                    printf("this instance of client faced registeration isssue and will now close\n");
                    exit(1);
                }

                continue;
            
        }
        /*
        Below statements will be executed with
        the client query for curret player list
        the server in return will reply 
        with the list of players
        */

        else if ((strncmp(userinput, "query players", 12)) == 0) 
        {
                if(isPlayerRegistered ==0)
                {
                    printf("You have not registered yet, please register!\n");
                    continue;
                }
                bzero(buff, sizeof(buff)); 
                bzero(userinput,sizeof(userinput));
                strcpy(buff,"query_players");

                write(serverSockfd, buff, sizeof(buff));
                sleep(1);
                bzero(buff, sizeof(buff)); 
                read(serverSockfd, buff, sizeof(buff));
                if ((strncmp(buff, "players=", 6)) == 0)
                {
                    int local_player_count;
                    sscanf(buff,"players=%d",&local_player_count);

                    printf("%d\n",local_player_count);
                    char portStr[] = "port=";
                    char *temp;
                    temp = malloc(sizeof(buff));
                    temp = strstr(buff,portStr);
                    for(int i =0;i<local_player_count;i++)
                    {
                        int num;
                        char localip_addr[20];
                        char localname[10];
                        sscanf(temp,"port=%d ip_addr=%s name=%s ",&num,&localip_addr,&localname);
                        printf("port number is %d , ipaddress is %s and name is %s\n",num,localip_addr,localname);
                        bzero(localname,sizeof(localname));
                        bzero(localip_addr,sizeof(localip_addr));
                        
                        temp++;
                        temp = strstr(temp,portStr);

                    }

                }
                else
                    printf("querying players did not recive suitable data\n"); 
                continue;
        }

        /*
        Simimlary when the cleint queries for current
        on going games, the server replies with that information
        */

        else if ((strncmp(userinput, "query games", 11)) == 0)
        {
                if(isPlayerRegistered ==0)
                {
                    printf("You have not registered yet, please register!\n");
                    continue;
                }
                bzero(buff, sizeof(buff)); 
                bzero(userinput,sizeof(userinput));
                strcpy(buff,"query_games");

                write(serverSockfd, buff, sizeof(buff));
                sleep(3);
                bzero(buff, sizeof(buff)); 
                read(serverSockfd, buff, sizeof(buff));
                if ((strncmp(buff, "game_count", 9)) == 0)
                {

                    //printf("%s\n",buff); //curently here
                    int local_game_count;
                    sscanf(buff,"game_count=%d ",&local_game_count);
                    

                    if(local_game_count == 0)
                    {
                        printf("Either currupt data recieved or number of games is 0\n");
                        continue;

                    }

                    char *new_String;
                    new_String = malloc(sizeof(buff));
                    new_String = strstr(buff,"game_id=");
                    //printf("%s\n",new_String);

                    printf("********query games log*******\n");

                    printf("game count = %d\n",local_game_count);
                    for(int j= 0; j<local_game_count;j++)
                    {
                        //printf("in next\n");
                        int local_game_id;
                        char local_Caller_name[10];
                        int number_players;


                        
                        sscanf(new_String,"game_id=%d caller_name=%s player_list:  player=%d",&local_game_id,&local_Caller_name,&number_players);
                        printf("game_id=%d caller_name=%s number of player=%d \n",local_game_id,local_Caller_name,number_players);
                        char portStr[] = "port=";
                        char *temp;
                        temp = strstr(new_String,portStr);
                        for(int i =0;i<number_players;i++)
                        {
                            int num;
                            char localip_addr[20];
                            char player_name[10];
                            //printf("%s\n",temp);
                            sscanf(temp,"port=%d ip_addr=%s name=%s",&num,&localip_addr,&player_name);
                            printf("port number is %d ,ipaddress is %s and name is %s \n",num,localip_addr,player_name);
                            temp++;
                            temp = strstr(temp,portStr);

                        }
                        //printf("going for next with string %s\n",temp);
                        if((j+1)<local_game_count)
                        {
                            //printf("in if condition\n");
                            new_String++;
                            new_String = strstr(new_String,"game_id=");
                            //printf("new string '%s'\n",new_String);
                        }
                    }
                }
                else
                    printf("Either currupt data recieved or number of games is 0\n");
                continue;
        }

        /*
        below statements will be executed when a client tries to 
        deregister, the current instance of the client will leave
        Note, a client can only deregister itself and
        no one else
        */


        else if ((strncmp(userinput, "deregister", 9)) == 0)
        {
                if(isPlayerRegistered ==0)
                {
                    printf("You have not registered yet, please register!\n");
                    continue;
                }
                if(isSelfCaller == 1)
                {
                    printf("this client instance cannot leave the game\n");
                    continue;
                }
                bzero(userinput,sizeof(userinput));
                char localname[10];
                sscanf(userinput,"deregister %s",&localname);
                
                bzero(buff, sizeof(buff)); 
                sprintf(buff,"deregister %s",localname);

                write(serverSockfd, buff, sizeof(buff));
                sleep(1);
                bzero(buff, sizeof(buff)); 
                read(serverSockfd, buff, sizeof(buff));
                if ((strncmp(buff, "sucess", 5)) == 0)
                {
                    printf("this client instance can successfully leave now, hence exiting\n");
                    exit(0);
                } 
                else
                {
                    printf("this client instance cannot leave, hence continuing\n");
                    continue;
                }

        }

        /*
        Below statements will be executed when a client
        tries to initiate a game. It ask the server for current player list
        if the there are sufficient players in the system, 
        then the server will send a list of available players
        This instance of client will now create thread for
        individual players . Those threads will then play 
        the game with the other players on the other end
        */

        else if ((strncmp(userinput, "start game", 9)) == 0)
        {
            if(isPlayerRegistered ==0)
                {
                    printf("You have not registered yet, please register!\n");
                    continue;
                }
            
            int local_player_count;
            sscanf(userinput,"start game %d",&local_player_count);
            bzero(userinput,sizeof(userinput));
            bzero(buff, sizeof(buff));
            sprintf(buff,"start_game count=%d\n",local_player_count);
            write(serverSockfd, buff, sizeof(buff));
            sleep(3);
            bzero(buff, sizeof(buff));
            read(serverSockfd, buff, sizeof(buff));
            if (strncmp("connect", buff, 6) == 0)
            { 
                    //printf("sending connect command\n");
                    isSelfCaller = 1;
                    int number_players;
                    sscanf(buff,"connect game_id=%d player=%d",&current_game_id,&number_players);
                    //printf("%d\n",number_players);
                    char portStr[] = "port=";
                    char *temp;
                    temp = strstr(buff,portStr);
                    for(int i =0;i<number_players;i++)
                    {
                        int num;
                        char localip_addr[6];
                        //printf("%s\n",temp);
                        sscanf(temp,"port=%d ip_addr=%s",&num,&localip_addr);
                        printf("port number to be connected is %d and ipaddress is %s \n",num,localip_addr);
                        accepterThreadStruct[accepterCount] = malloc(sizeof(struct ClientStruct));
                        accepterThreadStruct[accepterCount]->port = num;
                        accepterThreadStruct[accepterCount]->game_id = current_game_id;
                        sprintf(accepterThreadStruct[accepterCount]->ip_addr,"%s",localip_addr);
                        pthread_create(&accepterThreadStruct[accepterCount]->tid,NULL,ClientAcceptConFunc,accepterThreadStruct[accepterCount]);
                        //printf("created accepter thread %d\n",accepterCount);
                        accepterCount++;
                        temp++;
                        temp = strstr(temp,portStr);

                    }
                    continue;
                    
                     
            }
            else
            {
                printf("insufficient players\n");
                continue;
            }


        }
        else
        {
            printf("invalid command entered, please enter a valid command\n");
            continue;
        }
    } 
    pthread_exit(NULL); 
    close(serverSockfd);
}



int main(int argc, char *argv[] ) 
{ 

    
    pthread_t ServerConnectionTID;
    pthread_attr_t ServerCattr;
    pthread_create(&ServerConnectionTID,NULL,ServerConnectionFunc,NULL);
    pthread_exit(NULL);

} 
