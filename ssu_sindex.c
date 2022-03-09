#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "ssu_sindex.h"


// 프롬프트 출력 및 명렁어 find, exit, help 실행하는 함수
void ssu_sindex(void)
{
	char command[CMD_LEN] = {0, };
	char *tokens[TOKEN_CNT] = {NULL, };
	char filename[FILE_MAX];
	char pathname[PATH_MAX];
	int token_cnt;
	char *ptr;

	while (1) {
		// 프롬프트 출력 및 명령어 입력
		printf("20181224> ");
		fgets(command, sizeof(command), stdin);
		// 엔터키만 입력한 경우: 프롬프트 재출력
		if (strcmp(command, "\n") == 0) {
			command[strlen(command)-1] = '\0';
			continue;
		}
		command[strlen(command)-1] = '\0';

		// 입력한 명령어에서 문자열 파싱
		ptr = strtok(command, " \n");
		token_cnt = 0;
		while (ptr != NULL) {
			tokens[token_cnt] = ptr;
			token_cnt++;
			ptr = strtok(NULL, " \n");
		}

		if (strcmp(tokens[0], "find") == 0) {	// find 명령어 실행
			// FILENAME, PATH 입력 여부 점검
			if (token_cnt < 3) {
				fprintf(stderr, "Usage: find [FILENAME] [PATH]\n");
				continue;
			}
			// FILENAME, PATH의 존재 여부 확인
			if (access(tokens[1], F_OK) < 0) {
				fprintf(stderr, "%s is not exist\n", tokens[1]);
				continue;
			}
			if (access(tokens[2], F_OK) < 0) {
				fprintf(stderr, "%s is not exist\n", tokens[2]);
				continue;
			}
			sindex_find(tokens[1], tokens[2]);
		}
		else if (strcmp(tokens[0], "exit") == 0) {	// exit 명령어 실행
			puts("Prompt End");
			return;
		}
		else {	// help 및 기타 명령어 실행
			sindex_help();
		}
	}
}
// help 명령어 수행 함수
void sindex_help(void)
{
	printf("Usage:\n");
	printf("    > find [FILENAME] [PATH]\n");
	printf("\t>> [INDEX] [OPTION]\n");
	printf("    > help\n");
	printf("    > exit\n\n");
	printf("    [OPTION ... ]\n");
	printf("     q : report only when files differ\n");
	printf("     s : report when two files are the same\n");
	printf("     i : ignore case differences in file contents\n");
	printf("     r : recursively compare any subdirectories found\n");
	return;
}
// find 명령어 수행 함수
void sindex_find(char *filename, char *path)
{
	struct stat statbuf;
	off_t size;
	List list;
	Info * pinfo;

	lstat(filename, &statbuf);
	// 리스트 초기화 및 0번 인덱스 추가
	ListInit(&list);
	pinfo = (Info *)malloc(sizeof(Info));
	init_fileInfo(pinfo, filename);
	ListInsert(&list, pinfo);

	if (S_ISREG(statbuf.st_mode)) {
		size = statbuf.st_size;
		searchFiles(&list, pinfo, filename, path, &size);
	}

	// 리스트 출력
	printf("%5s %4s %-10s %-6s %-5s %-4s %-4s %-16s  %-16s  %-16s  %-30s",
			"Index", "Size", "Mode", "Blocks", "Links", "UID", "GID", "Access", "Change", "Modify", "Path");
	if (ListFirst(&list, &pinfo)) {
		show_fileInfo(&list, pinfo);
		while (ListNext(&list, &pinfo)) {
			show_fileInfo(&list, pinfo);
		}
	}

	// 전체 작업 종료 후 저장된 리스트 모두 삭제
	if (ListFirst(&list, &pinfo)) {
		pinfo = ListRemove(&list);
		free(pinfo);

		while (ListNext(&list, &pinfo)) {
			pinfo = ListRemove(&list);
			free(pinfo);
		}
	}
	return;
}

// find 명령어 세부작업 1: 입력한 파일명에 해당하는 파일 검색
void searchFiles(List * plist, Info * pinfo, char *filename, char *path, off_t *psize)
{
	struct stat statbuf;
	struct dirent **nameList;
	int fileCnt;
	char curPath[PATH_MAX];
	char newPath[PATH_MAX];
	int * visited;
	int i;
	
	chdir(path);
	printf("%d\n", plist->index);
	if ((fileCnt = scandir(path, &nameList, filter, alphasort)) < 0) {
		fprintf(stderr, "%s Directory Scan error\n", path);
		exit(1);
	}

	visited = (int *)calloc(fileCnt, sizeof(int));
	for (i = 0; i < fileCnt; i++) {
		visited[i] = TRUE;
		if (stat(nameList[i]->d_name, &statbuf) < 0) {
			continue;
		}
		if (S_ISDIR(statbuf.st_mode) && visited[i] == FALSE) {
			realpath(nameList[i]->d_name, newPath);
			searchFiles(plist, pinfo, filename, newPath, psize);
		}
		else {
			if (*psize == statbuf.st_size) {
				printf("%d\n", plist->index);
			}
		}
	}	

	for (i = 0; i < fileCnt; i++) {
		free(nameList[i]);
	}
	free(nameList);
	free(visited);

	chdir("..");
	return;
}
int filter(const struct dirent *entry)
{
	if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		return 0;
	else
		return 1;
}
// find 명령어 세부작업 2: 검색한 파일 속성 정보 출력
// 리스트에 저장할 파일 속성 정보
int init_fileInfo(Info * pinfo, char *filename)
{
	struct stat buf;
	char filepath[PATH_MAX];

	if (stat(filename, &buf) < 0) {
		fprintf(stderr, "%s's fileinfo error\n", filename);
		return FALSE;
	}
	pinfo->size = buf.st_size;
	pinfo->modes = buf.st_mode;
	pinfo->blockCnt = buf.st_blocks;
	pinfo->linkCnt = buf.st_nlink;
	pinfo->uid = buf.st_uid;
	pinfo->gid = buf.st_gid;
	pinfo->access = buf.st_atime;
	pinfo->change = buf.st_ctime;
	pinfo->modify = buf.st_mtime;
	realpath(filename, filepath);
	strcpy(pinfo->path, filepath);
	return TRUE;
}
// 파일 속성 중 파일 접근권한 정보 출력
void show_permission(mode_t * pmode)
{
	char permission[11];

	// 파일 유형 정보
	if (S_ISDIR(*pmode))
		permission[0] = 'd';
	else if (S_ISCHR(*pmode))
		permission[0] = 'c';
	else if (S_ISLNK(*pmode))
		permission[0] = 'l';
	else if (S_ISFIFO(*pmode))
		permission[0] = 'p';
	else if (S_ISBLK(*pmode))
		permission[0] = 'b';
	else if (S_ISSOCK(*pmode))
		permission[0] = 's';
	else
		permission[0] = '-';

	// 사용자 읽기, 쓰기, 실행 권한
	if (*pmode & S_IRUSR)
		permission[1] = 'r';
	else
		permission[1] = '-';
	if (*pmode & S_IWUSR)
		permission[2] = 'w';
	else
		permission[2] = '-';
	if (*pmode & S_IXUSR)
		permission[3] = 'x';
	else
		permission[3] = '-';
		
	// 그룹 읽기, 쓰기, 실행 권한
	if (*pmode & S_IRUSR)
		permission[4] = 'r';
	else
		permission[4] = '-';
	if (*pmode & S_IWUSR)
		permission[5] = 'w';
	else
		permission[5] = '-';
	if (*pmode & S_IXUSR)
		permission[6] = 'x';
	else
		permission[6] = '-';
		
	// 기타 읽기, 쓰기, 실행 권한
	if (*pmode & S_IRUSR)
		permission[7] = 'r';
	else
		permission[7] = '-';
	if (*pmode & S_IWUSR)
		permission[8] = 'w';
	else
		permission[8] = '-';
	if (*pmode & S_IXUSR)
		permission[9] = 'x';
	else
		permission[9] = '-';

	permission[10] = '\0';

	// 파일 접근 권한 출력
	printf("%s ", permission);
}
// 리스트에 저장할 파일 접근 권한 출력
void show_fileInfo(List * plist, Info * pinfo)
{
	printf("%-5d %-4ld ", plist->index, pinfo->size);
	show_permission(&pinfo->modes);
	printf("%-6ld %-5ld %-4d %-4d ", pinfo->blockCnt, pinfo->linkCnt, pinfo->uid, pinfo->gid);
	printTime(&pinfo->access);
	printTime(&pinfo->change);
	printTime(&pinfo->modify);
	printf("%-s\n", pinfo->path);
}
// 파일 최종 접근 시간, 변경 시간, 수정 시간 정보 출력용
void printTime(time_t * ptime)
{
	struct tm * t;
	t = localtime(ptime);
	printf("%04d-%02d-%02d %02d:%02d  ",
		       	(t->tm_year)+1900, (t->tm_mon)+1, t->tm_mday, t->tm_hour, t->tm_min);
}

// 리스트 관련 함수
// 리스트 초기화
void ListInit(List * plist)
{
	plist->head = (Node *)malloc(sizeof(Node));
	plist->tail = plist->head;
	plist->index = -1;
	plist->numOfData = 0;
}
// 리스트 노드 추가
void ListInsert(List * plist, LData data)
{
	Node * newNode = (Node *)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = NULL;

	plist->tail->next = newNode;
	plist->tail = newNode;
	(plist->index)++;
	(plist->numOfData)++;
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
