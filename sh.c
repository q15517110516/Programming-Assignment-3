///*
// * Sample Project 2
// *
// */
//
//#include <stdio.h>
//#include <string.h>
//#include <strings.h>
//#include <limits.h>
//#include <unistd.h>
//#include <stdlib.h>
//#include <pwd.h>
//#include <utmpx.h>
//#include <dirent.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//#include <signal.h>
//#include "sh.h"
//#include <glob.h>
//#include <sys/time.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//
//pid_t cpid;
//
//
//void *watchmail(void *);
//int noclobber = 0;
///* function that starts the shell
// * argc: argument count
// * argv: the array of arguments given
// * envp: the array of ponters to environment variables
// */
//
//int sh( int argc, char **argv, char **envp)
//{
//  //Setting up variables needed for the shell to function
//  char * prompt = calloc(PROMPTMAX, sizeof(char));
//  char * prefix = "";
//  char *commandline = calloc(MAX_CANON, sizeof(char));
//  char *command, *arg, *commandpath, *p, *pwd, *owd;
//  char **args = calloc(MAXARGS, sizeof(char*));
//  int uid, i, status, argsct, go = 1;
//  struct passwd *password_entry;
//  char *homedir;
//  
//  struct pathelement *pathlist;
//  int count;
//  char *token;
//  struct prev_cmd* head = NULL;
//  struct alias* alist = NULL;
//  int space;
//  int valid;
//  uid = getuid();
//  password_entry = getpwuid(uid);               /* get passwd info */
//  homedir = password_entry->pw_dir;		/* Home directory to start
//						  out with*/
// 
//  // store the current working directory  
//  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
//  {
//    perror("getcwd");
//    exit(2);
//  }
//  owd = calloc(strlen(pwd) + 1, sizeof(char));
//  memcpy(owd, pwd, strlen(pwd));
//
//  // Set up the initial prompt
//  prompt[0] = ' '; prompt[1] = '['; prompt[2] = '\0';
//
//  strcat(prompt, pwd);
//  prompt[strlen(prompt)+3] = '\0';
//  prompt[strlen(prompt)] = ']';
//  prompt[strlen(prompt)] = '>';
//  prompt[strlen(prompt)] = ' ';
//
//  /* Put PATH into a linked list */
//  pathlist = get_path();
//
//  while ( go )
//  {
//    /* print your prompt */
//    valid = 1;
//    printf("%s%s", prefix, prompt); 
//    
//    // Read in command line
//	if(fgets(commandline, MAX_CANON, stdin) == NULL){
//		commandline[0] = '\n';
//		commandline[1] = '\0';
//		valid = 0;
//		printf("\n");
//	}
//	int space = 1;
//	
//	// Remove newline character from end of input
//	if (strlen(commandline) > 1){
//		commandline[strlen(commandline) - 1] = '\0';
//	}
//	else {
//		valid = 0;
//	}	
//	
//	// Check command for special cases
//	for(i = 0; i < strlen(commandline); i++){
//		if(commandline[i] != ' '){
//			space = 0;
//		}
//	}
//	if (space){
//		commandline[strlen(commandline)-1] = '\n';
//		valid = 0;
//	}	
//	
//
//	// Parse the command line to separate the arguments
//	count = 1;
//	args[0] = strtok(commandline, " ");
//	while((arg=strtok(NULL, " ")) != NULL){
//		args[count] = arg;
//		count++;
//	}
//   	args[count] = NULL;
//	argsct = count;
//
//	// Check if command is an alias
//	struct alias* curr = alist;
//	int found = 0;
//	int done = 0;
//	if(argsct == 1){
//		while(curr != NULL && !done){
//			found = 0;
//			if(strcmp(curr->name, args[0]) == 0){
//				strcpy(commandline, curr->cmd);
//				found = 1;
//				count = 1;
//				args[0] = strtok(commandline, " ");
//				while((arg=strtok(NULL, " ")) != NULL){
//					args[count] = arg;
//					count++;
//				}
//   				args[count] = NULL;
//				argsct = count;
//				if(curr->used == 1){
//					args[0] = "\n\0";
//					printf("Alias Loop\n");
//					done = 1;
//				}
//				curr->used = 1;
//			}
//			curr = curr->next;
//			if(found) {
//				curr = alist;
//			}
//		}
//	}
//	
//	// Reset (used) aspect of each alias struct
//	curr = alist;
//	while(curr!=NULL){
//		curr->used = 0;
//		curr=curr->next;
//	}
//	
//        // Check for each built in command
//	command = args[0];
//	if(strcmp(command, "exit") == 0){
//		// Exit the shell
//		printf("Executing built-in exit\n");
//		exit(0);
//	}
//	else if(strcmp(command, "which") == 0){
//		// Finds first alias or file in path directory that 
//		// matches the command
//		printf("Executing built-in which\n");
//		if(argsct == 1){
//			fprintf(stderr, "which: Too few arguments.\n");
//		}
//		else{
//			// Checks for wildcard arguments
//			glob_t globber;
//			int i;
//			for(i = 1; i < argsct; i++){
//				int globreturn = glob(args[i], 0, NULL, &globber);
//				if(globreturn == GLOB_NOSPACE){
//					printf("glob: Runnning out of memory.\n");
//				}
//				else if(globreturn == GLOB_ABORTED){
//					printf("glob: Read error.\n");
//				}
//				else{
//				
//					if(globber.gl_pathv != NULL){
//						which(globber.gl_pathv[0], pathlist, alist);
//					}
//					else {
//						which(args[i], pathlist, alist);
//					}
//				}
//			}
//		}		
//	}
//	else if(strcmp(command, "noclobber") == 0){
//		noclobber = noclobber ^ 1;
//	}
//	else if (strcmp(command, "watchmail") == 0) {
//		runWatchMail(args, argsct);
//	}
//	else if (strcmp(command, "watchuser") == 0) {
//		runWatchUser(args, argsct);
//	}
//	else{
//
//        // If the command is not an alias or built in function, find it
//        // in the path
//	int found = 0;
//	int status;
//	char* toexec = malloc(MAX_CANON*sizeof(char));
//	if(access(args[0], X_OK) == 0){
//		found = 1;
//		strcpy(toexec, args[0]);
//	}
//	else{
//		struct pathelement* curr = pathlist;
//		char *tmp = malloc(MAX_CANON*sizeof(char));
//		
//		while(curr!=NULL & !found){
//			snprintf(tmp,MAX_CANON,"%s/%s", curr->element, args[0]);
//			if(access(tmp, X_OK)==0){
//				toexec = tmp;
//				found = 1;		
//			}
//			curr=curr->next;
//		}
//	}
//	
//	// If the command if found in the path, execute it as a child process
//	if(found){
//
//		printf("Executing %s\n", toexec);
//		
//		// Create a child process
//		cpid = fork();
//		
//		struct itimerval timer;		
//		
//		
//		if(cpid == 0){
//			// Child process executes command	
//			execve(toexec, args, envp);	
//		}
//		else if(cpid == -1){
//			perror(NULL);
//		}
//		else {
//			// Parent process (shell) times child process 
//			if(argc > 1){
//				timer.it_value.tv_sec = atoi(argv[1]);
//				timer.it_interval.tv_sec = 0;
//				if(setitimer(ITIMER_REAL, &timer, NULL)==-1){
//					perror(NULL);
//				}
//			}
//
//			// Parent process (shell) waits for child process to terminate
//			waitpid(pid, &status, 0);
//
//			// Disable timer after child process terminates
//			if(argc > 1){
//				timer.it_value.tv_sec = 0;
//				if(setitimer(ITIMER_REAL, &timer, NULL)==-1){
//					perror(NULL);
//				}
//			}
//
//			// Return non-normal exit status from child
//			if(WIFEXITED(status)){
//				if(WEXITSTATUS(status) != 0){
//					printf("child exited with status: %d\n", WEXITSTATUS(status));
//				}
//			}
//		}
//	}
//
//	// Give error if command not found
//        else if (valid){
//          fprintf(stderr, "%s: Command not found.\n", args[0]);
//        }
//      }
//  }
//  return 0;
//} /* sh() */
//
//
//
///* function call for 'which' command
// * command: the command that you're checking
// * pathlist: the path list data structure
// * alist: the alias data structure
// */
//char *which(char *command, struct pathelement *pathlist, struct alias *alist )
//{
//	// Set up path linked list variables
//	struct pathelement *curr = pathlist;
//	char *path = malloc(MAX_CANON*(sizeof(char)));
//	int found = 0;
//	
//
//	// Search aliases for command
//	struct alias* curra = alist;
//	while(curra != NULL){
//		if(strcmp(curra->name, command)==0){
//			printf("%s:\t aliased to %s\n", curra->name, curra->cmd);
//			found = 1;
//		}
//		curra=curra->next;
//	}
//
//	// Search path for command
//	while(curr != NULL && !found){
//		strcpy(path, curr->element);
//		path[strlen(path)+1] = '\0';
//		path[strlen(path)] = '/';
//		strcat(path, command);
//		if(access(path, F_OK) == 0){
//			printf("%s\n", path);
//			found = 1;
//		}
//		curr = curr->next;
//	}
//
//	// Print error if command not found
//	if (!found){
//		fprintf(stderr, "%s: command not found\n", command);
//		return NULL;
//	}
//
//} /* which() */
//
//struct User* findUser(struct User* h, char *name)
//{
//	struct User* p = h;
//	while (p != NULL)
//	{
//		if (strcmp(p->name, name) == 0)
//			return p;
//		p = p->next;
//	}
//	return NULL;
//}
//
//struct User* addUser(struct User* h, char *name)
//{
//	if (findUser(h, name) != NULL)
//		return h;
//	struct User*u = (struct User*)malloc(sizeof(struct User));
//	char *q = malloc(sizeof(name));
//	strcpy(q, name);
//	u->name = q;
//	u->next = h;
//	return u;
//}
//
//struct User* removeUser(struct User* h, char *name)
//{
//	struct User* u = findUser(h, name);
//	if (u == NULL)
//		return h;
//	if (u == h)
//		return h->next;
//	struct User* q = h;
//	while (q->next != u)
//		q = q->next;
//	q->next = u->next;
//	return h;
//}
//
//struct Mail* findMail(struct Mail *head, char *file)
//{
//	struct Mail* p = head;
//	while (p != NULL)
//	{
//		if (strcmp(p->file, file) == 0)
//			return p;
//		p = p->next;
//	}
//	return NULL;
//}
//
//struct Mail* insertMail(struct Mail* head, char *fileName)
//{
//	struct Mail* p = findMail(head, fileName);
//	if (p != NULL)
//		return head;
//	struct Mail *m = (struct Mail*)malloc(sizeof(struct Mail));
//	m->file = fileName;
//	pthread_create(&m->threadID, NULL, (void *)watchmail, (void *)m);
//	m->next = head;
//	return m;
//}
//
//void redirect(int fd, char *file, int mode)
//{
//	int fid = open(file, mode, 0666);
//	close(fd);
//	dup(fid);
//	close(fid);
//}
//
//void restoreRedirect()
//{
//	int fid = open("/dev/tty", O_WRONLY);
//	close(1);
//	dup(fid);
//	close(fid);
//
//	//fid = open("dev/tty", O_WRONLY);
//	dup2(1, 2);
//	//close(fid);
//
//	fid = open("/dev/tty", O_RDONLY);
//	close(0);
//	dup(fid);
//	close(fid);
//}
//
//void removeRedirection(char **args, int n)
//{
//	args[n - 2] = NULL;
//	args[n - 1] = NULL;
//}
//
//int checkRedirect(char **args, int *n)
//{
//	if (*n > 2)
//	{
//		if (strcmp(args[*n - 2], ">") == 0) {
//			if (noclobber == 1 && access(args[*n - 1], F_OK) == 0)
//			{
//				printf("%s: File exists\n", args[*n - 1]);
//				return -1;
//			}
//			redirect(1, args[*n - 1], O_CREAT | O_WRONLY);
//			removeRedirection(args, *n);
//		}
//		else if (strcmp(args[*n - 2], ">&") == 0) {
//			if (noclobber == 1 && access(args[*n - 1], F_OK) == 0)
//			{
//				printf("%s: File exists\n", args[*n - 1]);
//				return -1;
//			}
//			redirect(1, args[*n - 1], O_CREAT | O_WRONLY);
//			redirect(2, args[*n - 1], O_CREAT | O_WRONLY);
//			removeRedirection(args, *n);
//		}
//		else if (strcmp(args[*n - 2], ">>") == 0) {
//			if (noclobber == 1 && access(args[*n - 1], F_OK) != 0)
//			{
//				printf("%s: No such file or directory\n", args[*n - 1]);
//				return -1;
//			}
//			redirect(1, args[*n - 1], O_CREAT | O_WRONLY | O_APPEND);
//			removeRedirection(args, *n);
//		}
//		else if (strcmp(args[*n - 2], ">>&") == 0) {
//			if (noclobber == 1 && access(args[*n - 1], F_OK) != 0)
//			{
//				printf("%s: No such file or directory\n", args[*n - 1]);
//				return -1;
//			}
//			redirect(1, args[*n - 1], O_CREAT | O_WRONLY | O_APPEND);
//			redirect(2, args[*n - 1], O_CREAT | O_WRONLY | O_APPEND);
//			removeRedirection(args, *n);
//		}
//		else if (strcmp(args[*n - 2], "<") == 0) {
//			redirect(0, args[*n - 1], O_RDONLY);
//			removeRedirection(args, *n);
//		}
//	}
//	return 0;
//}
//
//struct User* users_to_watch = NULL;
//
//struct User* getCurrentUsers()
//{
//	struct utmpx *up;
//	setutxent();
//	struct User *p = NULL;
//	while (up = getutxent())
//	{
//		if (up->ut_type == USER_PROCESS)
//			p = addUser(p, up->ut_user);
//	}
//	return p;
//}
//
//void* watchuser(void)
//{
//	struct User* users = NULL;
//	users = getCurrentUsers();
//	while (1)
//	{
//		struct User*newusers = getCurrentUsers();
//		struct User *p = users_to_watch;
//		while (p)
//		{
//			if (findUser(users, p->name) != NULL && findUser(newusers, p->name) == NULL)
//				printf("User %s logged out.\n", p->name);
//			if (findUser(users, p->name) == NULL && findUser(newusers, p->name) != NULL)
//				printf("User %s logged in.\n", p->name);
//			p = p->next;
//		}
//		users = newusers;
//		sleep(20);
//	}
//}
//
//void runWatchUser(char **args, int n)
//{
//	int watchUserCreated = 0;
//	if (watchUserCreated == 0)
//	{
//		pthread_t tid;
//		pthread_create(&tid, NULL, (void *)watchuser, NULL);
//		watchUserCreated = 1;
//	}
//	if (n == 2)
//		users_to_watch = addUser(users_to_watch, args[1]);
//	else
//	{
//		if (n != 3 || strcmp(args[2], "off") != 0)
//		{
//			printf("Invalid argument.\n");
//			return;
//		}
//		users_to_watch = removeUser(users_to_watch, args[1]);
//	}
//}
//
//void *watchmail(void *arg)
//{
//	struct Mail* m = (struct Mail*)arg;
//	int size = -1;
//	while (1)
//	{
//		struct stat s;
//		stat(m->file, &s);
//		int newsize = s.st_size;
//		if (newsize > size && size != -1)
//		{
//			struct timeval tv;
//			gettimeofday(&tv, NULL);
//			printf("\aYou've Got Mail in %s at %s\n", m->file, ctime(&(tv.tv_sec)));
//		}
//		size = newsize;
//		sleep(1);
//	}
//}
//
//void runWatchMail(char **args, int n)
//{
//	struct Mail* mails = NULL;
//	if (n != 2 && n != 3)
//	{
//		fprintf(stderr, "Invalid argument.\n");
//		return;
//	}
//	if (access(args[1], F_OK) != 0)
//	{
//		fprintf(stderr, "File %s not exists\n", args[1]);
//		return;
//	}
//	if (n == 2)
//	{
//		char *p = malloc(strlen(args[1]));
//		strcpy(p, args[1]);
//		mails = insertMail(mails, p);
//	}
//	else
//	{
//		if (strcmp(args[2], "off") != 0)
//		{
//			fprintf(stderr, "Invalid argument\n");
//			return;
//		}
//		struct Mail *p = mails;
//		struct Mail *pre = NULL;
//		while (p != NULL)
//		{
//			if (strcmp(p->file, args[1]) == 0)
//				break;
//			pre = p;
//			p = p->next;
//		}
//		if (p)
//		{
//			if (p == mails)
//				mails = mails->next;
//			else
//				pre->next = mails->next;
//			pthread_cancel(p->threadID);
//			free(p);
//		}
//		else
//			printf("not found\n");
//	}
//}
//
//
