#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

void parse_input(cmd_buff_t *cmd_buff) {
    char *ptr = cmd_buff->_cmd_buffer;  // Pointer to command buffer
    cmd_buff->argc = 0;  // Reset argument count
    int cmd_count = 0;

    while (*ptr) {
        // Skip leading spaces
        while (*ptr == ' ') ptr++;  
        if (*ptr == '\0') break;  // End of input buffer

        // Handle pipe as a command separator
        if (*ptr == '|') {
            cmd_count++;
            ptr++;  // Skip the pipe character
            continue;
        }

        // If we encounter a quote, parse the quoted argument
        if (*ptr == '"') {
            ptr++;  // Skip the opening quote
            cmd_buff->argv[cmd_buff->argc++] = ptr;  // Store the argument start pointer
            
            // Move ptr forward until the closing quote is found
            while (*ptr && *ptr != '"') ptr++;

            // Null-terminate the quoted argument and move past the closing quote
            if (*ptr == '"') {
                *ptr = '\0';  // Null-terminate the quoted argument
                ptr++;        // Move past the closing quote
            }
        } else {
            // Handle non-quoted arguments: Store argument
            cmd_buff->argv[cmd_buff->argc++] = ptr;

            // Move forward until space or pipe is encountered
            while (*ptr && *ptr != ' ' && *ptr != '|') ptr++;

            // Null-terminate argument and move past the null character
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
        }
    }

    // Ensure the argument list is null-terminated
    cmd_buff->argv[cmd_buff->argc] = NULL;

    // Set total number of commands
    cmd_buff->argc = cmd_count + 1;
}


int exec_local_cmd_loop() {
    command_list_t cmd_list;
    cmd_buff_t cmd_buff;
    int rc = OK;

    // Allocate memory for the command buffer
    cmd_buff._cmd_buffer = (char *)malloc(SH_CMD_MAX);
    if (cmd_buff._cmd_buffer == NULL) {
        perror("Failed to allocate memory for command buffer");
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT);  // Show prompt
        if (fgets(cmd_buff._cmd_buffer, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;  // Exit loop if EOF or error occurs
        }

        // Remove the trailing newline
        cmd_buff._cmd_buffer[strcspn(cmd_buff._cmd_buffer, "\n")] = '\0';

        // Check if the input is empty
        if (strlen(cmd_buff._cmd_buffer) == 0) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }

        // Check for exit command
        if (strcmp(cmd_buff._cmd_buffer, EXIT_CMD) == 0) {
            break;  // Exit the loop if "exit" command is entered
        }

        // Build command list
        if (build_cmd_list(cmd_buff._cmd_buffer, &cmd_list) != OK) {
            fprintf(stderr, "Failed to build command list\n");
            continue;
        }

        // Execute pipeline
        if (execute_pipeline(&cmd_list) != OK) {
            continue;
        }
    }

    // Free the allocated buffer before exiting
    free(cmd_buff._cmd_buffer);
    return OK;
}


int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (!cmd_line || !clist) return ERR_CMD_OR_ARGS_TOO_BIG;

    // Make a copy of cmd_line to avoid modifying the original one
    char *cmd_copy = strdup(cmd_line);
    if (cmd_copy == NULL) {
        return ERR_MEMORY; // Handle memory allocation failure for cmd_copy
    }

    char *cmd;
    char *temp = NULL;

    // Tokenize and process each command segment
    cmd = strtok(cmd_copy, " ");
    while (cmd) {
        // If we encounter a "|", move to the next command
        if (strcmp(cmd, "|") == 0) {
            if (temp) {
                cmd_buff_t *current_cmd_buff = &(clist->commands[clist->num]);
                current_cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);

                if (current_cmd_buff->_cmd_buffer == NULL) {
                    free(cmd_copy);
                    return ERR_MEMORY; // Handle memory allocation failure
                }

                strcpy(current_cmd_buff->_cmd_buffer, temp);
                parse_input(current_cmd_buff); // Process the current command
                clist->num++;
                free(temp); // Free temp after processing
                temp = NULL; // Reset temp for the next command
            }
        } else {
            // Concatenate the current command segment to temp
            if (temp == NULL) {
                temp = strdup(cmd); // Initialize temp with the first token
            } else {
                char *new_temp = (char *)malloc(strlen(temp) + strlen(cmd) + 2); // +1 for space, +1 for null-terminator
                if (new_temp == NULL) {
                    free(cmd_copy); // Free the original cmd_copy
                    free(temp); // Free temp
                    return ERR_MEMORY; // Handle memory allocation failure
                }
                sprintf(new_temp, "%s %s", temp, cmd); // Concatenate temp and cmd with a space
                free(temp); // Free the old temp string
                temp = new_temp; // Update temp to the new concatenated string
            }
        }

        cmd = strtok(NULL, " "); // Get the next token
    }

    // After the loop finishes, process the last command if any
    if (temp) {
        cmd_buff_t *current_cmd_buff = &(clist->commands[clist->num]);
        current_cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);
        if (current_cmd_buff->_cmd_buffer == NULL) {
            free(cmd_copy); // Free the original cmd_copy
            free(temp); // Free temp
            return ERR_MEMORY; // Handle memory allocation failure
        }
        strcpy(current_cmd_buff->_cmd_buffer, temp);
        parse_input(current_cmd_buff);
        clist->num++;
        free(temp); // Free temp after processing
    }

    free(cmd_copy); // Free the cmd_copy after we're done
    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int status;

    if (clist->num == 0) {
        printf("%s\n", CMD_WARN_NO_CMD);
        return WARN_NO_CMDS;
    }

    if (clist->num > CMD_MAX) {
        printf("%s\n", CMD_ERR_PIPE_LIMIT);
        return ERR_TOO_MANY_COMMANDS;
    }

    int pipes[CMD_MAX - 1][2];  // Pipes between commands
    pid_t pids[CMD_MAX];        // Process IDs

    // Create necessary pipes
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    // Create processes for each command
    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) {  // Child process
            // Redirect input for all except the first command
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // Redirect output for all except the last command
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipes in the child process
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute command
            if (execvp(clist->commands[i].argv[0], clist->commands[i].argv) == -1) {
                perror("execvp");
                exit(ERR_EXEC_CMD);  // Exit with error code if execvp fails
            }
        }
    }

    // Parent process: close all pipe ends
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes and return the last command's status
    int exit_status = OK;
    for (int i = 0; i < clist->num; i++) {
        if (waitpid(pids[i], &status, 0) == -1) {
            perror("waitpid");
            return ERR_EXEC_CMD;
        }

        if (WIFEXITED(status)) {
            int child_exit_status = WEXITSTATUS(status);
            if (child_exit_status != 0) {
                exit_status = child_exit_status;  // Store the failing exit status
            }
        }
    }

    return exit_status;
}