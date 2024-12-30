# TightFileTransfer
File transfer over a single network socket.

Christmas project 2024

## Overview
I am using this project to learn about network programming. I have implemented a client and a server.
The client is run from the terminal window and you have to provide an argument with the filename of 
the file you wish to send to the server.
The server is then listening for connections and when the client connects it recives the filename
and then stores the file in the server's run directory. I am using my stoml library for reading settings from
a toml file.

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

Further improvements were made. My server could potentially create hundreds of threads as each client 
connection created its own thread. If this would be the case the system where my server would be running
could run out of resources. This would affect my server's performance and also affect other applications
running on the same system. In order to mitigate this I created a thread pool with a client queue. 
This would then limit the amount of threads which would be created. To avoid active waiting which would use
up my CPU resource I used a condition variable to signal the threads in the pool from the main thread when 
a new client was added to the client queue. The server now works in the way that if there is client in the 
client queue then the threads in the thread pool work to recieve data and close the client connection. 
When there is no more work i.e the client queue is empty the threads go to sleep. 
They are woken again up again when a new client is added to the client queue.
This is the multi_thread_queue branch.

## Performence improvements:

- Branch single_thread:  49 seconds
- Branch muliti_thread:  13 seconds
- Branch multi_thread_queue: 13 seconds
