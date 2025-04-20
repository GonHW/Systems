Shell Project: shell.c
Names: Hench Wu		(NetID: hhw14) 

Compile: make all

Execute shell: ./shell

Program execution

My shell prints a > prompt and reads user input, supporting execution of both absolute and relative paths.

Supports execution like:

Absolute: /bin/ls -l

Relative: ../src/hello

If the file does not exist, my shell prints a detailed error message:

Example: ../src/hello: No such file or directory

For unqualified commands like ls, it searches /usr/bin and /bin and prints:

commandName: command not found if not found.

Features include:

Background Jobs: My shell detects & at the end and runs jobs in the background, printing job ID and PID.

Example:

bash
Copy
Edit
> ./hello &
[1] 4061
Job Management: Assigns job IDs (%1, %2, etc.) and tracks status: Running, Stopped, or Terminated.

Signal Handling:

Ctrl+C sends SIGINT to foreground jobs (terminates them).

Ctrl+Z sends SIGTSTP to foreground jobs (stops them).

My shell itself ignores signals and reaps zombie processes to prevent leaks.

On termination by signal, prints:

csharp
Copy
Edit
[1] 1464 terminated by signal 2
Built-in Commands Implemented:

cd [path]: Change current working directory and updates PWD. Defaults to HOME.

jobs: Lists all active jobs with ID, PID, status, and command.

bg <jobID>: Resumes a stopped job in the background.

fg <jobID>: Brings a job to the foreground.

kill <jobID>: Sends SIGTERM to the specified job.

exit: Sends SIGHUP to jobs and exits the shell cleanly.

Robust Input Handling:

Handles extra spaces, varied spacing, and no-argument commands like:

bash
Copy
Edit
   ./hello    &
   ./hello&
   ./hello  
Example Interactions:

Running a job and terminating:

bash
Copy
Edit
> /bin/sleep 100
^C
[1] 1464 terminated by signal 2
Suspending and managing jobs:

bash
Copy
Edit
> /bin/sleep 100
^Z
> jobs
[1] 1234 Stopped /bin/sleep 100


