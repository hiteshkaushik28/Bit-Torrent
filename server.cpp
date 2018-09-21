#include <stdio.h>  
#include <string.h>   
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h>  

#define TRUE   1  
#define FALSE  0  
#define PORT 8880
     
int main(int argc , char *argv[])   
{   
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[50] ,  
          max_clients = 5 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;     
  
    fd_set readfds;   
      
    char *message = "ECHO Daemon v1.0 \r\n";   
     
   for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
   if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
   if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listening on port %d \n", PORT);   
         
    if (listen(master_socket, 5) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    addrlen = sizeof(address);   
    puts("Waiting for clients ...");   
         
    while(TRUE)   
    {   
        FD_ZERO(&readfds);   
     
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        for ( i = 0 ; i < max_clients ; i++)   
        {   
             sd = client_socket[i];   
                 
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                     
            if(sd > max_sd)   
                max_sd = sd;   
        }   
      
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
       // printf("\nactivity no : %d\n",activity);
        if ((activity < 0) && (errno!=EINTR))   
        {   
            perror("select error: ");
            continue;  
        }   
             
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
           printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
           
#if 0
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )   
            {   
                perror("send");   
            }   
                 
            puts("Welcome message sent successfully");   
#endif             
            for (i = 0; i < max_clients; i++)   
            {   
            if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        }   
        else
        {
            for (i = 0; i < max_clients; i++)   
            {   
                sd = client_socket[i];   
                     
                if (FD_ISSET( sd , &readfds))   
                {     
                    char buffer[1025]; 
                    if ((valread = read( sd , buffer, 1024)) == 0)   
                    {   
                        getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);   
                        printf("Host disconnected , ip %s , port %d \n" ,  
                              inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                             
                        close( sd );  
                        client_socket[i] = 0;   
                    }   
                    else 
                    {   
                       buffer[valread] = '\0';   
                       printf("%s\n",buffer);
                       fflush(stdout);
                    }   
                }   
            }
        }   
    }   
         
    return 0;   
}   
