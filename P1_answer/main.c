#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_sindex.h"

#define SEC_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(void)
{
	struct timeval begin_t, end_t;

	gettimeofday(&begin_t, NULL);	// 프로그램 시작 시간

	ssu_sindex();			// ssu_sindex 프로그램 실행

	gettimeofday(&end_t, NULL);	// 프로그램 종료시간
	ssu_runtime(&begin_t, &end_t);	// 프로그램 실행 시간 출력 후 종료
	exit(0);
}
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	// 초단위 계산
	end_t->tv_sec -= begin_t->tv_sec;

	// 마이크로초단위 자리내림
	if (end_t->tv_usec < begin_t->tv_usec) {
		end_t->tv_sec--;
		end_t->tv_usec += SEC_TO_MICRO;
	}

	// 마이크로초단위 계산
	end_t->tv_usec -= begin_t->tv_usec;
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
}
