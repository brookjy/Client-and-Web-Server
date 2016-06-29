A simple Web Client and Web Server using socket programming:

Client:
The client c file can send a message to the ip address(if the address is: 10.10.1.100 then it connect to the local server), and receive the message 
from the server and print out the message in the program.

Server:
The server with the ip address(ip: 10.10.1.100) will accpet the message from the client sight, and the server is going to receive the request and find the indeifier file 
from the location. And if the the identifier file is found then client receive "200 OK", otherwise receive "404 Not Found"

To Get the "501 Not Implimented":
Change the line 98 in client.c, and you can change the "Get.." to "XXX..." and response of the server will be "501 Not Implimented".  

The file "abc.txt" is the target file that for testing the program.

