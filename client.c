// Write CPP code here 
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <pthread.h>
#include <unistd.h>
#include<time.h> 
#define MAX 200 
#define ServerPORT 8080 
#define MYPORT 8083
#define SA struct sockaddr
int cardNumber =0; 

struct ClientStruct 
{
    int port;
    int ClientAcceptConserverSockfd;
    int ClientAcceptConserverConnfd;

    int len;
    struct sockaddr_in ClientAcceptConservaddr, ClientAcceptConcli;
    pthread_t tid;


    int ClientCreateConSockfd, ClientCreateConConnfd, ClientCreateConlen; 
    struct sockaddr_in ClientCreateConservaddr, ClientCreateConcli; 
    

    char client_name[20];
    int bingoCard[25];
    


}someVariable;

int createrCount = 0;
struct ClientStruct *createrThreadStruct[10];

int accepterCount =0;

struct ClientStruct *accepterThreadStruct[0];



//int bingoCard[25];

void generateBingoCard(int arr[])
{
    //printf("inside generate function\n");
    int upper  = 50, lower = 1;
    srand(1);
    for(int i = 0;i< 25;i++)
    {
        int num = (rand() % (upper - lower + 1)) + lower;

        arr[i] = num;
        printf("current random number is %d \n",arr[i]);
    }
}



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
  

void ClientAcceptConFunc(struct ClientStruct *currentPointer)

{

    //int ClientAcceptConserverSockfd, ClientAcceptConserverConnfd; 
    //struct sockaddr_in ClientAcceptConservaddr, ClientAcceptConcli; 
  
    // socket create and varification 
    currentPointer->ClientAcceptConserverSockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (currentPointer->ClientAcceptConserverSockfd == -1) { 
        printf("socket creation failed for P2p client...\n"); 
        pthread_exit(NULL); 
    } 
    else
        printf("Socket successfully created for P2p client..\n"); 
    bzero(&currentPointer->ClientAcceptConservaddr, sizeof(currentPointer->ClientAcceptConservaddr)); 
  
    // assign IP, PORT 
    currentPointer->ClientAcceptConservaddr.sin_family = AF_INET; 
    currentPointer->ClientAcceptConservaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    currentPointer->ClientAcceptConservaddr.sin_port = htons(currentPointer->port);
    printf("port number to be connected is %d\n",currentPointer->port);
  
    // connect the client socket to server socket 
    if (connect(currentPointer->ClientAcceptConserverSockfd, (SA*)&currentPointer->ClientAcceptConservaddr, sizeof(currentPointer->ClientAcceptConservaddr)) != 0) { 
        printf("connection with the server failed for P2p client...\n"); 
        pthread_exit(NULL); 
    } 
    else
        printf("connected to the server P2P..\n"); 
  
    // function for chat 
    //func(serverSockfd); 

    char buff[MAX]; 
    sprintf(buff,"Initiate"); 
    int n; 
    for (;;) 
    { 
        //bzero(buff, sizeof(buff)); 
        printf("sending %s command to client \n ",buff); 
        n = 0; 
        //while ((buff[n++] = getchar()) != '\n') ;
             
        write(currentPointer->ClientAcceptConserverSockfd, buff, sizeof(buff)); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client self Exit P2P...\n"); 
            break; }
        bzero(buff, sizeof(buff)); 
        read(currentPointer->ClientAcceptConserverSockfd, buff, sizeof(buff)); 
        printf("Command recieved is  : %s\n", buff); 
        if ((strncmp(buff, "get_card", 8)) == 0) { 
            bzero(buff, sizeof(buff));
            strcat(buff,"bingo_card ");
            strcat(buff,"data ");
            int arr[25];
             generateBingoCard(arr);
            printf("%s outside for loop\n",buff);
            sprintf(buff,"bingo_card %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ",arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6],arr[7],arr[8],arr[9],arr[10],arr[11],arr[12],arr[13],arr[14],arr[15],arr[16],arr[17],arr[18],arr[19],arr[20],arr[21],arr[22],arr[23],arr[24]);
            /*
            for(int i =0 ;i < 25 ;i++)
            {
                //printf("%d ",arr[i]);
                char temp[3];
                sprintf(temp,"%d ",arr[i]);
                strcat(buff,temp);
                printf("%s inside for loop\n");
            }*/
            printf("Sending over the card...%s\n",buff);
            continue;
            //write(ClientAcceptConserverSockfd, buff, sizeof(buff));
            //bzero(buff, sizeof(buff));
             
            
        }/*
        if ((strncmp(buff, "bingo_card", 7)) == 0) { 
            printf("adding values to the card...\n");
            sscanf(buff,"bingo_card %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ",&bingoCard[0],&bingoCard[1],&bingoCard[2],&bingoCard[3],&bingoCard[4],&bingoCard[5],&bingoCard[6],&bingoCard[7],&bingoCard[8],&bingoCard[9],&bingoCard[10],&bingoCard[11],&bingoCard[12],&bingoCard[13],&bingoCard[14],&bingoCard[15],&bingoCard[16],&bingoCard[17],&bingoCard[18],&bingoCard[19],&bingoCard[20],&bingoCard[21],&bingoCard[22],&bingoCard[23],&bingoCard[24]); 
            for (int i = 0 ;i < 25; i++)
            {
                printf("%d\n",bingoCard[i] );
            }
            bzero(buff, sizeof(buff));
            sprintf(buff,"get_number");
            continue;
            
        }*/
        if ((strncmp(buff, "get_number", 10)) == 0) { 
            
            bzero(buff, sizeof(buff));
            int upper = 100, lower = 1;
            int num = (rand() % (upper - lower + 1)) + lower;
            printf("Number requested,Sending number %d \n ",num);
            sprintf(buff,"New_number_is %d",num);
            cardNumber++;
            //if(cardNumber>24) exit(0);
                
            continue;
        }
        /*
        if  (strncmp(buff,"New_number_is",10)== 0) 
        {
            printf("Number received is %s\n",buff);
            //TO DO to check here if i am winner create a function for this and return true or false
            bzero(buff, sizeof(buff));
            sprintf(buff,"I_am_winner");
            continue;


        }*/
        if ((strncmp(buff, "I_am_winner", 10)) == 0)
        {
            printf("winner found for the game hence closing thread");
            bzero(buff, sizeof(buff));
            sprintf(buff,"close");
            break   ;
        }


        else if (strncmp("close", buff, 4) == 0) 
        { 
            printf("All clients closing down since server closed down"); 
            //break; 
            exit(0); 
        }
    } 
    
  
    // close the socket 
    close(currentPointer->ClientAcceptConserverSockfd);

}

int ClientCreateConSockfd, ClientCreateConConnfd, ClientCreateConlen; 
struct sockaddr_in ClientCreateConservaddr, ClientCreateConcli;



void createrHandlerFunction(struct ClientStruct *currentPointer)
{
    printf("Inside Handler function\n");
    currentPointer->ClientCreateConSockfd = ClientCreateConSockfd;

    currentPointer->ClientCreateConlen = sizeof(currentPointer->ClientCreateConcli); 
  
    // Accept the data packet from client and verification 
    currentPointer->ClientCreateConConnfd = accept(currentPointer->ClientCreateConSockfd, (SA*)&ClientCreateConcli, &currentPointer->ClientCreateConlen); 
    if (currentPointer->ClientCreateConConnfd < 0) { 
        printf("server acccept failed for P2P client Server...\n"); 
        pthread_exit(NULL);
    } 
    else
        printf("server acccept the client for P2P client Server...\n"); 



    createrTransferHandlerFunct(currentPointer);

    // Function for chatting between client and server 
    //func(ClientCreateConConnfd); 

    
    // After chatting close the socket 
    close(currentPointer->ClientCreateConConnfd); ///to check if doesnt work put ClientCreateConSockfd

}

void createrTransferHandlerFunct(struct ClientStruct *currentPointer)
{
    printf("Inside transfer handler function\n");
    char buff[MAX]; 
    int n; 

    createrCount++;

    printf("creating a creater thread here %d\n",createrCount);

    char append[2];

     createrThreadStruct[createrCount] = malloc(sizeof(struct ClientStruct));



        
    //a[counter]->port = 8080;
    strcpy(createrThreadStruct[createrCount]->client_name,"Client");
    sprintf(append,"%d",createrCount);
    strcat(createrThreadStruct[createrCount]->client_name,append);

    
    pthread_create(&createrThreadStruct[createrCount]->tid,NULL,createrHandlerFunction,createrThreadStruct[createrCount]);

    // infinite loop for chat 
    for (;;) 
    { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(currentPointer->ClientCreateConConnfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("From P2P client: %s\n", buff);
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
                printf("%d\n",currentPointer->bingoCard[i] );
            }
            bzero(buff, sizeof(buff));
            sprintf(buff,"get_number");
            
            
        } 
        else if ((strncmp(buff, "get_number", 10)) == 0) { 
            
            bzero(buff, sizeof(buff));
            int upper = 100 ,lower = 1;
            int num = (rand() % (upper - lower + 1)) + lower;
            printf("Number requested,Sending number %d \t ",num);

            sprintf(buff,"New_number_is %d",num);
            //cardNumber++;
            //if(cardNumber>24) exit(0);
            
            
        }
        else if  (strncmp(buff,"New_number_is",10)== 0)  
        {
            printf("Number received is %s\n",buff);
            //TO DO to check here if i am winner create a function for this and return true or false
            int num;
            sscanf(buff,"New_number_is %d",&num);
            bzero(buff, sizeof(buff));
            if(num == 36)
            {
            
            sprintf(buff,"I_am_winner ");
            }
            else
                sprintf(buff,"get_number");


            


        }
        else if (strncmp("close", buff, 4) == 0) 
        { 
            printf("All clients closing down since server closed down\n"); 
            //break; 
            pthread_exit(NULL);
        }
       // bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        //char deleteBuff[MAX];
        //while ((buff[n++] = getchar()) != '\n') ;
             
  
        // and send that buffer to client 
        write(currentPointer->ClientCreateConConnfd, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    }
  pthread_exit(NULL);

}
/*

void ClientCreateConnectFunc (void)
{
    int ClientCreateConSockfd, ClientCreateConConnfd, ClientCreateConlen; 
    struct sockaddr_in ClientCreateConservaddr, ClientCreateConcli; 
  
    // socket create and verification 
    ClientCreateConSockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (ClientCreateConSockfd == -1) { 
        printf("socket creation failed for P2P client Server...\n"); 
        pthread_exit(NULL);
    } 
    else
        printf("Socket successfully created for P2P client Server..\n"); 
    bzero(&ClientCreateConservaddr, sizeof(ClientCreateConservaddr)); 
  
    // assign IP, PORT 
    ClientCreateConservaddr.sin_family = AF_INET; 
    ClientCreateConservaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    ClientCreateConservaddr.sin_port = htons(8083); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(ClientCreateConSockfd, (SA*)&ClientCreateConservaddr, sizeof(ClientCreateConservaddr))) != 0) { 
        printf("socket bind failed for P2P client Server...\n"); 
        pthread_exit(NULL);
    } 
    else
        printf("Socket successfully binded for P2P client Server..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(ClientCreateConSockfd, 5)) != 0) { 
        printf("Listen failed for P2P client Server...\n"); 
        pthread_exit(NULL);
    } 
    else
        printf("Server listening for P2P client Server..\n"); 
    ClientCreateConlen = sizeof(ClientCreateConcli); 
  
    // Accept the data packet from client and verification 
    ClientCreateConConnfd = accept(ClientCreateConSockfd, (SA*)&ClientCreateConcli, &ClientCreateConlen); 
    if (ClientCreateConConnfd < 0) { 
        printf("server acccept failed for P2P client Server...\n"); 
        pthread_exit(NULL);
    } 
    else
        printf("server acccept the client for P2P client Server...\n"); 
  
    // Function for chatting between client and server 
    //func(ClientCreateConConnfd); 

    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(ClientCreateConConnfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("From P2P client: %s\t To client : ", buff);

        if ((strncmp(buff, "bingo_card", 7)) == 0) { 
            printf("adding values to the card...\n");
            sscanf(buff,"bingo_card %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ",&bingoCard[0],&bingoCard[1],&bingoCard[2],&bingoCard[3],&bingoCard[4],&bingoCard[5],&bingoCard[6],&bingoCard[7],&bingoCard[8],&bingoCard[9],&bingoCard[10],&bingoCard[11],&bingoCard[12],&bingoCard[13],&bingoCard[14],&bingoCard[15],&bingoCard[16],&bingoCard[17],&bingoCard[18],&bingoCard[19],&bingoCard[20],&bingoCard[21],&bingoCard[22],&bingoCard[23],&bingoCard[24]); 
            for (int i = 0 ;i < 25; i++)
            {
                printf("%d\n",bingoCard[i] );
            }
            bzero(buff, sizeof(buff));
            sprintf(buff,"get_number");
            
            
        } 

        else if ((strncmp(buff, "get_number", 10)) == 0) { 
            
            bzero(buff, sizeof(buff));
            int upper = 100 ,lower = 1;
            int num = (rand() % (upper - lower + 1)) + lower;
            printf("Number requested,Sending number %d \n ",num);

            sprintf(buff,"New_number_is %d",num);
            //cardNumber++;
            //if(cardNumber>24) exit(0);
            
            
        }
        else if  (strncmp(buff,"New_number_is",10)== 0)  
        {
            printf("Number received is %s\n",buff);
            //TO DO to check here if i am winner create a function for this and return true or false
            bzero(buff, sizeof(buff));
            sprintf(buff,"I_am_winner ");
            


        }
        else if (strncmp("close", buff, 4) == 0) 
        { 
            printf("All clients closing down since server closed down\n"); 
            //break; 
            exit(0); 
        }
       // bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        //char deleteBuff[MAX];
        while ((buff[n++] = getchar()) != '\n') ;
             
  
        // and send that buffer to client 
        write(ClientCreateConConnfd, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    }
  
    // After chatting close the socket 
    close(ClientCreateConSockfd); 
}


*/

void *deleteFunc(void)
{
    printf("deleteFunction is caller \n");
}

void ServerConnectionFunc (void)
{

    int serverSockfd, serverConnfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    serverSockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (serverSockfd == -1) { 
        printf("socket creation failed for connecting to main Server...\n"); 
        pthread_exit(NULL); 
    } 
    else
        printf("Socket successfully created for connecting to main server..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(ServerPORT); 
  
    // connect the client socket to server socket 
    if (connect(serverSockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the MAIN server failed...\n"); 
        pthread_exit(NULL); 
    } 
    else
        printf("connected to the MAIN server..\n"); 
  
    // function for chat 
    //func(serverSockfd); 

    char buff[MAX]; 
    bzero(buff, sizeof(buff)); 
    sprintf(buff,"my_port=%d",MYPORT);
    write(serverSockfd, buff, sizeof(buff)); 
    sleep(2);
    bzero(buff, sizeof(buff));
    read(serverSockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    sprintf(buff,"start_game count=2\n");
    write(serverSockfd, buff, sizeof(buff)); 
    int n; 
    for (;;) 
    { 
        
        //sleep(3);
        //printf("Enter the for MAIN string : "); 
        n = 0; 
        //while ((buff[n++] = getchar()) != '\n') ;
            
        //write(serverSockfd, buff, sizeof(buff)); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("MAIN Client self Exit...\n"); 
            break; }

        bzero(buff, sizeof(buff)); 
        read(serverSockfd, buff, sizeof(buff)); 
        
        //printf("From MAIN Server : %s", buff);   // TO DO this will go on in infinite loop

        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("MAIN Client Exit since server left...\n"); 
            break; 
        }

        else if (strncmp("connect", buff, 6) == 0) // TO DO receive port number from server here for client as a client which initiates a new game
        { 
            printf("seding connect command\n");

            int number_players;
    

            sscanf(buff,"connect player=%d",&number_players);
            printf("%d\n",number_players);
            //int portArray[number_players-1];
            char portStr[] = "port=";
            char *temp;
            temp = strstr(buff,portStr);
            for(int i =0;i<number_players;i++)
            {
                int num;
                printf("%s\n",temp);
                sscanf(temp,"port=%d",&num);
                printf("port number to be connected is %d \n",num);
                accepterThreadStruct[accepterCount] = malloc(sizeof(struct ClientStruct));
                accepterThreadStruct[accepterCount]->port = num;
                pthread_create(&accepterThreadStruct[accepterCount]->tid,NULL,ClientAcceptConFunc,accepterThreadStruct[accepterCount]);
                accepterCount++;
                temp++;
                temp = strstr(temp,portStr);

            }
            /*
            //int connectionCount = 1;
            int localCount = 0;
            while(localCount < number_players)
            {
                accepterThreadStruct[accepterCount] = malloc(sizeof(struct ClientStruct));
                pthread_create(&accepterThreadStruct[accepterCount]->tid,NULL,ClientAcceptConFunc,NULL);
                localCount++;
                accepterCount++;
            }*/

            //pthread_t CLientAcceptConTID;

            //pthread_create(&CLientAcceptConTID,NULL,ClientAcceptConFunc,NULL); 
            //break; 
             
        }
        /*
        else if (strncmp("create", buff, 4) == 0) 
        { 
            printf("seding connect command\n");
            pthread_t ClientCreateConTID;

            pthread_create(&ClientCreateConTID,NULL,ClientCreateConnectFunc,NULL); 
            //break; 
         }  
         */  
        
    } 
    pthread_exit(NULL);
    // close the socket 
    close(serverSockfd);
}



int main() 
{ 
    pthread_t ServerConnectionTID;
    pthread_attr_t ServerCattr;
    //pthread_t ClientCreateConTID;
    //pthread_create(&ClientCreateConTID,NULL, ClientCreateConnectFunc,NULL);

    pthread_create(&ServerConnectionTID,NULL,ServerConnectionFunc,NULL);

    

    //pthread_t CLientAcceptConTID;

    //pthread_create(&CLientAcceptConTID,NULL,ClientAcceptConFunc,NULL);

    pthread_exit(NULL);

/*
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(ServerPORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    // function for chat 
    func(sockfd); 
  
    // close the socket 
    close(sockfd); 
    */
} 
