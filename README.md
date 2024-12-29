# TightFileTransfer
File transfer over a single network socket.

#Overview
I am using this project to learn about network programming. I have implemented a client and a server.
The client is run from the terminal window and you have to provide an argument with the filename of 
the file you wish to send to the server.
The server is then listening for connectiond and when the client connects it recives the filename
and the stores the file in it's run directory.

I have then created some test data. There are several files that I have used, but ended up writing a bash script
to produce a large text file which is then used as the baseline. The batch file is called create_data.sh.
Another bash script is called loop.sh. This runs the client the client 50 times and records the time from 
start to end of each time the client is called. When the client has been run 50 times it shows the total
time that the script used. This time is the basis of the comparison below.

I have created some branches in the repository which represents the different "milestones" for the server.
In the beginning it was implemented as a single threaded server as shown in the single_thread branch.

I then improved my server to use threads. Every connection is handled by a new thread and once the 
data is recieved the thread will die on the return of the data handling function.
This is the multi_thread branch.

##Performence improvements:

-Branch single_thread:  49 seconds
-Branch muliti_thread:  13 seconds 
