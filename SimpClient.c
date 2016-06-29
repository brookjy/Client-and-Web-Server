/*------------------------------
* client.c
* Description: HTTP client program
* CSC 361
* Instructor: Kui Wu
-------------------------------*/
  
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //for exit(0);
#include<sys/socket.h>
#include<errno.h> //For errno - the error number
#include<netdb.h> //hostent
#include<arpa/inet.h>
 
/* define maximal string and reply length, this is just an example.*/
/* MAX_RES_LEN should be defined larger (e.g. 4096) in real testing. */
#define MAX_STR_LEN 120
#define MAX_RES_LEN 120

/* --------- Main() routine ------------
 * three main task will be excuted:
 * accept the input URI and parse it into fragments for further operation
 * open socket connection with specified sockid ID
 * use the socket id to connect sopecified server
 * don't forget to handle errors
 */

void main(int argc, char **argv)
{
    char uri[MAX_STR_LEN];
    char hostname[MAX_STR_LEN];
    char identifier[MAX_STR_LEN]="";
    char ip[100];
    int sockid, port;
    //printf("Open URI: ");
    //scanf("%s", uri);
    //printf("Testing Begin...n");

    //test for parse
    if(parse_URI(argv[1], hostname, &port, identifier) == 0)
    {
        printf("Errorr in Parasingn ");
        exit(1);
    }
    
    //get the ip address
    if(hostname_to_ip(hostname, ip))
    {
        printf("Error resolving: %s\n", hostname);
        exit(1);
    }
    printf("port----->%d\n",port);

    // gernerate socket id
    sockid = open_connection(hostname, port, ip, identifier);

    // send and receive message
    perform_http(sockid, identifier, hostname);
}

/*------ Parse an "uri" into "hostname" and resource "identifier" --------*/

int parse_URI(char *uri, char *hostname, int *port, char *identifier)
{
    /* this method is to parse the content into parts*/
    *port = 80;
    //*identifier = NULL;
    if (strncmp(uri, "Http://",7) == 0)
    {
        uri = uri+7;
    }
    else if (sscanf(uri, "%99[^:]:%i/%199[^\n]", hostname, port, identifier) == 3)
    {
        return 1;
    }
    else if (sscanf(uri, "%99[^/]/%199[^\n]", hostname, identifier) == 2)
    {
        return 1;
    }
    else if (sscanf(uri, "%99[^:]:%i[^\n]", hostname, port) == 2)
    {
        return 1;
    }
    else if (sscanf(uri, "%99[^\n]", hostname) == 1)
    {
        return 1;
    }
    return 0;
}

/*------------------------------------*
* connect to a HTTP server using hostname and port,
* and get the resource specified by identifier
*--------------------------------------*/

perform_http(int sockid, char *identifier, char *hostname)
{
    /* connect to server and retrieve response */
    char sendline[2048];
    char recvline[2048];
    //while(1)
    bzero( sendline, 2048);
    bzero( recvline, 2048);
    sprintf(sendline, "GET http://%s/%s HTTP/1.0\r\n\r\n", hostname, identifier); //pass the request to 'sendline', and to change the Get to test for the 501 Not found
    write(sockid,sendline,strlen(sendline)+1);
    printf("---Request end---\n");
    printf("HTTP request sent, awaiting response...\n\n");
    read(sockid,recvline,2048);
    printf("%s\n",recvline);
    close(sockid);
}

/*---------------------------------------------------------------------------*
*
* open_conn() routine. It connects to a remote server on a specified port.
*
*---------------------------------------------------------------------------*/

int open_connection(char *hostname, int port, char* ip, char *identifier)
{
    // build ip and port and the path, and build up the connection between the server and client 
    int sockfd;  //init
    /* generate socket
    * connect socket to the host address
    */
    printf("---Request begin---\n");
    printf("GET http://%s/%s HTTP/1.0\r\n",hostname, identifier);
    printf("Host: %s\n", hostname);
    struct sockaddr_in servaddr;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(port);//port
    servaddr.sin_addr.s_addr=inet_addr(ip);//ip address
    //inet_pton(AF_INET,"10.10.1.100",&(servaddr.sin_addr));
    printf("Connection: Keep-Alive\n\n");
    if (connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
    {
        printf("Error in connection\n");
    }
    return sockfd;
}

/*---------------------------------------------------------------------------*
*
* Convert the hostname to ip address and use it to connect to the server
*
*---------------------------------------------------------------------------*/

int hostname_to_ip(char * hostname , char* ip)
{
    //convert the web name to actual ip address
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
    addr_list = (struct in_addr **) he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
    return 1;
}
