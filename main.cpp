
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <string.h> 

#define PORT 8080 
#define __STDC_WANT_LIB_EXT1__ 1
using namespace std;
#define CHUNKSIZE (512*1024)

fstream logfile,torrent;
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


void share_torrent(string argument)
{   
    
    torrent<<argument<<"\n";

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

    char *tok;
    vector<string> tokens;
    string client(argv[1]);
    string tracker1(argv[2]);
    string tracker2(argv[3]);
    char input[100];

    logfile.open(argv[4],ios::out | ios::app);

    if(!logfile)
        cout<<"error creating log file\n";

    else
    {
        logfile<<get_time()<<" Log file creation successful\n";

        blank();
        command_prompt();   
        cin.getline(input,sizeof(input));

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
                logfile<<get_time()<<" Client trying to initiate mtorrent share.\n";
                logfile<<get_time()<<" Torrent file creation successful.\n";
                share_torrent(client);
                share_torrent(tracker1);
                share_torrent(tracker2);
                //string path = makepath(tokens[1].c_str());
                string hash = file_read((tokens[1]).c_str());
                share_torrent(hash);
            }
            
        }

        else if(tokens[0] == "get")
        {

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
