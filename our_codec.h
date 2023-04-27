/**
 * Authors: 
 * Lior Nagar
 * Shaked Levi
*/
#include <stdio.h>
/**
 * This method is responsible to handle the flags:
 * '-d' -> decryption
 * '-e' -> encryption
*/
void flags(char* flag);

/**
 * This method gets the stdinput from the user and splits it into commands.
*/
void tokenize(char buffer[]);

/**
 * Pipe support for multiple commands
*/
void pipe_handle(char **buffer, int index);

/**
 * Checks how many cores does the system has, 
 * and will operate accordinly in the calculations
*/
void cpu_cores();