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
#include <sys/stat.h>
#include <bits/stdc++.h>
#include <fstream>
using namespace std;

#define TRUE   1  
#define FALSE  0  
#define PORT 8880
ofstream seed;
map<string,set<string > > seeder_list;
vector<string>tokens;
    map<string,set<string > > ::iterator itr;
set<string>::iterator itr1;
int max_sd;   

string process_get(string input)
{
    string result="";
    int pos = input.find(':');
    string hash = input.substr(pos+1);
    itr = seeder_list.find(hash);
    for(itr1=itr->second.begin();itr1!=itr->second.end();itr1++)
        result += *itr1 + ":";

    result.pop_back();
    return result;
}

/* Function to extract IP and PORT from client meta */
string get_ip_port(string input)
{
    string temp;
    int first,last;
    first = input.find_first_of(":");
    last = input.find_last_of(":");
    temp = input.substr(first+1,last-first-1);
    return temp;
}

/* Function to extract HASH from client meta */
string get_hash(string input)
{
    int pos = input.find_last_of(":");
    string hash = input.substr(pos+1);
    return hash;
}


/* Update seeder list from backup file */
void get_seeder_list(string file)
{
    struct stat buffer;
    if(stat(file.c_str(),&buffer) == 0)
    {
        seed.open(file.c_str());
        if(!seed)
        {
            cout<<"Error openin seed file\n";
        }
        else
        {

        }
    }
}

  
int main(int argc , char *argv[])   
{   
    /* Update seeder list at init */
    string seed_file = argv[3];
    get_seeder_list(seed_file);

    char *tok;
    
    /* Listening to multiple clients using select() */
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[50] ,  
          max_clients = 5 , activity, i , valread , sd;   
    
    struct sockaddr_in address;     
  
    fd_set readfds;   
      
    
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
            
            for (i = 0; i < max_clients; i++)   
            {   
            if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                                            
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
                       string input(buffer);
                                           
                       if(input[0] == 's')
                       {
                            string hash = get_hash(input);
                            string ip = get_ip_port(input);
                            seeder_list[hash].insert(ip);
                        }

                        else 
                        {
                            cout<<"get here";
                          string result =  process_get(input);
                          send(sd , result.c_str() , strlen(result.c_str()) , 0 ); 
                          cout<<"\nGET processed successfully successfully\n";   
                        }
                       
                    }   
                }   
            }
        }   
    }   

    /* Listen code ends here */
         
    return 0;   
}   
