/*
 * @file cmd_proc.c
 * @author Shuran Xu (shxu6388@colorado.edu)
 * @brief - implements the functioning of the command processor
 * and defines the commands table.
 * @date 2021-11-07
 * @reference: the command table design is implemented based on the example shown in 
 * lecture 22.
 */

#include "cmd_proc.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "handlers.h"

/* Define the maximum length of the local command buffer and the
 * local command argument array*/
#define CMD_BUF_CAPACITY         		(32U)
#define MAX_ARG_NUM						(4)
/* Define the function pointer to be used as the command handler template */
typedef void (*command_handler_t)(int argc,char *argv[]);
/* Define the structure for the command table entry */
typedef struct {
	const char *name;
	command_handler_t handler;
	const char *help_string;
}command_table_t;

/* Define a constant command table with all required commands.
 * However, the commands are easy to append to the table.
 */
static const command_table_t commands[] = {
		//name 			handler 		help_string
		{"author", 		handle_author, 	"Print the author's name"},
		{"dump", 		hex_dump, 		"Print a hexdump of the memory requested, with up to 16 bytes per line of output"},
		{"info",        handle_info,    "Print the build information about the program"},
		{"help",        handle_help,    "Print a help message with info about all supported commands"},
};

/* Determine the number of commands supported */
static const int num_commands = sizeof(commands) / sizeof(command_table_t);


/**
 * @brief the command processing function. Inside the function, the received
 * command line will be parsed and the associated handler will be called.
 * @param cmd_buff the buffer holding the received command line
 */
static void process_cmd(char *cmd_buff)
{
	int argc = 0;
	char *argv[MAX_ARG_NUM];
	memset(argv,0,sizeof(argv));
	char *p = cmd_buff;
	char *end;
	bool in_token = false;

	/* find the end of string */
	for(end = cmd_buff;*end != '\0'; end++);

	/* tokenize input in place */
	for(p=cmd_buff;p < end; p++){

		if((*p != ' ') && (!in_token)){
			in_token = true;
			argv[argc++] = p;
		}
		if((*p == ' ') && in_token){
			*p = '\0';
			in_token = false;
		}
	}
	/* mark the end of argv */
	argv[argc] = NULL;
	/* return if no command */
	if(argc == 0)
		return;

	/* dispatch argc/argv to handler */
	for(int i=0; i<num_commands; i++) {
		if(strcasecmp(argv[0], commands[i].name) == 0) {
			commands[i].handler(argc, argv);
			return;
		}
	}
	/* output error messages for the invalid command */
	printf("\r\nUnknown command: %s", argv[0]);
	return;
}

/**
 * @brief define the command processor. It receives the
 * user inputs and executes the associated command, if
 * supported.
 */
void command_processor()
{
	uint8_t cmd_buffer[CMD_BUF_CAPACITY];
	memset(cmd_buffer,0,CMD_BUF_CAPACITY);
	int cmd_buf_index = 0;
	uint8_t character;

//	printf("? ");
	while(1) {

		character = getchar();
		if(character != 0xFF){

			if(character == '\b'){
				cmd_buf_index--;
				cmd_buffer[cmd_buf_index] = '\0';
				printf("\b \b");
			}
			else{
				if(character == '\r'){
					cmd_buf_index = 0; //reset the index
					process_cmd((char *)cmd_buffer);
					memset(cmd_buffer,0,CMD_BUF_CAPACITY);
					printf("\r\n");
				}
				else{
					cmd_buffer[cmd_buf_index++] = character;
				}
			}
		}
	}

	return;
}

