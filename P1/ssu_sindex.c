#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include "ssu_sindex.h"

char fileName[FILE_MAX] = {0,};	// FILENAME 인자로 입력한 파일명
char filePath[PATH_MAX] = {0,};	// FILENAME 인자에 해당하는 파일의 절대경로

char cwd[PATH_MAX] = {0, };		// 현재 작업 중인 디렉토리 경로
char startPath[PATH_MAX] = {0, };	// 입력한 탐색 시작할 경로

char compareFile[PATH_MAX] = {0, };			// FILENAME과 비교할 파일의 절대경로
char origBuf[BUFFER_SIZE][BUFFER_SIZE] = {0, };		// 원본 파일의 각 라인별 데이터 저장
char compBuf[BUFFER_SIZE][BUFFER_SIZE] = {0, };		// 비교본 파일의 각 라인별 데이터 저장
int LCS[BUFFER_SIZE][BUFFER_SIZE] = {0, };		// 원본과 비교본의 현재 라인 기준 일치하는 라인의 최대 개수

// 프롬프트 출력 및 명렁어 find, exit, help 실행하는 함수
void ssu_sindex(void)
{
	struct stat statbuf;
	char command[CMD_LEN] = {0, };
	char *tokens[TOKEN_CNT] = {NULL, };
	int token_cnt;
	char *ptr;
	FILE *fp;
	DIR *dirp;

	while (1) {
		fileName[0] = 0;
		filePath[0] = 0;
		cwd[0] = 0;
		startPath[0] = 0;

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
			if (token_cnt < TOKEN_CNT) {
				fprintf(stderr, "Usage: find [FILENAME] [PATH]\n");
				continue;
			}
			// FILENAME의 파일 존재 여부 확인
			realpath(tokens[1], filePath);	// 입력된 경로/파일명을 절대경로로 변환
			if (lstat(filePath, &statbuf) < 0) {
				fprintf(stderr, "%s is not exist\n", filePath);
				continue;
			}
			if (S_ISREG(statbuf.st_mode) || S_ISDIR(statbuf.st_mode)) {
				// 파일명 추출
				// FILENAME의 경로 표시 방식에 관게없이 동일한 결과 내기 위함
				getcwd(cwd, PATH_MAX);
				ptr = filePath;
				ptr += strlen(cwd)+1;	// 절대경로에서 파일명만 추출
				strcpy(fileName, ptr);
			}
			// PATH 인자의 경로에 해당하는 디렉토리 존재 여부 확인
			realpath(tokens[2], startPath);
			if (lstat(startPath, &statbuf) < 0) {
				fprintf(stderr, "%s is not exist\n", startPath);
				continue;
			}
			if (!S_ISDIR(statbuf.st_mode))
				continue;

			sindex_find(fileName, startPath);
			chdir(cwd);
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
void sindex_find(char *filename, char *pathname)
{
	struct stat statbuf;
	off_t size = 0;
	char curDir[PATH_MAX];	// 현재 디렉토리 경로
	int depth = 0;
	int index = 0;
	char subCmd[CMD_LEN];
	List list;
	Info * pinfo;

	lstat(filename, &statbuf);
	// 리스트 초기화
	ListInit(&list);

	// [FILENAME]의 파일을 [PATH]를 시작디렉토리로 하여 탐색
	if (S_ISREG(statbuf.st_mode)) {
		size = statbuf.st_size;
		searchFiles(&list, pinfo, &size, pathname, depth);
	}
	else if (S_ISDIR(statbuf.st_mode)) {
		sizeOfDir(&size, filePath, depth);
		searchFiles(&list, pinfo, &size, pathname, depth);
	}

	// 리스트 출력
	printf("%5s %4s %-10s %-6s %-5s %-4s %-4s %-16s  %-16s  %-16s  %-30s",
			"Index", "Size", "Mode", "Blocks", "Links", "UID", "GID", "Access", "Change", "Modify", "Path");

	if (ListFirst(&list, &pinfo)) {
		printf("%-5d ", index++);
		show_fileInfo(&list, pinfo);
			
		while (ListNext(&list, &pinfo)) {
			printf("%-5d ", index++);
			show_fileInfo(&list, pinfo);
		}
	}

	// 출력된 리스트 중 하나와 비교
	if (ListCount(&list) < 2) {	// 0번 인덱스 외 다른 파일 없음
		printf("(None)\n");
	}
	else {				// 0번 인덱스 외 다른 파일 존재
		int listNum;

		while (1) {
			int i = 0;
			listNum = 0;

			printf(">> ");
			fgets(subCmd, sizeof(subCmd), stdin);
        	        // 엔터키만 입력한 경우: 프롬프트 재출력
			if (strcmp(subCmd, "\n") == 0) {
				subCmd[strlen(subCmd)-1] = '\0';
				fprintf(stderr, "index input is null\n");
				continue;
			}
			// 숫자 입력인 경우
			while (isdigit(subCmd[i])) {
				listNum = 10 * listNum + subCmd[i++] - 48;
				// 검색된 인덱스 범위 초과 여부 확인
				if (listNum >= ListCount(&list)) {
					listNum = -1 ;
					break;
				}
			}

			subCmd[strlen(subCmd)-1] = '\0';
			if (listNum == -1) {		// 인덱스 범위 초과
				fprintf(stderr, "index number is not exist\n");
				continue;
			}
			else if (listNum == 0) {	// 숫자 아닌 문자 입력
				fprintf(stderr, "index is not number\n");
				continue;
			}
			else	// 범위 내 인덱스 번호 입력
				break;
		}
		// 입력한 인덱스 번호에 해당하는 파일 정보 불러오기
		if (ListFirst(&list, &pinfo)) {
			for (int i = 1; i <= listNum; i++)
				ListNext(&list, &pinfo);
			strcpy(compareFile, pinfo->path);
		}

		// 원본과 비교본 간 내용 비교
		if (S_ISREG(statbuf.st_mode)) {
			if (statbuf.st_size >= BUFFER_SIZE)	// 파일 크기가 1024바이트 이상이면 비교 없음
				printf("%s is too big!\n", fileName);
			else
				CompareFileData(filePath, pinfo->path);
		}
		else if (S_ISDIR(statbuf.st_mode)) {
			CompareDir(filePath, pinfo->path);
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

// 지정한 디렉토리 파일의 크기 구하는 함수
long int sizeOfDir(long int *psize, char *curDir, int depth) 
{
	struct dirent *entry;
	struct stat statbuf;
	DIR *dir;
	int fd;
	long int size = 0;

	if ((dir = opendir(curDir)) == NULL, chdir(curDir) < 0) {
		fprintf(stderr, "opendiri, chdir error for %s\n", curDir);
		exit(1);
	}

	while ((entry = readdir(dir)) != NULL) {
		char newDir[PATH_MAX];
		int len;

		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		lstat(entry->d_name, &statbuf);
		if (entry->d_type == DT_DIR && S_ISDIR(statbuf.st_mode)) {
			len = snprintf(newDir, sizeof(newDir)-1, "%s/%s", curDir, entry->d_name);
			newDir[len] = 0;
			size += sizeOfDir(psize, newDir, depth+1);
		}
		else if (entry->d_type == DT_REG) {
			len = snprintf(newDir, sizeof(newDir)-1, "%s/%s", curDir, entry->d_name);
			newDir[len] = 0;
			if ((fd = open(entry->d_name, O_RDONLY)) < 0) {
				fprintf(stderr, "file open error for %s\n", entry->d_name);
				continue;
			}
			size += lseek(fd, 0, SEEK_END);
			close(fd);
		}
	}

	*psize = size;
	closedir(dir);
	chdir("..");
	return *psize;
}
// find 명령어 세부작업 1: 입력한 파일명에 해당하는 파일 검색
void searchFiles(List * plist, Info * pinfo, off_t *psize, char *path, int depth)
{
	DIR *dir;
	struct dirent **nameList;
	struct stat statbuf[2];	// 0번 인덱스는 FILENAME, 1번 인덱스는 현재 탐색중인 파일
	char curDir[PATH_MAX];
	int nameCount;		 
	int idx = 0;		// nameList의 인덱스 번호
	long int dirSize;
	int dirDepth;
	
	realpath(path, curDir);
	// 파일 오픈
	if ((dir = opendir(curDir)) == NULL || chdir(curDir) < 0) {
		fprintf(stderr, "opendir error for %s\n", curDir);
	        return;
	}
	// 현재 디렉토리의 전체 파일 목록 불러오기(알파벳순 정렬)
	if ((nameCount = scandir(curDir, &nameList, NULL, alphasort) < 0))
	{
		fprintf(stderr, "%s: readdir error and scandir error\n", curDir);
	        return;
	}

	// 파일 탐색
	while (nameList[idx] = readdir(dir))
	{
		char newPath[PATH_MAX];
		int len;
		// 절대 경로로 변환
		len = snprintf(newPath, sizeof(newPath)-1, "%s/%s", curDir, nameList[idx]->d_name);
		newPath[len] = 0;
		// 정규 파일인 경우
		lstat(filePath, &statbuf[0]);
		lstat(nameList[idx]->d_name, &statbuf[1]);
		if (nameList[idx]->d_type == DT_REG) {
			// 상대경로/절대경로가 동일한 파일인 경우
                       	len = snprintf(newPath, sizeof(newPath)-1, "%s/%s", curDir, nameList[idx]->d_name);
			newPath[len] = 0;
			if ((strcmp(fileName, nameList[idx]->d_name) == 0 || strcmp(filePath, newPath) == 0)) {
				// 리스트에 추가
				if (*psize == statbuf[1].st_size) {
					pinfo = (Info *)malloc(sizeof(Info));
					init_fileInfo(pinfo, newPath, *psize);
					ListInsert(plist, pinfo);
					break;
				}
			}
		}
		else if (nameList[idx]->d_type == DT_DIR) {	// 디렉토리 파일인 경우
			// 현재 디렉토리를 경로에 추가
			if (strcmp(nameList[idx]->d_name, ".") == 0 || strcmp(nameList[idx]->d_name, "..") == 0)
                                continue;
			if (strcmp(nameList[idx]->d_name, "run") == 0 || strcmp(nameList[idx]->d_name, "proc") == 0)
				continue;
			if (strcmp(fileName, nameList[idx]->d_name) == 0 || strcmp(filePath, newPath) == 0) {
				len = snprintf(newPath, sizeof(newPath)-1, "%s/%s", curDir, nameList[idx]->d_name);
				newPath[len] = 0;
				dirSize = 0;
				dirDepth = 0;
				if (S_ISDIR(statbuf[0].st_mode) && S_ISDIR(statbuf[1].st_mode)) {
					dirSize = sizeOfDir(&dirSize, newPath, dirDepth);
					if (dirSize == *psize) {
						pinfo = (Info *)malloc(sizeof(Info));
						init_fileInfo(pinfo, newPath, dirSize);
						ListInsert(plist, pinfo);
					}
				}
			}
			else
		   		searchFiles(plist, pinfo, psize, newPath, depth+1);	// 재귀를 통한 깊이우선탐색
               	}
		idx++;

	} 
	closedir(dir);	// 파일 탐색 종료 후 디렉토리 닫음
	chdir("..");
}

// find 명령어 세부작업 2: 검색한 파일 속성 정보 출력
// 리스트에 저장할 파일 속성 정보
int init_fileInfo(Info * pinfo, char *file, long int size)
{
	struct stat buf;
	int depth = 0;

	// 현재 파일 정보 불러오기
	if (lstat(file, &buf) < 0) {
		fprintf(stderr, "%s's fileinfo error\n", file);
		return FALSE;
	}

	// 파일 크기 정보 불러오기: 디렉토리면 하위 정규 파일들의 합 저장
	if (S_ISDIR(buf.st_mode))
		pinfo->size = size;
	else
		pinfo->size = buf.st_size;

	pinfo->modes = buf.st_mode;		// 파일 종류 및 접근 권한
	pinfo->blockCnt = buf.st_blocks;	// 할당된 블록 수
	pinfo->linkCnt = buf.st_nlink;		// 하드링크 개수
	pinfo->uid = buf.st_uid;		// 사용자 ID
	pinfo->gid = buf.st_gid;		// 그룹 ID
	pinfo->access = buf.st_atime;		// 최종 접근 시간
	pinfo->change = buf.st_ctime;		// 최종 상태 변경 시간
	pinfo->modify = buf.st_mtime;		// 최종 수정 시간
	realpath(file, pinfo->path);		// 파일의 절대 경로
	return TRUE;
}

// 파일 속성 중 파일 접근권한 정보 출력
void show_permission(mode_t * pmode)
{
	char permission[11];

	// 파일 유형 정보: 디렉토리, 정규 파일, 그 외
	if (S_ISDIR(*pmode))
		permission[0] = 'd';
	else if (S_ISREG(*pmode))
		permission[0] = '-';
	else
		permission[0] = '?';

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
	printf("%-4ld ", pinfo->size);
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

// 리스트 초기화
void ListInit(List * plist)
{
	plist->head = (Node *)malloc(sizeof(Node));
	plist->head->next = NULL;
	plist->tail = plist->head;
	plist->comp = NULL;
	plist->numOfData = 0;
}
// 리스트 노드 추가
void ListInsert(List * plist, LData data)
{
	Node * newNode = (Node *)malloc(sizeof(Node));
	newNode->data = data;

	newNode->next = plist->head->next;
	plist->head->next = newNode;

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

void CompareDir(char *origDir, char *compDir) 
{
	struct dirent **origEnt;			// 기준 디렉토리의 파일 목록
	struct dirent **compEnt;			// 비교할 디렉토리의 파일 목록
	DIR *dirp1, *dirp2;
	FILE *fp1, *fp2;
	int cnt1, cnt2;					// 디렉토리 내 파일 개수
	int i, j;
	char *path1 = origDir + strlen(cwd) + 1;	// 기준 디렉토리명 추출
	char *path2 = compDir + strlen(cwd) + 1;	// 비교할 디렉토리명 추출
	char orig[BUFFER_SIZE];			// 기준 디렉토리 내 현재 파일의 절대경로
	char comp[BUFFER_SIZE];			// 비교할 디렉토리 내 현재 파일의 절대경로

	// 기준 디렉토리와 비교할 디렉토리 오픈
	dirp1 = opendir(origDir);
	dirp2 = opendir(compDir);
	if (dirp1 == NULL || dirp2 == NULL) {
		fprintf(stderr, "opendir error\n");
		return;
	}

	// 두 디렉토리의 하위 파일목록을 알파벳순으로 정렬
	cnt1 = scandir(origDir, &origEnt, NULL, alphasort);
	cnt2 = scandir(compDir, &compEnt, NULL, alphasort);
	if (cnt1 < 0 || cnt2 < 0) {
		fprintf(stderr, "scandir error\n");
		return;
	}

	// 두 디렉토리 간 파일명 비교 수행
	for (i = 0; i < cnt1; i++) {
		if (strcmp(origEnt[i]->d_name, ".") == 0 || strcmp(origEnt[i]->d_name, "..") == 0)
			continue;
		for (j = 0; j < cnt2; j++) {
			if (strcmp(compEnt[j]->d_name, ".") == 0 || strcmp(compEnt[j]->d_name, "..") == 0)
				continue;

			// 찾고자 하는 파일 이름이 동일
			if (strcmp(origEnt[i]->d_name, compEnt[j]->d_name) == 0) {
				if (origEnt[i]->d_type == DT_REG && compEnt[j]->d_type == DT_REG)	// 둘 다 정규 파일
				{
					int same = TRUE;

					// 파일 데이터 불러오기
					snprintf(orig, sizeof(orig)-1, "%s/%s", origDir, origEnt[i]->d_name);
					snprintf(comp, sizeof(comp)-1, "%s/%s", compDir, compEnt[j]->d_name);
					
					fp1 = fopen(orig, "r");
					fp2 = fopen(comp, "r");
					if (fp1 == NULL || fp2 == NULL) {
						fprintf(stderr, "subdirectory fopen error\n");
						break;
					}

					// 파일의 일치 여부 확인
					while (!feof(fp1)) {
						char c1 = fgetc(fp1);
						while (!feof(fp2)) {
							int c2 = fgetc(fp2);
							if (c1 != c2) {
								same = FALSE;
							}
						}
					}
					fclose(fp1);
					fclose(fp2);

					// 두 파일이 다를 경우 차이 출력
					if (same == FALSE) {
						printf("diff %s/%s %s/%s\n", path1, origEnt[i]->d_name, path2, compEnt[j]->d_name);
						CompareFileData(orig, comp);
					}
				}
				else if (origEnt[i]->d_type == DT_DIR && compEnt[j]->d_type == DT_DIR)	// 둘 다 디렉토리 파일	
				{
					printf("Common Subdirectories: %s/%s and %s/%s\n", path1, origEnt[i]->d_name, path2, compEnt[j]->d_name);
				}
				else {	// 두 파일의 종류가 서로 다른 경우
					printf("File %s/%s is ", path1, origEnt[i]->d_name);
					if (origEnt[i]->d_type == DT_REG) {
						printf("regular file ");
					}			
					else if (origEnt[i]->d_type == DT_DIR)
					{
						printf("directory ");
					}
					else;
					printf("while file %s/%s is a ", path2, compEnt[j]->d_name);
					if (compEnt[j]->d_type == DT_REG) {
						printf("regular file\n");
					}
					else if (compEnt[j]->d_type == DT_DIR) {
						printf("directory\n");
					}
				}
				break;
			}
			else if (j == cnt2-1) {	// 기준 디렉토리의 한 파일이 비교 디렉토리에 없는 파일
				printf("Only in %s: %s\n", path1, origEnt[i]->d_name);
			}
		}
		if (i == cnt1-1) {	// 기준 디렉토리에만 있고 비교 디렉토리에는 없는 파일
			for (j = i; j < cnt2; j++)
				printf("Only in %s: %s\n", path2, compEnt[j]->d_name);
		}
	}

	closedir(dirp1);
	closedir(dirp2);
}

// 파일 내용 비교 작업 수행용 함수
void CompareFileData(char *original, char *comparision)
{
	FILE *fp1, *fp2;
	int len1 = 0, len2 = 0;
	int i, j;
	int o1, o2;	// 원본 파일의 라인 범위 설정용
	int n1, n2;	// 비교본 파일의 라인 범위 설정용
	int maxLen;

	fp1 = fopen(original, "r");
	fp2 = fopen(comparision, "r");

	if (fp1 == NULL || fp2 == NULL) {
		fprintf(stderr, "file not found\n");
		return;
	}

	ArrayInit(origBuf);
	ArrayInit(compBuf);

	// 라인 단위로 비교할 데이터 읽기 수행, 라인 길이 구하기
	while (fgets(origBuf[len1], BUFFER_SIZE, fp1) != NULL) len1++;
	while (fgets(compBuf[len2], BUFFER_SIZE, fp2) != NULL) len2++; 

	// LCS(Longest Common Subsequence) 구하기(출력을 위한 밑작업)
	for (i = 0; i < len1; i++) {		// 배열 초기화
		for (j = 0; j < len2; j++)
			LCS[i][j] = 0;
	}

	// LCS 연산 수행
	maxLen = 0;
	for (i = 1; i <= len1; i++) {	
		for (j = 1; j <= len2; j++) {
			if (strcmp(origBuf[i-1], compBuf[j-1]) == 0) 
			{
				LCS[i][j] = LCS[i-1][j-1]+1;
				maxLen = (LCS[i][j] > maxLen) ? LCS[i][j] : maxLen;
			}
			else {
				LCS[i][j] = 0;
			}
		}
	}

	for (i = 0; i <= len1; i++) {
		for (j = 0; j <= len2; j++)
			printf("%d ", LCS[i][j]);
		printf("\n");
	}

	// 비교 결과 출력
	for (i = 0; i < len1; i++) {
		for (j = 0; j < len2; j++) {
			if (strcmp(origBuf[i], compBuf[j]) == 0)	// 현재 비교중인 라인이 서로 같은 경우
			{	
				o1 = i;
				n1 = j;
				if ((o1 == 0 || o1 == len1-1) && n1 > o1) 	// 원본 내용 시작 시점
				{
					printf("%d %d a\n", o1, n1);
				}
				else if (o1 > 0 && (n1 == 0 || n1 == len2-1)) 	// 원본 내용 종료 시점
				{
					printf("%d %d d\n", o1, n1);
				}
				else if (LCS[o1][n1] >= 1 && LCS[o1+1][n1+1] <= LCS[o1][n1])
				{
					printf("%d %d s\n", o1, n1);
					break;
				}
				else;
			}
			else {
				o1 = i;
				n1 = j;
				if (LCS[o1][n1] == maxLen && LCS[o1+1][n1+1] < LCS[o1][n1])	// 내용 변경 시작 라인
				{
					printf("%d %d c\n", o1, n1);
					break;
				}
			}
		}
	}
	
	fclose(fp1);
	fclose(fp2);
}
// 배열 초기화 함수
void ArrayInit(char (*arr)[BUFFER_SIZE]) 
{
	for (int i = 0; i < BUFFER_SIZE; i++) {
		for (int j = 0; j < BUFFER_SIZE; j++)
			arr[i][j] = 0;
	}
}
