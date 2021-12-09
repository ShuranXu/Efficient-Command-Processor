/*
 * @file handlers.h
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief - header for the registered command handlers.
 * @date 2021-11-07
 */

#ifndef __HANDLER_H_
#define __HANDLER_H_

#include <stdint.h>

/**
 * @brief print out the author name.
 */
void handle_author();

/**
 * @brief prints a string representing a 'dump' of
 * the nbytes of memory starting at loc. Bytes are
 * printed up to 16 bytes per line, separated by newlines.
 *
 * @param argc the total number of arguments combined
 * with the function name
 * @param argv the array of arguments, which include
 * source address and the length
 */
void hex_dump(int argc,char *argv[]);

/**
 * @brief Print out the current build information
 * of the executable.
 */
void handle_info();

/**
 * @brief Print messages to help users to use the command processor.
 */
void handle_help();

/**
 * @brief PLay the tone at the specified frequency
 */
void handle_tone(int argc,char *argv[]);


#endif

