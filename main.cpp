
#include <stdlib.h>
#include <string.h>
#include <string>
#include <openssl/sha.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <iomanip>
#include <vector>
#include <sys/ioctl.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string>
#include <thread>
#include <bits/stdc++.h>
using namespace std;

#define TRUE   1  
#define FALSE  0
#define PORT 8080 
#define __STDC_WANT_LIB_EXT1__ 1

#define CHUNKSIZE (512*1024)

char *tok;
vector<string> tokens;
vector<string> tracker_tokens;

char inp[100];
char buffer[1024] = {0}; 

fstream logfile,torrent,gettorrent,getfile;
bool flag = false;
struct winsize w;

map<string,string>id_peer;



string get_time()
{
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Must be static, otherwise won't work
    static char _retval[20];
    strftime(_retval, sizeof(_retval),"[%H:%M:%S]", timeinfo);
    
    return _retval;   
}

void client_listener(string ip_port)
{
    //string ip = ip_port.substr(0,ip_port.find(':'));
    int port = stoi(ip_port.substr(ip_port.find(':')+1));
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[30] ,max_clients = 30 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;   
         
    char buffer[1025];
         
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
    address.sin_port = htons( port );   
         
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }

    logfile<<get_time()<<" Client Listening on port:";
    logfile<<port;
    logfile<<"\n";   
         
    if (listen(master_socket, 3) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    addrlen = sizeof(address);   
    logfile<<get_time()<<" Client Waiting for connections ...\n";   
         
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
            logfile<<get_time()<<" select error:\n";   
        }   
             
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            logfile<<get_time()<<" New connection , socket fd is:"<<new_socket<<", ip is:"<<inet_ntoa(address.sin_addr)<<", port:"<<ntohs(address.sin_port)<<"\n";   
                   
            for (i = 0; i < max_clients; i++)   
            {   
            if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    logfile<<get_time()<<" Adding to list of sockets as: "<<i<<"\n";   
                    break;   
                }   
            }   
        }   
             
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {

            char getinput[1025]; 
            if ((valread = read( sd , getinput, 1024)) == 0)   
            {   
                getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);   
                printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                close( sd );   
                client_socket[i] = 0;   
                }   
                     
                else 
                {   
                    getinput[valread] = '\0';   
                    string input(getinput);

                    if(input[0] == 'i')
                    {
                        //process_id_request(input);
                    }
                }   
            }   
        }   
    }   
}

void blank()
{
    printf("\033c" );
}

void command_prompt()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    printf("\033[1;1H");
    printf("%c[2K", 27);
    printf("\033[30;");
    printf("47m");
    printf("kaushik@hitesh:~$ ");
    printf("\033[0m");
    printf(" ");
    fflush(stdin);   
}

void create_mtorrent(string argument)
{   
    torrent<<argument<<"\n";
}

string gethash()
{
    gettorrent.open(tokens[1].c_str(),ios::in);
    string get;
    if(!gettorrent)
    {
        logfile<<get_time()<<" Error opening mtorrent file for downloading\n";
    }

    else
    {
        logfile<<get_time()<<" opening mtorrent file for downloading\n";
        for(int i = 0;i < 6;i++)
        {
            getline(gettorrent,get);
        }
        return get;
    }
}

int create_connection(string ip,int port)
{
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port); 
       
    if(inet_pton(AF_INET,ip.c_str(), &serv_addr.sin_addr)<=0)  
    { 
        logfile<<get_time()<<"Invalid IP address\n"; 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        logfile<<get_time()<<" Connection attempt to tracker Failed \n"; 
        return -1; 
    } 
    
    return sock;

}

/* Function to get seeders of the file */
int file_get(string torrent)
{
    string ip;
    int port;
    gettorrent.open(torrent.c_str(),ios::in);
    string get;
    if(!gettorrent)
    {
        logfile<<get_time()<<" Error opening mtorrent file for downloading\n";
        return -1;
    }

    else
    {
        logfile<<get_time()<<" Opening mtorrent file for downloading\n";
        for(int i = 0;i < 2;i++) // skipping client ip
        {
            getline(gettorrent,get);
        }
        int pos = get.find(':');
        ip = get.substr(0,pos); 
        port = stoi(get.substr(pos+1));
        gettorrent.close();
        return create_connection(ip,port);

    }

}

string file_read(const char *filename)
{
    char hashchar[3] = "\0";
    unsigned char hash[20];
    unsigned char chunk[CHUNKSIZE];
    fstream input;
    size_t current_read;
    string temp;

    input.open(filename,ios::binary | ios::in | ios::ate);
    

    if(!input)
    {
        logfile<<get_time()<<" error opening input file\n";
        logfile<<"\n"; 
    }

    else
    {
        double size = input.tellg();
        string str = to_string(size);
        create_mtorrent(str);
        long int num = ceil(size/CHUNKSIZE);
        input.seekg(0,ios::beg);
        
        while(num--)
        {
            input.read((char *)chunk, CHUNKSIZE);
            current_read = input.gcount();
            SHA1(chunk,current_read,hash);
            for(int i = 0;i<10;++i)
            {
                snprintf(hashchar,sizeof(hashchar),"%02x",hash[i]);
                temp += hashchar;
            }
            
        }

        SHA1((unsigned char *)temp.c_str(),strlen(temp.c_str()),hash);
        temp = "";
        for(int i = 0;i<10;++i)
        {
            snprintf(hashchar,sizeof(hashchar),"%02x",hash[i]);
            temp += hashchar;
        }
    }
    //cout<<"\n\n<------newhash---->"<<temp<<"\n\n";
    return temp;
}

string makepath(char *path)
{
    char *tok;
    vector<string>split;
    tok = strtok(path, " ");
    while(tok != NULL)
    {
        split.push_back(tok);
        tok = strtok(NULL,"/");
    }

}

void download(string ip,int port,string hash,int index)
{
    int sock = create_connection(ip,port);
    if(sock < 0)
        logfile<<get_time()<< "Thread failed to create socket\n";
    else
    {
        string _send = "id$"+hash;
        char buff[24];
        strcpy(buff,_send.c_str());
        send(sock,buff,strlen(buff),0); 
        logfile<<get_time()<<" Request for chunk IDs sent to PEER: "<<index;
    }
}

int main(int argc,char *argv[])
{   
    fflush(stdin);
    string client(argv[1]);
    string tracker1(argv[2]);
    string tracker2(argv[3]);
    string delim = ":";
    vector<string>ip;
    string hash;
    char hello[1024];

    thread listener(client_listener,client);

    logfile.open(argv[4],ios::out | ios::app);

    if(!logfile)
        cout<<"error creating log file\n";

  else
  {
    logfile<<get_time()<<" Log file creation successful\n";
    blank();
    command_prompt();  
    
    fflush(stdin);
    cin.getline(inp,sizeof(inp));
    tok = strtok(inp, " ");
    tokens.clear();
    while(tok != NULL)
    {
        tokens.push_back(tok);
        tok = strtok(NULL," ");
    }


    if(tokens[0] == "share")
    {
        torrent.open(tokens[2].c_str(),ios::out | ios::out);
       
        if(!torrent)
        {
            logfile<<get_time()<<" Torrent file creation failed.\n";
        } 

        else
        {
          
            logfile<<get_time()<<" Client trying to initiate share.\n";
            logfile<<get_time()<<" Torrent file creation successful.\n";
            create_mtorrent(client);
            create_mtorrent(tracker1);
            create_mtorrent(tracker2);
            create_mtorrent(tokens[1]);

            //string path = makepath(tokens[1].c_str());
            hash = file_read((tokens[1]).c_str());
            create_mtorrent(hash);

            int sock = create_connection(tracker1.substr(0,tracker1.find(':')),stoi(tracker1.substr(tracker1.find(':')+1)));
            if(sock < 0)
            {
                cout<<"Connection attemptfor sharing failed\n";
            }
            else
            {
                strcpy(hello,tokens[0].c_str());
                strcat(hello,(delim).c_str());
                strcat(hello,client.c_str());
                strcat(hello,(delim).c_str());
                strcat(hello,(hash).c_str());
                send(sock , hello , strlen(hello) , 0 ); 
                printf("Share request sent successfully\n");   
                close(sock);
             
            }

        }
        
    }

    else if(tokens[0] == "get")
    {
        char buffer[1024];
        int sock = file_get(tokens[1]);
        if(sock < 0)
        {
            logfile<<get_time()<<" Connection attempt for downloading failed \n";
        }
        else
        {
            getfile.open(tokens[2].c_str(),ios::out);
            string hash = gethash();
            strcpy(hello,tokens[0].c_str());
            strcat(hello,(delim).c_str());
            strcat(hello,hash.c_str());
            send(sock , hello , strlen(hello) , 0 ); 
            logfile<<get_time()<<" GET request sent to tracker successfully\n";   
            int bytesread = read(sock, buffer,sizeof(buffer));
            buffer[bytesread] = '\0';
                           
            /* separate out the IPs and PORTs recieved from tracker */
            tracker_tokens.clear();
            tok = strtok(buffer, ":");
            while(tok != NULL)
            {
                tracker_tokens.push_back(tok);
                tok = strtok(NULL,":");
            }

            for(int i = 0;i<tracker_tokens.size();i+=2)
            {
                download(tracker_tokens[i],stoi(tracker_tokens[i+1]));
            }
        }
    }

    else if(tokens[0] == "exit")
    {
        char ch;
        cout<<"\nClient Exiting!!! Press any key to continue ";
        cin>>ch;
        exit(0);
    }  
 }           

return 0;
}
