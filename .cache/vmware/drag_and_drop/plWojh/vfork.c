#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

// vfork를 통한 자식 프로세스 실행
void ssu_local_func(void);

int main(void)
{
	printf("Before vfork\n");
	ssu_local_func();
	printf("After ssu_local_func, my PID is %d\n", getpid());
	exit(0);
}
// vfork()를 통한 자식 프로세스 생성 및 실행
void ssu_local_func(void)
{
	pid_t pid;

	if ((pid = vfork()) == 0)	// 자식 프로세스
		printf("I'm child. My PID is %d\n", getpid());
	else if (pid > 0)		// 부모 프로세스
		sleep(3);
	else				// 자식 프로세스 생성 실패 시 에러 처리
		fprintf(stderr, "vfork error\n");
}