#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <iostream>
#include <ctime>
#include <string>
#include <fstream>

std::string getCPUTemp()
{
   for(;;){
	
    std::string therm;
    int i =0;
    std::ifstream inFile("/sys/class/thermal/thermal_zone0/temp");
    if(inFile.fail()){ 
      std::cerr << "file cannot be read" << std::endl;
      exit(1);
    }
    while(inFile >> therm){
     i++;
     if(i==1)
	return therm;
    }
    inFile.close();
   }
}

void error(const char *msg) { perror(msg); exit(0); }

int main(int argc,char *argv[])
{
    /* first what are we going to send and where are we going to send it? */
    int portno =        6540;
    char *host =        "192.168.0.105";
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char message[1024];

    
while(1){

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL) error("ERROR, no such host");

    /* fill in the structure */
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);


    /* connect the socket */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
 

   std::string postString = "{\"body\" : \""+getCPUTemp()+"\"}";
   const char* data = postString.c_str();
   int size = strlen(data);
    
   char* message_fmt = "POST /rasp HTTP/1.0\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s";


   sprintf(message, message_fmt,size,data);
   printf("Request:\n%s\n", message);

    /* send the request */
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd,message+sent,total-sent);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent+=bytes;
    } while (sent < total);
sleep(5);

    /* close the socket */
    close(sockfd);
}
    printf("Stopped\n");
    return 0;
}
