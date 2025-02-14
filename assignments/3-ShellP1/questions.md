1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

   > **Answer**: Yes, as it has the ability to read data and determine whether or not it has exceeded the buffer size. The ability to identify newlines and other empty spaces aids with formatting as well.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

   > **Answer**: Malloc enables dynamic memory allocation, allowing for variable input size. It may result in either too much memory being allocated or not enough memory for the necessary command if it was a fixed-size array.

3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

   > **Answer**: It would require extra memory if spaces weren't reduced. Due to parsing problems, the shell may potentially understand the command incorrectly.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

  > **Answer**:  
  >  Both stdout and stderr can be redirected independently: 2> reroutes stderr to error.txt, and > reroutes stdout to output.txt. Separating the file descriptors for each of the redirection symbols and parsing them will be difficult.

  Changing the direction of stdin: stdin would read inputs from a text document. It will be difficult to identify the < symbol, obtain the file descriptors, open the file, and deal with issues related to reading and handling.

  Instead of overwriting, stdout appends: Instead of overwriting, >> would append the output to a text document. It would be difficult to manage making a new file or locating one with the name, then opening it and attaching it.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

  > **Answer**: Redirection involves file inputs and outputs. The transfer of outputs as inputs from one command to another is known as piping.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

  > **Answer**: It makes it simple to debug and locate the findings by accurately identifying various outcomes.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

  > **Answer**: We can show them independently to prevent misunderstandings. In contrast, we would simply combine them with redirection if we wanted to: 2>&1 reroutes STDERR to STDOUT.
