# UNIX I/O 
The purpose of this assignment is to practice making UNIX I/O system calls in C. 
In a multiplexed manner, your main process will read from multiple pipes, from the standard input (the terminal), and write to the file system. 

Procedure 
Create five pipes and spawn five child processes. Connect a pipe to each child process. Each child process should write to its pipe, and the parent process should read from all the pipes. Recall that you read and write a pipe as if it were a file. 

Each of the first four child processes should generate time stamped messages to the 
nearest 1000th of a second (time starts at 0): 

0:00.123: Child 1 message 1
0:02.456: Child 1 message 2
 				 etc.

Each process should sleep for a random time of  0, 1, or 2 seconds between messages. 
Terminate the process after 30 seconds. 

Meanwhile, the fifth child process should repeatedly prompt at the terminal (standard out) and read one line of input (standard in) typed by the user. The process should write the message (with time stamp) to its pipe and then immediately prompt for the next message. Terminate the process after 30 seconds. 

After spawning the child processes, the parent process should repeatedly read lines 
from the pipes. It must use the select() system call to determine whether any of the 
pipes has any input. It should write the messages that it read from any of the pipes to an output file output.txt in the order that it read them. It should prepend each line with a time stamp to the nearest 1000th of a second. Therefore, each output line will have two time stamps: the first time stamp from the parent process, and the second time stamp from the child process. File output.txt will contain a mixture of lines from the child processes. 

Tip: Read the “man” pages about the select() system call. 

Terminate the parent process (and the program) after all the child processes have 
terminated. 

What to turn in 
Email a zip file to the Grader, Name the zip file after your Group#, for example, Group-3.zip. The file should contain: 

•	Your C source files 
•	Your output.txt file 
•	A brief report describing any issues you encountered. 
 Email your answers to the grader. Your subject line should be “COEN383 Assignment #6, Group-n” for example COEN383 Assignment #6, Group-3. Cc all your group members so that the grader can do a “Reply all” when needed. This is a group assignment; all group members will receive the same score.
