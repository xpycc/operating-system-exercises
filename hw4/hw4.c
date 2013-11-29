#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<readline/readline.h>
extern char **environ;

/* parses out command line arguments and runs a single command, without redirection or pipes */
void parseAndExec(char *part) {
	if(strchr(part,' ')>0) {
		fprintf(stderr,"WARNING: arguments to commands not supported\n");
		return;
	}
	// TODO: use strsep to populate the args array from the command string
	char *args[10]={part,0};
	
	// absolute or relative path, not relying on the PATH environment variable
	if(strchr(args[0],'/'))
		execve(args[0],args,environ);
	else {
		/* TODO: use getenv("PATH") to find the appropriate binary to run */
		fprintf(stderr,"WARNING: auto-path feature not implemented\n");
	}
}

/* extracts redirection operators and applies these before calling parseAndExec() */
void handleRedirects(char *part) {
	char buf[255];
	memset(buf,0,255);
	memcpy(buf,part,strcspn(part,"<>"));
	if(strlen(part)!=strlen(buf)) {
		fprintf(stderr,"WARNING: redirection feature not implemented\n");
		/* TODO: find the redirection operators, and use open() and dup2() to redirect stdin(0) or stdout(1) to/from the indicated files */
	}
	parseAndExec(buf);	
}


void startCommand(char* part) {
	int pid=fork();
	if(!pid)
		handleRedirects(part);
	else {
		//		close(0);
		//		close(1);
		int stat_loc;			
		waitpid(pid,&stat_loc,0);
	}
}

/* splits the full command line into a number of separate commands separated by pipe ('|') characters. Calls startCommand() on
	 each of these commands, with appropriate redirection of input and output */
void startPipes(char *command) {
	char *commandptr = command;
	char *part = strsep(&commandptr,"|");				

	// if there are no pipes, just start the command
	if(!commandptr) {
		startCommand(part);
		return;
	}

	fprintf(stderr,"WARNING: pipe feature not implemented\n");
	/* TODO: use pipe(), fork(), dup2() and waitpid to start all piped commands simultaneously, 
		 with input / output redirected through the pipes. 

		 HINT: I used a recursive solution, where the child is responsible for the "tail" of the list 
		 (recursive call), and the parent executes the one command at the head, and then waits for the child.
	*/
}

char *prompt() {
	static char prompt[102];
	char pwd[100];
	getcwd(pwd,100);
	sprintf(prompt,"[%s/]: ",pwd);
	return prompt;
}

/* look for and execute built-in commands in line. 
	 return 0 if no built-in command was found, else return 1. */
int builtinCommands(char *line) {
	if(strcmp("exit",line)==0) {
		exit(0);
		return 1;
	}
	else if(strncmp("cd ",line,3)==0) {
		chdir(line+3);
		return 1;
	}
	return 0;
}


int main(int argc, char ** argv) {
	if(argc>1) {
		int scriptfile = open(argv[1],O_RDONLY);
		dup2(scriptfile,0); // read from file
	}

	int origin = dup(0);
	int origout = dup(1);

	while(1) {
		char *line = readline(prompt());
		if(line && *line) add_history(line);

		if(!line) return 0;
		if(line[0]=='#') continue;

		if(argc>2) 
			fprintf(stderr,"WARNING: command line arguments not yet implemented\n");
		/* TODO: replace any occurrences of $1,$2,$3...$9 with argv[2],argv[3] etc. */

		if(!builtinCommands(line)) {
			startPipes(line);
			
			// return stdin and stdout to normal after each command
			dup2(origin,0);
			dup2(origout,1);
		}
		free(line);
	}	
}
