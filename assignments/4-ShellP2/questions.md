1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

   > **Answer**: The fork system function duplicates the running process to create a new one. Using fork before calling execvp has the advantage of enabling us to execute a new program in a different process without interfering with the parent process. While the new application is being run by the child process, the parent process can continue to function independently. Without a fork, the shell would not execute as the new program would replace the existing one.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

   > **Answer**: Fork() returns -1 and sets errno to reflect the error if it fails. Exhaustion of resources or going over the system's limit on the number of processes are frequent causes of failure. To deal with the failure, we check to see if fork() returns -1 in our implementation. If it does, depending on the situation, we either break the loop or terminate the program and use perror() to display an error message.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

   > **Answer**: The PATH environment variable is used by the execvp() function to find the command in the folders it specifies. The PATH variable is divided into a list of directories, and the command is searched across each directory until it is located. execvp() returns -1 and sets errno to reflect the error (often ENOENT, meaning "No such file or directory") if the command cannot be located in any directory.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

   > **Answer**: The parent process's wait() system function makes sure that it waits for the child process to complete running before proceeding. When the parent process quits without collecting its exit status, the child process turns become a zombie process if wait() is not invoked. Because the child's process entry stays in the process table until the parent retrieves its status, this may result in resource leaks.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

   > **Answer**: It is a macro that is used to extract the exit status of a child process after it finishes.It will then return a integer to show if it failed or not. It is important because it allows the parent process to get the exit value.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

   > **Answer**: I implemented a parsing function that handles the quotes by using a variable to track spaces inside quotes and it will decide if it should be part of the argument or not. This will make sure arguments like "Hello World" are treated as one entity and also allowing arguments like "Hello WOrld " to work"

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

   > **Answer**: In the new one I used strtok() to manually go through the command buffer to handle quoted arguments. It was a bit hard because I had a change a lot. The one I had before was used to format and parse the entire line including "|", so that was challenging.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

  > **Answer**: IPC usually uses more structured and dependable communication techniques like message queues or shared memory, however signals enable asynchronous communication between processes, giving them a means of notification or interruption.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

  > **Answer**: SIGKILL forcibly terminates a process, SIGTERM requests a process to terminate gracefully, and SIGINT interrupts a process (often from the keyboard).

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

  > **Answer**: When a process receives SIGSTOP, it is paused and cannot be caught or ignored because it is a stop signal designed to pause execution.
