#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void remove_extra_spaces(char *);

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    const char *src = user_str; 
    char *dest = buff;           
    int char_count = 0;          
    int is_prev_space = 0;        

    while (*src != '\0' && char_count < len) {
        if (*src == ' ' || *src == '\t') {
            if (!is_prev_space) {
                *dest++ = ' ';
                char_count++;
                is_prev_space = 1;
            }   
        } else {
            *dest++ = *src; 
            char_count++;
            is_prev_space = 0;
        }
        src++;
    }

    if (*src != '\0') {
        return -1; 
    }

    return char_count; 
}

void print_buff(char *buff, int len, int str_len) {
    printf("Buffer:  [");
    for (int i = 0; i < str_len; i++) {
        putchar(*(buff + i));
    }
    for (int i = str_len; i < len; i++) {
        putchar('.');
    }
    printf("]\n");
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len) {
    char *start = buff;
    int word_count = 0;
    int is_prev_space = 1;

    for (int i = 0; i < str_len && *buff != '\0'; i++) {
        if (*buff == ' ') {
            is_prev_space = 1;
        } else {
            if (is_prev_space) {
                word_count++;
            }
            is_prev_space = 0;
        }
        buff++;
    }

    return word_count;
}

void reverse_str(char *buff, int str_len) {
    int start = 0;
    int end = str_len - 1;
    char temp;

    while (start < end) {
        temp = buff[start];
        buff[start] = buff[end];
        buff[end] = temp;

        start++;
        end--;
    }
}

void word_print(char *buff, int str_len) {
    const char *src = buff;
    char word[256];
    int char_count = 0;
    int is_prev_space = 0;
    int counter = 1;

    printf("Word Print\n");
    printf("----------\n");
    for (int i = 0; i < str_len && *src != '\0'; ++i) {
        if (*src == ' ' || *src == '\t') {
            is_prev_space = 1;
        } else {
            if (is_prev_space) {
                word[char_count] = '\0';
                printf("%d. %s(%d)\n", counter, word, char_count);
                char_count = 0;
                counter++;
            }
            word[char_count] = *src;
            char_count++;
            is_prev_space = 0;
        }
        src++;
    }

    if (!is_prev_space && *src == '\0') {
        word[char_count] = '\0';
        printf("%d. %s(%d)\n\n", counter, word, char_count);
        printf("Number of words returned: %d\n", counter);
    }
}

void remove_extra_spaces(char *input) {
    int i = 0, j = 0;
    int in_word = 0;

    while (input[i] != '\0') {
        if (input[i] != ' ') {
            input[j++] = input[i];
            in_word = 1;
        } else if (in_word) {
            input[j++] = ' ';
            in_word = 0;
        }
        i++;
    }
    if (j > 0 && input[j - 1] == ' ') {
        j--;
    }
    input[j] = '\0';
}


//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string
    int i;

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    // This code ensures argv[1] exists and starts with '-' to avoid invalid access.
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag
    //printf("%c\n", opt);

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    
    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // Ensures at least three arguments are provided to proceed safely.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string
    remove_extra_spaces(input_string);

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char *) malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(99);
    }
    for (i = 0; i < BUFFER_SZ - 1 && input_string[i] != '\0'; i++) {
        buff[i] = input_string[i];
    }
    buff[i] = '\0';

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        case 'r':
            reverse_str(buff, user_str_len);
            break;
        case 'w':
            word_print(buff, user_str_len);
            break;
        case 'x':
            if (argc == 5){ 
                printf("Not Implemented!\n");
                exit(1);
            }

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting

    print_buff(buff, BUFFER_SZ, user_str_len);
    free(buff);

    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//          
//          PLACE YOUR ANSWER HERE
//          Ensures safety and supports buffers of different sizes.