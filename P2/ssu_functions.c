// ssu_sdup.c, ssu_find-md5.c, ssu_find-sha1.c에서 공통으로 사용하는 함수 정의
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ssu_functions.h"

// 명령문 파싱
int parseCmd(char *cmd, char *argv[])
{
	int argc = 0;
	char *ptr = NULL;

	ptr = strtok(cmd, " ");
	while (ptr != NULL) {
		argv[argc++] = ptr;
		ptr = strtok(NULL, " ");
	}

	return argc;
}
// 총 검색시간 출력
void ssu_searchTime(struct timeval *begin_t, struct timeval *end_t)
{
	end_t->tv_sec -= begin_t->tv_sec;

	if (end_t->tv_usec < begin_t->tv_usec) {
		end_t->tv_sec--;
		end_t->tv_usec += SEC_TO_MICRO;
	}
	end_t->tv_usec -= begin_t->tv_usec;

	printf("Searching time: %ld:%06ld(sec:usec)\n\n", end_t->tv_sec, end_t->tv_usec);
}
// 파일 크기 출력: 천 단위마다 구분, 바이트 단위 출력
void putCommaToSize(long int size)
{
	char sizebuf[BUFMAX];
	char *digits;
	int len;
	
	len = snprintf(sizebuf, sizeof(sizebuf), "%ld", size);
	digits = sizebuf;
	for (int i = len; i > 0; ) {
		printf("%c", *digits++);
		i--;
		if ((i % 3 == 0) && i > 0) {
			printf(","); 
		}
	}
	return;
}
// 시간 출력
char *printTime(time_t ptime)
{
	char *time = (char *)malloc(sizeof(char) * BUFMAX);
	struct tm * t;

	t = localtime(&ptime);
	sprintf(time, "%04d-%02d-%02d %02d:%02d:%02d",
	       	(t->tm_year)+1900, (t->tm_mon)+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	return time;
}

// 큐 구현
// 큐 초기화
void QueueInit(Queue * pq)
{
	pq->front = NULL;
	pq->rear = NULL;	
}
// 큐가 비었는지 확인
int QEmpty(Queue * pq)
{
	if (pq->front == NULL)
		return TRUE;
	else
		return FALSE;
}
// 큐에 노드 추가
void Enqueue(Queue * pq, LData data)
{
	Node * newNode = (Node *)malloc(sizeof(Node));
	newNode->next = NULL;
	newNode->data = data;

	if (QEmpty(pq))
	{
		pq->front = newNode;
		pq->rear = newNode;
	}
	else {
		pq->rear->next = newNode;
		pq->rear = newNode;
	}
}
// 큐에서 노드 삭제
LData Dequeue(Queue *pq)
{
	Node * delNode;
	LData retData;

	if (QEmpty(pq))
	{
		printf("Queue Memory Error!\n");
		exit(-1);
	}

	delNode = pq->front;
	retData = delNode->data;
	pq->front = pq->front->next;

	free(delNode);
	return retData;
}

// 연결리스트 초기화
void ListInit(List * plist)
{
	plist->head = (Node *)malloc(sizeof(Node));
	plist->head->next = NULL;
	plist->cur = plist->head;
	plist->comp = NULL;
	plist->numOfData = 0;
}
// 정렬 규칙 없이 리스트 노드 추가
void FreeInsert(List * plist, LData data)
{
	Node * newNode = (Node *)malloc(sizeof(Node));
	newNode->data = data;

	newNode->next = plist->head->next;
	plist->head->next = newNode;

	(plist->numOfData)++;
}
// 정렬 규칙을 적용하여 노드 추가
void SortInsert(List * plist, LData data)
{
	Node * newNode = (Node *)malloc(sizeof(Node));
	Node * pred = plist->head;
	newNode->data = data;

	while (pred->next != NULL && plist->comp(data, pred->next->data) != -1)
	{
		pred = pred->next;
	}

	newNode->next = pred->next;
	pred->next = newNode;

	(plist->numOfData)++;
}
// 노드 추가
void ListInsert(List * plist, LData data)
{
	if (plist->comp == NULL)
		FreeInsert(plist, data);
	else
		SortInsert(plist, data);
}
// 탐색 시작할 리스트 노드의 데이터 불러오기
int ListFirst(List * plist, LData * pdata)
{
	if (plist->head->next == NULL)
		return FALSE;

	plist->before = plist->head;
	plist->cur = plist->head->next;

	*pdata = plist->cur->data;
	return TRUE;
}
// 다음에 탐색할 노드의 데이터 불러오기
int ListNext(List * plist, LData * pdata)
{
	if (plist->cur->next == NULL)
		return FALSE;

	plist->before = plist->cur;
	plist->cur = plist->cur->next;

	*pdata = plist->cur->data;
	return TRUE;
}
// 리스트 노드 삭제
LData ListRemove(List * plist)
{
	Node * rmPos  = plist->cur;
	LData rmData = rmPos->data;

	plist->before->next = plist->cur->next;
	plist->cur = plist->before;

	free(rmPos);
	(plist->numOfData)--;
	return rmData;
}
// 리스트 노드 개수 리턴
int ListCount(List * plist)
{
	return plist->numOfData;
}
// 정렬 규칙 수행용 함수 불러오기
void SetSortRule(List * plist, int (*comp)(LData d1, LData d2))
{
	plist->comp = comp;
}
