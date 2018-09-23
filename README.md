# Bit-Torrent
WHAT IS MINI BIT-TORRENT
A mini bit torrent implemented in c++.
This project aims to implement a bit torrent client and tracker using pure C/C++.
The application developed and tested on UBUNTU 14.x.

COMPILING
The project compiles using g++ compile statments.

COMPILING CLIENT:
g++ -std=c++0x -o client main.cpp -lcrypto

RUN CLIENT:
./client <clientip:port>  <tracker1-ip:port>  <tracker2-ip:port>  <logfile>
  
COMPILING TRACKER:
g++ -std=c++0x -o tracker server.cpp

RUN CLIENT:
./tracker

PACKAGE:
The package includes two source,one each for server and client.

FUTURE PERSPECTIVE:
- To handle synchronisation b/w trackers
- To handle downloading of files.
  
