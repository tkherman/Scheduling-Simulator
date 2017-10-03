CSE.30341.FA17: Project 02
==========================

This is the documentation for [Project 02] of [CSE.30341.FA17].

Members
-------

1. Herman Tong (ktong1@nd.edu)
2. John McGuinness (jmcguin1@nd.edu)

Design
------

> 1. You will need at least two types of structures:
>
>   - **Process**: This should keep track of each job along with any accounting
>     information such as **command** and **PID**.
>
>   - **Scheduler**: This should keep track of the jobs in running and waiting
>     queues along with other information such as **policy**, **number of
>     CPUs**, and **time slice duration**.
>
>   With these in mind, consider the following questions:
>
>   - What sort of **accounting** with you need to keep track of for each
>     **Process**?
		- We need to keep track of the pid, command, state, user time, threshold,
		  cpu usage, arrival time, and start time for each process.  To do so, we
		  created a process struct that holds all the above information.
>
>   - How will you compute the turnaround and response times for each
>     **Process**?
		- To calculate the turnaround and response times, we have to keep track
		  of when each job arrives, starts, and finishes.  We will compute the
		  response time (start time - arrival time) when a job starts and the
		  turnarount time (finish time - arrival time) when a job finishes.
>
>   - What information do you need to store in the **Scheduler**?  How will it
>     maintain a running and waiting queue?
		- In the Scheduler, we need to store a queue for waiting jobs and a queue
		  for running jobs.  Exactly how this works depends on the scheduling
		  policy.  For both FIFO and Round Robin, the waiting queue simply
		  stores jobs that are waiting to be run and the running queue stores
		  running jobs.  For MLFQ, the waiting queue only holds the jobs that
		  have yet to start, and the running queue works the same.  However, for
		  MLFQ, there is also a vector of queues to keep track of the jobs on
		  each priority level.
		  
>   - How will you compute the average turnaround and response times for the
>     whole process queue?
>       - We have variables that keep track of the turaround and repsonse 
		  as well as the number of processes considered.  We update the average
		  times when a job starts or finishes.

Response.

> 2. Debugging this project will be difficult without good logging.  Because
>    timing is an important component of scheduling, it will be useful to have
>    a consistent logging mechanism that includes timestamps.
>
>   - How will you go about logging information in your program?
>       - We will use a macro to print the information below to the stdout when
		  logging.  We will also use another macro to aid with debugging.
>
>   - What sort of information will you log?
>       - For each loggin message, we will include a timestamp, a description of
>         the action, the pid and the command that's being run.
>       - When a process finishes, we will also log the turnaround and response
>         times.

Response.

> 3. Jobs in the process queue eventually become processes which need to be
>    created, preempted, resumed, and terminated.
>
>   - How will you create a process and execute its command?
>
>       Note: you will not want to use `/bin/sh -c command` in this assignment
>       since that creates two processes and you would only have direct control
>       of `/bin/sh` rather than `command`
>
>       - We will be using fork/exec/wait and will get the pid of the new 
          process from fork().
>
>   - How will you implement preemption?  That is, how will you **stop** or
>     **pause** a running process?
>       - We will send the SIGSTOP signal to the process to pause it.
>
>   - How will you **resume** a process that has been preempted?
>       - To resume a paused process, we will send it the SIGCONT signal.
>
>   - How will you **terminate** an active process?
>       - To terminate an active process, we will send it the SIGTERM signal.
>
>   - How will you gather statistics or accounting information about each
>     process?  What will you store?
>       - We will open the /proc/{pid}/stat file and parse it to calculate
> 		  the user time, CPU usage, and state of the process.  
>
>       [Hint](https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat)

Response.

> 4. The scheduler will need to activated under two conditions: when a process
>    dies and after some period of time (ie. time slice has expired).
>
>   - How will you trigger your scheduler when a process dies?  What must
>     happen when a process dies (consider both the **Scheduler** and the
>     **Process**)?
>       - We will write a function that handles SIGCHLD to detect if a process 
>		  dies, then use sigaction to call a handler that calls waitpid() to 
>		  find out which process has died. The process struct will be removed 
>		  and the scheduler will have to decide what process to run next.
>
>   - How will you ensure your scheduler runs periodically after a time slice
>     has expired?
>       - this is answered in question 6
>
>       Note: you may wish to consider how your response to question 6 can help
>       with this.

Response.

> 5. The client and server need to communicate via a request and response
>    model.
>
>   - Which IPC mechanism will you use: named pipes or unix domain sockets?
>       - We think unix domain sockets is the better choice because they allow
>         bi-directional communication whereas named pipes only allow one-way
>         communication which means we would have to create two separate pipes.
>
>   - How will you utilize this IPC mechanism?
>       - We will use this with poll() to detect if client is sending any new
>         processes.
>
>       Note: you may wish to consider this response in light of your answer in
>       question 6.

Response.

> 6. The server will need to perform its scheduling duties and process requests
>    from clients.
>
>   - How will you multiplex I/O and computation?
>       - We will use poll() to check if there's anything to process with the UNIX
>         domain sockets. When there is something to process, we will handle the
>		  request; otherwise, we'll continue with the scheduling.
>
>   - How will you ensure that your I/O will not block indefinitely?
>       - We will set a timeout for the poll() call so that it would not block
>         indefinitely.
>
>   - How will you allow events such as a child process dying interrupt your
>     I/O, but block such an event from interrupting your normal scheduling
>     functions?  Why would this be necessary?
>       - When performing scheduling functions, we will use sigprocmask(SIG_BLOKC) 
>		  to block and unblock.

Response.

> 7. Although FIFO is straightforward, both Round Robin and Multi-Level
>    Feedback Queue require preemption and some extract accounting.
>
>   - How will you perform preemption?  What happens to a process when it is
>     prempted?
>       - In the event loop, we will use the poll() timer to as the time slice 
>		  timer.  We will preempt a process by sending it the SIGSTOP signal.
>       - When a process is preempted, it will be paused.
>           - For Round Robin, this process simply goes to the back of the
>             waiting queue.
>           - For MLFQ, the process priority will be lowered if it has used up
>			  its entire time slice, then the scheduler will decide what will 
>			  run next depending on the priority table.
>
>   - How will MLFQ determine if a process needs to be lowered in priority?
>     What information must be tracked and how it be updated?
>       - We will have to determine if a process is I/O heavy.  In other words,
>		  this means that we will have to keep track of the time a process spends 
>		  in user mode as opposed to kernel mode.  We get this information by
>		  parsing the /proc/[PID]/stat file.  With this information, we can
>		  calculate if a process's priority level needs to be lowered.
>
>   - How will MFLQ determine if a priority boost is required?
>       - After a certain number of time slices we boost every process to level 1.

Response.

Demonstration
-------------

> Place a link to your demonstration slides on [Google Drive].

Errata
------

> Describe any known errors, bugs, or deviations from the requirements.

Extra Credit
------------

> Describe what extra credit (if any) that you implemented.
>
>   For the extra credit, we did both protocol and multiple clients. In order to
>   acheive them, we made changes to our client.cpp and server.cpp.
>
>   For the client.cpp, we modified it so that after the initial request sent to
>   the server, the client will keep the socket connection and continuously
>   prompt user to input further request. When the user is done, he/she simply
>   has to type exit to close the connection.
>
>   For the server.cpp, we extended the poll() call to poll on the listening
>   socket and file descriptors of existing connected clients. All the file
>   descriptors are stored in a vector of which the underlying array structure
>   is passed to the poll() call. In addition, the poll() also poll for the
>   closing of existing connection usuing POLLHUP. This way, closed conncetion
>   can be removed from the vector.




[Project 02]:       https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/project02.html
[CSE.30341.FA17]:   https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/
[Google Drive]:     https://drive.google.com
