#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "ssu_functions.h"

#define PROMPT "20181224>> "

int main(void)
{
	char command[BUFMAX];
	pid_t pid;
	int status;

	while (1) {
		// 명령어 입력
		printf(PROMPT);
		memset(command, 0, BUFMAX);
		fgets(command, BUFMAX, stdin);

		// Enter 키 혹은 exit 입력
		if (strcmp(command, "\n") == 0)
			continue;
		else if (strcmp(command, "exit\n") == 0) {
			printf("Prompt End\n");
			break;
		}
		else	// 기타 다른 명령어 입력
			command[strlen(command)-1] = '\0';

		// 내장명령어 fmd5, fsha1, help를 실행하기 위한 자식 프로세스 생성
		pid = fork();
		if (pid == 0) {	// 자식 프로세스
			char *argv[ARGMAX];
			int argc;
			
			argc = parseCmd(command, argv);
		
			// fmd5/fsha1 명령어 실행	
			if (strcmp(argv[0], "fmd5") == 0 || strcmp(argv[0], "fsha1") == 0) {
				// 주어진 인자 중 하나라도 입력이 없으면 에러 처리
				if (argc != ARGMAX) {
					fprintf(stderr, "usage: %s [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n", argv[0]);
					exit(1);
				}
				 
				// fmd5/fsha1 명령어 실행
				if (execl(argv[0], argv[0], argv[1], argv[2], argv[3], argv[4], NULL) == -1)
				{
					fprintf(stderr, "%s: execl error\n", argv[0]);
					exit(1);
				}
			}
			else {	// help 명령어 실행
				if (execl("help", argv[0], NULL) == -1) {
					fprintf(stderr, "%s: execl error\n", argv[0]);
					exit(1);
				}
			}
		}

		// 부모 프로세스는 자식 프로세스 종료까지 대기
		while (wait(&status) != pid);
	}

	exit(0);
}
