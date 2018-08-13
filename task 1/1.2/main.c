#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	pid_t pid;
	while(1) {
		pid = fork();
		if (pid == 0) {							
			/*child code*/
			char *string;
			char* strings[100];
			printf("Print the console command: ");
			/*Чтение строки*/ 
			fgets(string, 1024, stdin);
			int i;
			
			/*Декомпозиция на токены*/
			for (i = 0; (strings[i] = strsep(&string, " ")); i++);
			
			/* strsep в последнюю строку в конце писала лишний символ,
			 * который порождал ошибки в аргументах команд, заменяю его
			 * символом конца строки */
			strings[i - 1][strlen(strings[i - 1]) - 1] = '\0'; 

			/*построение пути до бинарника исходя из команды*/
			char *path = "/bin/";
			char full_path[64];
			snprintf(full_path, sizeof path, "%s%s", path, strings[0]);
			/*Запуск команды*/
			execv(full_path, strings);
		}
		else if (pid > 0) {
			/*parent code*/
			wait(0);
			
		}
		else {
			perror("can't create a child");
		}
	}
	return 0;
}


