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
>       - we need to keep track of the pid, command, state, user, threshold, cpu
>         usage, arrival time and start time
>       - to do the above, we will create a struct for each process that holds
>         all the information listed
>
>   - How will you compute the turnaround and response times for each
>     **Process**?
>       - we have to keep track of the time that all jobs arrive, start and
>         finish
>       - for each process, we will compute the turnaround time when the job
>         finishes and the response time when the job starts
>
>   - What information do you need to store in the **Scheduler**?  How will it
>     maintain a running and waiting queue?
>       - queue for waiting jobs
>       - FIFO
>           - keep track of number of running jobs
>       - Round Robin
>           - a running queue
>       - MLFQ
>           - a vector of queues where the index of is the level and the queue
>             is just like a queue in Round Robin
>
>   - How will you compute the average turnaround and response times for the
>     whole process queue?
>       - have two variables that keeps track of the average turaround and
>         repsonse times that we constantly update when a job starts or finishes

Response.

> 2. Debugging this project will be difficult without good logging.  Because
>    timing is an important component of scheduling, it will be useful to have
>    a consistent logging mechanism that includes timestamps.
>
>   - How will you go about logging information in your program?
>       - we will use a macro to print the information below to the stdout
>
>   - What sort of information will you log?
>       - for each loggin message, we will include a timestamp, description of
>         the action, the pid and the command that's being run
>       - when a process finishes, we will also log the turnaround and response
>         time

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
>       - we will be using fork/exec/wait and will get the pid of the new 
        process from fork()
>
>   - How will you implement preemption?  That is, how will you **stop** or
>     **pause** a running process?
>       - we would send SIGSTOP signal to the process to pause it
>
>   - How will you **resume** a process that has been preempted?
>       - to resume, we will send SIGCONT to the process
>
>   - How will you **terminate** an active process?
>       - send a SIGTERM signal
>
>   - How will you gather statistics or accounting information about each
>     process?  What will you store?
>       - for the CPU usage, we will open the /proc/{pid}/stat file and parse it
>         to calculate the %CPU usage
>
>       [Hint](https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat)

Response.

> 4. The scheduler will need to activated under two conditions: when a process
>    dies and after some period of time (ie. time slice has expired).
>
>   - How will you trigger your scheduler when a process dies?  What must
>     happen when a process dies (consider both the **Scheduler** and the
>     **Process**)?
>       - use SIGCHLD to detect if a process dies, then use sigaction to call a
>         handler that calls waitpid() to find out which process died
>       - the process struct will be removed and the scheduler will have to
>         decide what process will be run next
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
>       - we think unix domain sockets is the choice because it allows
>         bi-directional communications where as named pipes only allow one
>         direction communication which means we have to create two separate
>         pipes
>
>   - How will you utilize this IPC mechanism?
>       - we can use this with poll() to detect if client is sending any new
>         processes 
>
>       Note: you may wish to consider this response in light of your answer in
>       question 6.

Response.

> 6. The server will need to perform its scheduling duties and process requests
>    from clients.
>
>   - How will you multiplex I/O and computation?
>       - we will use poll() to check if there's anything to process in the UNIX
>         domain sockets
>       - when there's something, we will process such request. otherwise, we
>         will continue with the scheduling duties
>
>   - How will you ensure that your I/O will not block indefinitely?
>       - we will set a timeout for the poll() call so that it would not block
>         indefinitely
>
>   - How will you allow events such as a child process dying interrupt your
>     I/O, but block such an event from interrupting your normal scheduling
>     functions?  Why would this be necessary?
>       - when performing scheduling functions, we will use
>         sigprocmask(SIG_BLOKC) to block and unblock when performing the
>         schduling function

Response.

> 7. Although FIFO is straightforward, both Round Robin and Multi-Level
>    Feedback Queue require preemption and some extract accounting.
>
>   - How will you perform preemption?  What happens to a process when it is
>     prempted?
>       - in the event loop, use the poll() timer to as the time slice timer so
>           - the issue may be if client keeps sending jobs, the scheduler may
>             be called repeatedly but that is ok i guess
>           - perhaps we can keep track of when the scheduler is last called
>       - when a process is preempted, it will be paused
>           - for Round Robin, this process simply goes to the back of the
>             waiting queue
>           - for MLFQ, the process priority will be lowered then the scheduler
>             will decide what will be running next depending on the priority
>             table
>
>   - How will MLFQ determine if a process needs to be lowered in priority?
>     What information must be tracked and how it be updated?
>       - we would have to determine if a process is I/O heavy
>           - this means that we would have to keep track of the time the
>             process spends in user mode as opposed to kernel mode.
>           - using this we along with the time slices we can determine if the
>             process is I/O heavy
>       - if process is I/O heavy, we will keep it at high priority.
>       - otherwise, we will lower the priority of a process after each time
>         slice
>       - we will use a variable threshold to keep track of how much alloted the
>         process has used. this would involve keep track of the time the
>         process spends on user mode and deduct that from the threshold.
>       - once a process uses all its alloted time, it will be demoted
>
>   - How will MFLQ determine if a priority boost is required?
>       - after a certain number of time slices or cpu cycles, boost everything

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




[Project 02]:       https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/project02.html
[CSE.30341.FA17]:   https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/
[Google Drive]:     https://drive.google.com
