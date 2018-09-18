
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#define __STDC_WANT_LIB_EXT1__ 1

using namespace std;
#define CHUNKSIZE (512*1024)

fstream logfile,torrent;
bool flag = false;


void blank()
{
    printf("\033c" );
}

void command_prompt()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    printf("\033[%1;1H");
    printf("%c[2K", 27);
    printf("\033[30;");
    printf("47m");
    printf("Command Line: ");
    printf("\033[0m");
    printf(" ");
    fflush(stdin);   
}


string time_get()
{
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Must be static, otherwise won't work
    static char _retval[20];
    strftime(_retval, sizeof(_retval)," %H:%M:%S", timeinfo);
    
    return _retval;   
}

string file_read(char *filename)
{

    char hashchar[3] = "\0";
    unsigned char hash[20];
    unsigned char chunk[CHUNKSIZE];
    fstream input;
    size_t current_read;

    input.open(name,ios::binary | ios::in | ios::ate);
    

    if(!input)
        logfile<<time_get<<": error opening input file\n";
        logfile<<"\n"; 

    else
    {
        if(!torrent)
        {
            logfile<<time_get()<<": error";
        }

        client = client_ip;
        tracker1 = trak_ip1;
        tracker2 = trak_ip2;
        double size = input.tellg();
        long int num = ceil(size/CHUNKSIZE);
        cout<<"\nsize: "<<(int)size<<"chunks: "<<num<<"\n";  
        input.seekg(0,ios::beg);


        string temp;
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

        if(!torrent)
        {
            logfile<<"\nerror creating mtorrent file";
        }

        else
        {
            torrent << temp;
            input.close();
            torrent.close();
        }
    }

    return temp;
}


void share_torrent(string argument)
{   
    
    torrent<<argument<<"\n";

}

int main(int argc,char *argv[])
{   

    char *tok;
    vector<string> tokens;
    string client(argv[1]);
    string tracker1(argv[2]);
    string tracker2(argv[3]);

    logfile.open(argv[4],ios::out | ios::app);

    if(!logfile)
        cout<<"error creating log file\n";

    else
    {
        logfile<<time_get()<<": Log file creation successful\n";

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
           
            torrent.open(tokens[3].c_str(),ios::out | ios::trunc | ios::app);
           
            if(!torrent)
            {
                logfile<<"Torrent file creation failed!!!\n";
            } 

            else
            {
                share_torrent(client);
                share_torrent(tracker1);
                share_torrent(tracker2);
                string hash = file_read(tokens[1].c_str());
                share_torrent(hash);
            }
            
        }

        else if(tokens[0] == "get")
        {

        }

    }

   
             
    return 0;
}
