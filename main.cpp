
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

#define PORT 8080 
#define __STDC_WANT_LIB_EXT1__ 1
using namespace std;
#define CHUNKSIZE (512*1024)

char *tok;
vector<string> tokens;

char input[100];
char buffer[1024] = {0}; 

fstream logfile,torrent,gettorrent,getfile;
bool flag = false;
struct winsize w;

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
        cout<<ip<<" "<<port<<"\n";
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

    }

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


int main(int argc,char *argv[])
{   

    string client(argv[1]);
    string tracker1(argv[2]);
    string tracker2(argv[3]);
    string delim = "$";
    vector<string>ip;
    string hash;
    char hello[1024];

    logfile.open(argv[4],ios::out | ios::app);

    if(!logfile)
        cout<<"error creating log file\n";

    else
    {
        logfile<<get_time()<<" Log file creation successful\n";

        blank();
        command_prompt();   
        cin.getline(input,sizeof(input));
        tokens.clear();
        tok = strtok(input, " ");
        while(tok != NULL)
        {
            tokens.push_back(tok);
            tok = strtok(NULL," ");
        }

        if(tokens[0] == "share")
        {   
            torrent.open(tokens[2].c_str(),ios::out | ios::app);
           
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

                int sock = create_connection("127.0.0.1",8880);
                if(sock < 0)
                {
                    cout<<"Connection attempt failed\n";
                }
                else
                {
                    size_t pos = client.find(':');
                    ip.push_back(client.substr(0,pos));
                    ip.push_back(client.substr(pos+1));

                    strcpy(hello,tokens[0].c_str());
                    strcat(hello,(delim).c_str());
                    strcat(hello,(ip[0]).c_str());
                    strcat(hello,(delim).c_str());
                    strcat(hello,(ip[1]).c_str());
                    strcat(hello,(delim).c_str());
                    strcat(hello,(hash).c_str());
                    cout<<hello<<"\n";
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
                logfile<<get_time()<<" Connection attempt failed\n";
            }
            else
            {
                getfile.open(tokens[2].c_str(),ios::out);
                string hash = gethash();
                cout<<"\nhere i am"<<hash<<"\n";
                strcpy(hello,tokens[0].c_str());
                strcat(hello,(delim).c_str());
                strcat(hello,hash.c_str());
                send(sock , hello , strlen(hello) , 0 ); 
                logfile<<get_time()<<" GET request sent to tracker successfully\n";   
                int bytesread = read(sock, buffer,sizeof(buffer));
                buffer[bytesread] = '\0';
                string input = buffer;
                cout<<bytesread<<"\n"<<input;
                //printf("%s\n",buffer);
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
