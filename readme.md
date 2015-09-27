Kewen Gu

Program 1

CS 3516

Sep 11, 2015


Introduction:
-------------
	This assignment is about writing a tcp client and server program which can ask user to input command include 'login', 'add', 'update', 'remove', 'find', 'list', 'locate' and 'quit'. 
	The client accepts the commands from command line or the specified file, then runs a set of test, sends the command to the server, waits for the response from the server and prints out the correct implementation to the standard output. The client will write all the communication data between client and server into a log file called MEClient.log.
	The server handle each command upon receiving the command from the client, and maintained a database consists of body entries. When user attempted to quit from the client, the server writes the database to a file called MEDatabase.txt.


How to run:
-------------
	To make and compile all the files, enter:
		make

	To run the MEClient program, enter:
		./MEClient <Server Hostname> <MEClient.txt (optional)>

	To run the MEServer program, enter:
		./MEServer


Note: 
-------------
	1. The "wellknown port number" used by both server and client is 4074.
	2. if the <MEClient.txt> is not specified, the program will get input from the standard input.
		Valid inputs to MEClient are:
		1) login <user_name>
		2) add <id_number> <first_name> <last_name> <gender> <location>
		3) update <id_number> <first_name> <last_name> <gender> <location>
		4) remove <id_number>
		5) find <first_name> <last_name>
		6) list <start(optional)> <finish(optional)>
		7) location <location> 
	



