/* Kewen Gu, 09-11-2014 */
/* This is the MEClient program for receiving commands, sending to server for operation, and printing out the response from the server */

#include <stdio.h>         /* for printf() and fprintf() */
#include <stdlib.h>        /* for atoi() */
#include <sys/socket.h>    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>     /* for sockaddr_in and inet_addr() */
#include <string.h>        /* for memset() */
#include <unistd.h>        /* for close() */
#include <sys/types.h>     /* for addrinfo */
#include <netdb.h>         /* for getaddrinfo() */

#include "header.h"

int main(int argc, char *argv[])
{
	int sock, argNum, bytesRcvd, bufLen, entNum, cmdCount, packNum;
	char *portNum = WELLKNOWNPORT;
	char *hostName, *fileName, *ret, *recvBuf;
	char readBuf[MAXBUFSIZE], readBuf0[MAXBUFSIZE];
	char *args[MAXARGS];
	char *argRcvd[6];
	char *mode = NULL;
	struct addrinfo hints, *servinfo, *res;
	FILE *fp, *log;
	int i, j, flag = 0;

	/* Memory allocation */
	recvBuf = (char *)calloc(1, MAXBUFSIZE);

	/* Test for correct number of arguments */
	if ((argc < 2) || (argc > 3))
	{
		fprintf(stderr, "Usage: %s <MEServermachine> <MEClient.txt>\n", argv[0]);
		exit(1);
	}

	/* Store the hostname */
	hostName = argv[1];
	
	/* Configure the server address */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* Obtain the server address info */
	if(getaddrinfo(hostName, portNum, &hints, &servinfo) != 0)
	{
		fprintf(stderr, "Getting IP address failed\n");
		exit(1);
	}

	/* Try the obtained address one by one until connected */
	for (res = servinfo; res; res = res->ai_next)
	{
		/* Create a relaible, stream socket using TCP */
		if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
			DieWithError("socket() failed");
		
		/* Establish the connection to the echo server */
		if (connect(sock, res->ai_addr, res->ai_addrlen) < 0)
			DieWithError("connect() failed");
	}

	/* If the input file is specified, open the file for reading */
	if (argc == 3)
	{
		fileName = argv[2];
		if((fp = fopen(fileName, "r")) == NULL)
		{
			perror("fopen");
			exit(1);
		}
	}
	/* If the input file is not specified, read from the command line */
	else if (argc == 2)
		fp = stdin;

	/* Open the log file for writing */
	if((log = fopen("MEClient.log", "w")) == NULL)
	{
		perror("fopen");
		exit(1);
	}

	/* Get line from the file */
	ret = fgets(readBuf, MAXBUFSIZE - 1, fp);

	/* If read successful and is not the new line character '\n' */
	while ((ret != NULL) && (!!strcmp(ret, "\n")))
	{	
		/* Remove the new line character */
		bufLen = strlen(readBuf);
		if (readBuf[bufLen - 1] == '\n')
			(readBuf[bufLen - 1] = '\0');
		strncpy(readBuf0, readBuf, MAXBUFSIZE);
		/* Slice the input string into tokens */
		argNum = StrSlicing(readBuf0, args, " ");

		/* Switch state according to the command read */
		switch (CmdNumbering(args[0]))
		{
			case LOGIN:
				/* Check whether the input strings meet the requirements */
				if (StrCheck(argNum, args) == 0)
				{
					/* Set the mode to be the inputted user name */
					mode = strdup(args[1]);
					/* Send input string to the server */
					ClntServSend(readBuf, MAXBUFSIZE, sock);
					fprintf(log, "Sent: %s\n", readBuf);
					/* Receive message from the server */
					ClntServRecv(recvBuf, MAXBUFSIZE, sock);
					fprintf(log, "Received: %s\n", recvBuf);
					printf("================%s logged in=================\n", args[1]);
					printf("%s\n", recvBuf);
				}
			  break;

			case ADD:
				/* Check authentication */
				if (!strcasecmp(mode, "FEMA"))
				{
					/* Check whether the input strings meet the requirements */
					if (StrCheck(argNum, args) == 0)
					{
						/* Send input string to the server */
						ClntServSend(readBuf, MAXBUFSIZE, sock);
						fprintf(log, "Sent: %s\n", readBuf);
						/* Receive message from the server */
						ClntServRecv(recvBuf, MAXBUFSIZE, sock);
						fprintf(log, "Received: %s\n", recvBuf);
						printf("%s\n", recvBuf);
					}
				}
				else
					/* Print the error message */
					fprintf(stderr, "You do not have permission to issue %s\n", args[0]);
			  break;

			case UPDATE:
				/* Check authentication */
				if (!strcasecmp(mode, "FEMA"))
				{
					/* Check whether the input strings meet the requirements */
					if (StrCheck(argNum, args) == 0)
					{
						/* Send input string to the server */
						ClntServSend(readBuf, MAXBUFSIZE, sock);
						fprintf(log, "Sent: %s\n", readBuf);
						/* Revceive message from the server */
						ClntServRecv(recvBuf, MAXBUFSIZE, sock);
						fprintf(log, "Received: %s\n", recvBuf);
						printf("%s\n", recvBuf);
					}
				}
				else
					/* Print the error message */
					fprintf(stderr, "You do not have permission to issue %s\n", args[0]);
			  break;

			case REMOVE:
				/* Check authentication */
				if (!strcasecmp(mode, "FEMA"))
				{
					/* Check whether the input strings meet the requiements */
					if (StrCheck(argNum, args) == 0)
					{
						/* Send input string to the server */
						ClntServSend(readBuf, MAXBUFSIZE, sock);
						fprintf(log, "Sent: %s\n", readBuf);
						/* Receive message from the server */
						ClntServRecv(recvBuf, MAXBUFSIZE, sock);
						fprintf(log, "Received: %s\n", recvBuf);
						printf("%s\n", recvBuf);
					}
				}
				else
					/* Print the error message */
					fprintf(stderr, "You do not have permission to issue %s\n", args[0]);
			  break;

			case FIND:
				/* Check authentication */
				if (mode != NULL)
				{
					/* Check whether the input strings meet the requirements */
					if (StrCheck(argNum, args) == 0)
					{
						/* Send input string to the server */
						ClntServSend(readBuf, MAXBUFSIZE, sock);
						fprintf(log, "Sent: %s\n", readBuf);
						/* Receive the number of entries found from the server */
						ClntServRecv(&entNum, sizeof(int), sock);
						fprintf(log, "Received: %d\n", entNum);
						/* If the entry exists */
						if (entNum > 0)
						{
							printf("-----------------\n");
							printf("Found %d entry(s)\n", entNum);
							printf("-----------------\n");
							for(i = 0; i < entNum; i++)
							{	
								/* Receive the entry in one stream */
								ClntServRecv(recvBuf, MAXBUFSIZE, sock);
								fprintf(log, "Received: %s\n", recvBuf);
								/* Slice the received string into 5 tokens */
								if (StrSlicing(recvBuf, argRcvd, " ") == 5)
								{
									/* Print the body information */
									printf("Body No.%d:\n", i+1);
									printf("ID Number:  %s\n", argRcvd[0]);
									printf("First Name: %s\n", argRcvd[1]);
									printf("Last Name:  %s\n", argRcvd[2]);
									printf("Gender:     %s\n", argRcvd[3]);
									printf("Location:   %s\n", argRcvd[4]);
									printf("-----------------------------------------------\n");
								}
							}
						}
						else
							/* Print the error message */
							fprintf(stderr, "No entry found\n");
					}
				}
				else
					/* Print the error message */
					fprintf(stderr, "You do not have permission to issue %s\n", args[0]);
			  break;

			case LIST:
				/* Check authentication */
				if (mode != NULL)
				{	
					/* Check whether the input strings meet the requirements */
					if (StrCheck(argNum, args) == 0)
					{
						/* Send input string to the server */
						ClntServSend(readBuf, MAXBUFSIZE, sock);
						fprintf(log, "Sent: %s\n", readBuf);
						/* Receive the number of entries found from the server */
						ClntServRecv(&entNum, sizeof(int), sock);
						fprintf(log, "Received: %d\n", entNum);
						/* If the entry exists */
						if (entNum > 0)
						{
							printf("-----------------\n");
							printf("Found %d entry(s)\n", entNum);
							printf("-----------------\n");
							for(i = 0; i < entNum; i++)
							{
								/* Receive the entry in one stream */
								ClntServRecv(recvBuf, MAXBUFSIZE, sock);
								fprintf(log, "Received: %s\n", recvBuf);
								/* Slice the received string into 5 tokens */					
								if (StrSlicing(recvBuf, argRcvd, " ") == 5)
								{
									/* Print the body information */
									printf("Body No.%d:\n", i+1);
									printf("ID Number:  %s\n", argRcvd[0]);
									printf("First Name: %s\n", argRcvd[1]);
									printf("Last Name:  %s\n", argRcvd[2]);
									printf("Gender:     %s\n", argRcvd[3]);
									printf("Location:   %s\n", argRcvd[4]);
									printf("-----------------------------------------------\n");
								}
							}
						}
						else
							/* Print the error message */
							fprintf(stderr, "No entry found\n");
					}
				}
				else
					/* Print the error message */
					fprintf(stderr, "You do not have permission to issue %s\n", args[0]);
			  break;

			case LOCATE:
				/* Check authentication */
				if (mode != NULL)
				{
					/* Check whether the input strings meet the requirements */
					if (StrCheck(argNum, args) == 0)
					{
						/* Send input strings to the server */
						ClntServSend(readBuf, MAXBUFSIZE, sock);
						fprintf(log, "Sent: %s\n", readBuf);
						/* Reveive the number of entries found from the server */
						ClntServRecv(&entNum, sizeof(int), sock);
						fprintf(log, "Received: %d\n", entNum);
						/* If the entry exists */
						if (entNum > 0)
						{
							printf("-----------------\n");
							printf("Found %d entry(s)\n", entNum);
							printf("-----------------\n");
							for(i = 0; i < entNum; i++)
							{
								/* Receive the entry in one stream */
								ClntServRecv(recvBuf, MAXBUFSIZE, sock);
								fprintf(log, "Received: %s\n", recvBuf);
								/* Slice the received string into 5 tokens */			
								if (StrSlicing(recvBuf, argRcvd, " ") == 5)
								{
									/* Print the body information */
									printf("Body No.%d:\n", i+1);
									printf("ID Number:  %s\n", argRcvd[0]);
									printf("First Name: %s\n", argRcvd[1]);
									printf("Last Name:  %s\n", argRcvd[2]);
									printf("Gender:     %s\n", argRcvd[3]);
									printf("Location:   %s\n", argRcvd[4]);
									printf("-----------------------------------------------\n");
								}
							}
						}
						else
							/* Print the error message */
							fprintf(stderr, "No entry found\n");
					}
				}
				else
					/* Print the error message */
					fprintf(stderr, "You do not have permission to issue %s\n", args[0]);
			  break;

			case QUIT:
				/* Check whether the input strings meet the requirements */
				if (StrCheck(argNum, args) == 0)
				{	
					/* Send input strings to the server */
					ClntServSend(readBuf, MAXBUFSIZE, sock);
					fprintf(log, "Sent: %s\n", readBuf);
					/* Receive the quiting indicator from the server */
					ClntServRecv(recvBuf, MAXBUFSIZE, sock);
					fprintf(log, "Received: %s\n", recvBuf);

					if (!strcmp(recvBuf, "Closing..."))
					{
						/* Receive the number of command issued from the server */
						ClntServRecv(&cmdCount, sizeof(int), sock);
						fprintf(log, "Received: %d\n", cmdCount);
						printf("Total number of commands requested by %s: %d\n", mode, cmdCount);
						/* If end-of-file is specified */
						if ((args[1] != NULL) && (!strcasecmp(args[1], "EOF")))
						{
							/* Receive the number of packets sent by the server */
							ClntServRecv(&packNum, sizeof(int), sock);
							fprintf(log, "Received: %d\n", packNum);
							printf("Total number of packets received from server: %d\n", packNum);
							printf("=====================Quited======================\n");
							/* Set the flag to indicate end-of-file */
							flag = 1;
						}
						/* Set the mode to NULL */
						mode = NULL;
					}
					else
						/* Print the error message */
						fprintf(stderr, "Quit failed\n");
				}
			  break;

			default:
				/* Print the error message */
				fprintf(stderr, "Invalid command: %s\n", args[0]);
			  break;

		}
		/* Zero the readBuf and recvBuf */
		bzero(readBuf, MAXBUFSIZE);
		bzero(recvBuf, MAXBUFSIZE);
		/* Break out of the loop if flag is set */
		if (flag)
			break;
		/* Get new command from the file */
		ret = fgets(readBuf, MAXBUFSIZE - 1, fp);
	}
	/* close the socket and file */
	close(sock);
	fclose(fp);
	fclose(log);
	/* free memory */
	free(recvBuf);
}









