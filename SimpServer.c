/*------------------------------
* server.c
* Description: HTTP server program
* CSC 361
* Instructor: Kui Wu
-------------------------------*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>

#define MAX_STR_LEN 120         /* maximum string length */
#define SERVER_PORT_ID 9898     /* server port number */

void cleanExit();

/*---------------------main() routine--------------------------*
 * tasks for main
 * generate socket and get socket id,
 * max number of connection is 3 (maximum length the queue of pending connections may grow to)
 * Accept request from client and generate new socket
 * Communicate with client and close new socket after done
 *---------------------------------------------------------------------------*/

main(int argc, char *argv[])
{
    int newsockid; /* return value of the accept() call */
    char str[2048];
    int listen_fd;
    int port;
    char msg1[1024];
    char msg2[1024];
    chdir(argv[2]);
    time_t current_time;
    char* c_time_string;
    current_time = time(NULL);
    c_time_string = ctime(&current_time);
    port = atoi(argv[1]);
    strcpy(msg1,"---Response header ---\nHTTP/1.0 404 Not Found\n");
    sprintf(msg1,"%sDate: %s%s",msg1,c_time_string,"Server: Apache/2.2.31 (Unix) mod_jk/1.2.40");
    sprintf(msg2,"---Response header ---\nHTTP/1.0 200 OK\r\nDATE:%s\r%s \n--- Response body ---\n",c_time_string,"Server: Apache/2.2.31 (Unix) mod_jk/1.2.40n");
    struct sockaddr_in servaddr;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero( &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(port); //Set default to 80
    //test for the binding
    if (bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0 )
    {
        perror("Erroro in binding\n");
        cleanExit();
    }
    //test if is listened
    if (listen(listen_fd, 10) != 0)
    {
        perror("Error in listening\n");
        cleanExit();
    }
    while(1)
    {
        newsockid = accept(listen_fd, (struct sockaddr*) NULL, NULL);
        if (newsockid <0)
        {
            perror("Fail to create socket\n");
            exit(0);
        }
        else
        {
            perform_http(newsockid,str,msg1,msg2,port);
        }
    }
}

/*---------------------------------------------------------------------------*
*
* cleans up opened sockets when killed by a signal.
* 
*---------------------------------------------------------------------------*/

void cleanExit()
{
    exit(0);
}
/*---------------------------------------------------------------------------*
*
* Accepts a request from "sockid" and sends a response to "sockid".
*
*---------------------------------------------------------------------------*/
perform_http(int sockid, char str[], char msg1[], char msg2[], int port)
{
    // this is the method to send and receive the message from client
    char hostname[MAX_STR_LEN];
    char identifier[MAX_STR_LEN]="";
    char content[2048];
    char *combine;
    bzero(str, 2048);
    read(sockid,str,2048);
    if (strncmp(str,"GET",3) != 0)
    {
        strcpy(str, "---Response header ---\nHTTP/1.0 501 Not Implemented.");
        write(sockid, str, strlen(str)+1);
	
        	
    }
    else
    {
        parse_URI(str, &hostname, &identifier);
        // open file and write the content
        if(open_File(identifier, content) == 1)
        {
            //printf("Error in reading the file n");
            strcpy(str,msg1);
  	    str[2048]='\0';
            write(sockid, str, strlen(str)+1);
        }
        else
        {
            combine = strcat(msg2,content);
            strcpy(str,combine);
            printf("Echoing back -\n");
            str[2048]='\0';
            write(sockid, str, strlen(str)+1);
        }
    }
    //printf("%s\n",str);
    //cleanExit();
    close(sockid);
}

/*---------------------------------------------------------------------------*
*
* Open file and check if the file is exit or not.
*
*---------------------------------------------------------------------------*/

int open_File(char* directory, char* content)
{
    // open file and write the message into content
    char ch;
    FILE *fp;
    int i=0;
    printf("Getting the file...\n");
    fp = fopen(directory,"r"); // read mode
    if( fp == NULL )
    {
        perror("Error while opening the file.\n");
        return 1;
    }
    while( ( ch = fgetc(fp) ) != EOF )
    {
        content[i]=ch;
        i++;
    }
    fclose(fp);
    return 0;
}

/*------ Parse an "uri" into "hostname" and resource "identifier" --------*/

parse_URI(char *uri, char *hostname, char *identifier)
{ 
    // Done is for test if the parse is not working
    int Done;
    //*identifier = NULL;
    sscanf(uri, "GET http://%99[^/]/%199[^ ]", hostname, identifier) ;
    Done = 0;
}
