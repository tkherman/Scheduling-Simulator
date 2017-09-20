CSE.30341.FA17: Project 02
==========================

This is the documentation for [Project 02] of [CSE.30341.FA17].

Members
-------

1. Domer McDomerson (dmcdomer@nd.edu)
2. Belle Fleur (bfleur@nd.edu)

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
>       - we will be using fork, exec, wait and get pid of the new process from
>         fork
>
>   - How will you implement preemption?  That is, how will you **stop** or
>     **pause** a running process?

        -UNSURE 
        -we do not think we can simply send the SIGSTOP signal to the process
          since it might not necessarily stop, but we are unsure how to approach
          this
>
>   - How will you **resume** a process that has been preempted?
>
>   - How will you **terminate** an active process?
>
>   - How will you gather statistics or accounting information about each
>     process?  What will you store?
>
>       [Hint](https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat)

Response.

> 4. The scheduler will need to activated under two conditions: when a process
>    dies and after some period of time (ie. time slice has expired).
>
>   - How will you trigger your scheduler when a process dies?  What must
>     happen when a process dies (consider both the **Scheduler** and the
>     **Process**)?
        -UNSURE
        -the scheduler will have to take over
>
>   - How will you ensure your scheduler runs periodically after a time slice
>     has expired?
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
>
>       Note: you may wish to consider this response in light of your answer in
>       question 6.

Response.

> 6. The server will need to perform its scheduling duties and process requests
>    from clients.
>
>   - How will you multiplex I/O and computation?
>
>   - How will you ensure that your I/O will not block indefinitely?
>
>   - How will you allow events such as a child process dying interrupt your
>     I/O, but block such an event from interrupting your normal scheduling
>     functions?  Why would this be necessary?

Response.

> 7. Although FIFO is straightforward, both Round Robin and Multi-Level
>    Feedback Queue require preemption and some extract accounting.
>
>   - How will you perform preemption?  What happens to a process when it is
>     prempted?
>
>   - How will MLFQ determine if a process needs to be lowered in priority?
>     What information must be tracked and how it be updated?
>       - have an external data structure that keeps track of the priority of
>         all jobs
>       - use something like a vector of linked list
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
