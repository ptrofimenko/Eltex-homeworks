#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int fd[2];
int main() {
	pid_t pid, pid2;
	int status;
	
	while(1) {
		if(pipe(fd) < 0){ perror("Can\'t create pipe\n"); } 
		pid = fork();
		if (pid == 0) {							
			/*child code*/
			int isconveyor = 0;
			//int fd[2];
			char *string;
			char *buf_str;
			char* strings[100];
			//printf("Print the console command: ");
			/*Чтение строки*/ 
			fgets(string, 1024, stdin);
			int i;
			
			/*Декомпозиция на токены*/
			for (i = 0; (strings[i] = strsep(&string, " ")); i++) {
				/* завершаем декомпозицию если дошли до символа конвейра, 
				 * чтобы передать остаток строки следующему процессу */
				if (!strcmp(strings[i], "|")) {
					isconveyor = 1;
					break;
				}	
			}
			strings[i] = NULL;
			if (isconveyor == 1) {
				int j;
				pid2 = fork();
				if (pid2 > 0) {
					//for (j = 0; j < i; j++) { printf("%s\n", strings[j]); }
					dup2(fd[1], 1);
					close(fd[0]);
					//wait(0);
					
					execvp(strings[0], strings);
				}
				else if (pid2 == 0) { 
					for (i = 0; (strings[i] = strsep(&string, " ")); i++);
					strings[i - 1][strlen(strings[i - 1]) - 1] = '\0';
					//for (j = 0; j < i; j++) { printf("%s\n", strings[j]); }
					dup2(fd[0], 0);
					close(fd[1]);
					sleep(1);
					execvp(strings[0], strings);
				}
				else { perror("can't create a child"); }
					
			}
			else {
				/* strsep в последнюю строку в конце писала лишний символ,
				* который порождал ошибки в аргументах команд, заменяю его
				* символом конца строки */
				strings[i - 1][strlen(strings[i - 1]) - 1] = '\0'; 

				/*построение пути до бинарника исходя из команды*/
				//snprintf(full_path, sizeof path, "%s%s", path, strings[0]);
				/*Запуск команды*/
				execvp(strings[0], strings);
				
			}
		}
		else if (pid > 0) {
			/*parent code*/
			close(fd[0]);
			close(fd[1]);
			waitpid(pid, &status, 0);
		}
		else { printf("can't create a child"); }
	}
	return 0;
}


