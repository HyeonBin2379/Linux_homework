#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <openssl/sha.h>
#include "ssu_functions.h"

char *ext;		// 추출한 파일 확장자명
long int minsize;
long int maxsize;
char dirPath[BUFMAX];	// [TARGET_DIRECTORY]의 절대경로
int d_option, i_option, f_option, t_option;

void get_md5Hash(char *filename, unsigned char *md);	// 파일로부터 md5 해시값 추출
void put_md5Hash(unsigned char *md);			// 해시값 출력
List setList[LISTMAX];
int setIdx = 0;

int main(int argc, char *argv[])
{
	char *ptr;
	double min, max;
	char dirName[BUFMAX];
	struct stat statbuf;
	struct timeval begin_t, end_t;
	long int size;
	unsigned char *hash;
	List list;
	Info * pinfo;

	// [FILE_EXTENSION] 인자 점검
	if (argv[1][0] == '*') {
		if (strlen(argv[1]) == 1) {	// "*"만 입력
			ext = argv[1] + 1;
		}
		else if (argv[1][1] == '.' && strlen(argv[1]) > 2) { // "*.(확장자명)"
			ext = argv[1] + 1;
		}
		else {	// 입력된 것이 "*."나 "*asdsds" 같은 경우
			fprintf(stderr, "file extension error\n");
			exit(1);
		}	
	}
	else {	// "*"나 "*.(확장자명)" 이외의 입력
		fprintf(stderr, "file extension error\n");
		exit(1);
	}

	// [MINSIZE] 인자 점검
	// 실수로 변환할 숫자가 없는 경우
	if ((min = strtod(argv[2], &ptr)) == 0) {
		if (strcmp(argv[2], "~") == 0) {	// "~" 입력
			minsize = 1;
		}
		else {	// "~" 외 다른 문자열 입력
			fprintf(stderr, "minsize error!\n");
			exit(1);
		}
	}
	else if (strcmp(ptr, "\0") != 0) {	// 실수 변환 후 문자열 존재
		if (strcmp(ptr, "KB") == 0 || strcmp(ptr, "kb") == 0)	// KB 단위 byte 변환
		{
			min *= 1024;
		}
		else if (strcmp(ptr, "MB") == 0 || strcmp(ptr, "mb") == 0)	// MB단위 byte 변환
		{
			min *= 1024*1024;
		}
		else if (strcmp(ptr, "GB") == 0 || strcmp(ptr, "gb") == 0)	// GB단위 byte 변환
		{
			min *= 1024*1024*1024;
		}
		else {
			fprintf(stderr, "minsize error\n");
			exit(1);
		};
		minsize = (long)min;
	}
	else
		minsize = (long)min;

	// [MAXSIZE] 인자 점검
	// 숫자 없이 "~"만 입력된 경우
	if ((max = strtod(argv[3], &ptr)) == 0) {
		if (strcmp(argv[3], "~") == 0) {	// "~" 입력
			maxsize = LONG_MAX;		// long형의 최대치로 설정
		}
		else {	// "~" 외 다른 문자열 입력
			fprintf(stderr, "maxsize error!\n");
			exit(1);
		}
	}
	else if (strcmp(ptr, "\0") != 0) {	// 실수 변환 후 문자열 존재
		if (strcmp(ptr, "KB") == 0 || strcmp(ptr, "kb") == 0)	// KB 단위 byte 변환
		{
			max *= 1024;
		}
		else if (strcmp(ptr, "MB") == 0 || strcmp(ptr, "mb") == 0)	// MB단위 byte 변환
		{
			max *= 1024*1024;
		}
		else if (strcmp(ptr, "GB") == 0 || strcmp(ptr, "gb") == 0)	// GB단위 byte 변환
		{
			max *= 1024*1024*1024;
		}
		else {
			fprintf(stderr, "maxsize error\n");
			exit(1);
		}
		maxsize = (long)max;
	}
	else {
		maxsize = (long)max;
	}

	// minsize가 maxsize보다 큰 경우 에러 처리
	if (minsize > maxsize) {
		fprintf(stderr, "minsize must not be bigger than maxsize\n");
		exit(1);
	}

	// [TARGET_DIRECTORY] 인자 점검
	if ((ptr = strstr(argv[4], "~")) != NULL) {	// "~(홈 디렉토리)"를 포함한 경로인 경우
		sprintf(dirName, "%s/%s%s", "/home", getenv("USER"), ptr+1);
	}
	else {
		ptr = argv[4];
		strcpy(dirName, ptr);
	}
	// 입력한 경로를 절대경로로 변환, 실제로 유효한 경로가 아니면 에러 처리
	if (realpath(dirName, dirPath) == NULL) {
		fprintf(stderr, "%s is not exist\n", dirPath);
		exit(1);
	}
	// 입력한 경로의 파일이 디렉토리인지 확인
	if (lstat(dirPath, &statbuf) < 0) {
		fprintf(stderr, "lstat error for %s\n", dirPath);
		exit(1);
	}
	if (!S_ISDIR(statbuf.st_mode)) {
		fprintf(stderr, "Path is not directory\n");
		exit(1);
	}

	// 검색 시간 측정
	gettimeofday(&begin_t, NULL);

	// 지정된 디렉토리의 모든 하위 파일들에 대한 파일 리스트 초기화
	// 중복 파일 리스트 초기화 및 정렬 규칙 설정
	ListInit(&list);
	SetSortRule(&list, sort_identical);
	for (int i = 0; i < LISTMAX; i++) {
		ListInit(&setList[i]);
		SetSortRule(&setList[i], sort_identical);
	}

	// 명령행 인자의 조건을 충족하는 전체 파일 리스트 생성
	searchFiles(dirPath, 0, &list);

	// 전체 파일에서 중복 리스트 추출: 현재 노드 기준 직후 노드의 해시값이 다르면 재귀
	setIdx = get_identicalFiles(&list, setIdx);

	// 중복 파일 리스트가 존재하지 않는 경우
	if (setIdx == 0) {
		printf("No duplicates in %s\n", dirPath);
		gettimeofday(&end_t, NULL);
		ssu_searchTime(&begin_t, &end_t);	// 전체 검색 시간 출력
	}
	else {
		for (int i = 0; i < setIdx; i++) {
			showFileList(&setList[i], i);
		}
		gettimeofday(&end_t, NULL);
		ssu_searchTime(&begin_t, &end_t);	// 전체 검색 시간 출력

		// >> [SET_INDEX] [OPTION ... ] 작업 수행
		while (1) {
			char cmd[BUFMAX];	// 명령문
			int set_num;		// 세트 번호
			int list_num;		// 한 세트 내 파일 번호
			int cnt;		// 명령문 파싱용
			char *token[TOKMAX];
			Info *pinfo;

			// 삭제를 수행할 세트 번호 및 옵션 입력
			memset(cmd, 0, BUFMAX);
			printf(">> ");
			fgets(cmd, BUFMAX, stdin);

			// 입력이 없으면 에러 처리 
			if (strcmp(cmd, "\n") == 0) {
				fprintf(stderr, "usage: [SET_INDEX] [OPTION ... ]\n");
				continue;
			}
			else if (strcmp(cmd, "exit\n") == 0) {
				printf(">> Back to Prompt\n");
				break;
			}
			else
				cmd[strlen(cmd)-1] = '\0';

			// 인자의 개수는 최소 2개 이상
			if ((cnt = parseCmd(cmd, token)) < 2) {
				fprintf(stderr, "usage: [SET_INDEX] [OPTION ... ]\n");
				continue;
			}

			// 첫번째 인자가 인덱스 범위 내 숫자인지 확인
			if ((set_num = atoi(token[0])) <= 0 || set_num > setIdx) {
				fprintf(stderr, "set index error\n");
				continue;
			}

			if (strcmp(token[1], "d") == 0) {	// d옵션 설정
				int index = 1;			// 인덱스 번호
				Info * rminfo;			// 삭제된 데이터

				// [LIST_IDX] 에 해당하는 파일 삭제
				if (cnt > 2 && (list_num = atoi(token[2])) > 0 && 
						list_num < ListCount(&setList[set_num-1])) {
					// 지정한 파일 제거
					if (ListFirst(&setList[set_num-1], &pinfo)) {
						for (int i = 1; i <= ListCount(&setList[set_num-1]); i++) {
							// 현재 인덱스 번호가 [LIST_IDX]와 동일
							if (i == list_num) {
								// 중복 파일 리스트에서 삭제
								rminfo = ListRemove(&setList[set_num-1]);
								// 실제 파일 삭제
								unlink(rminfo->path);
								printf("\"%s\" has been deleted in #%d\n\n", rminfo->path, set_num);
								break;
							}
							else 
								ListNext(&setList[set_num-1], &pinfo);
						}

						// 삭제 후 중복 리스트 내 파일이 1개만 존재
						// 중복파일리스트에서 제거
						if (ListCount(&setList[set_num-1]) < 2) {
							for (int i = set_num; i <= setIdx; i++)
								setList[i-1] = setList[i];
						}
					}

					// 삭제 후 중복 리스트 출력	
					showFileList(&setList[set_num-1], set_num);
				}
				else {	// 인자의 개수가 맞지 않거나 범위를 벗어난 경우
					fprintf(stderr, "argument or index range error\n");
				}
			}
			else if (strcmp(token[1], "t") == 0) {	// t옵션 설정
				char trash[PATHMAX];
				time_t max = 0;
				Info * temp;		// 최근 수정 파일 정보 임시 저장

				// 휴지통 절대경로 생성
				sprintf(trash, "%s/%s/%s", "/home", getenv("USER"), ".local/share/Trash/files");

				// 최근 수정된 파일 외 나머지 파일 휴지통 이동
				// 최종 수정시간 동일할	시 이전의 값 유지
				if (ListFirst(&setList[set_num-1], &pinfo)) {
					while (ListCount(&setList[set_num-1]) > 1) 
					{
						char *sub;
						char newpath[PATHMAX];

						// 파일명 추출
						sub = strrchr(pinfo->path, '/');
						// 휴지통으로 연결된 경로 생성
						strcpy(newpath, trash);
						strcat(newpath, sub);

						// 현재 파일이 최근 수정된 파일인 경우
						if (max < pinfo->mtime) {
							max = pinfo->mtime;
							temp = pinfo;
						}
						else {	// 수정 시간이 오래된 경우
							// 중복 파일 리스트에서 수정 시간이 오래된 파일 제거
							ListRemove(&setList[set_num-1]);

							// 현재 파일의 위치를 휴지통으로 변경, 기존 위치의 파일 삭제
							// 동명의 파일이 이미 존재할 시 계속 진행(해시값, 이름 모두 동일)
							if (link(pinfo->path, newpath) < 0) {
								continue;
							}
							unlink(pinfo->path);
						}

						ListNext(&setList[set_num-1], &pinfo);
					}
				}
				// 삭제 결과 출력
				printf("All files in #%d have moved to Trash except \"%s\" (%s)\n\n", set_num, temp->path, printTime(temp->mtime));

				// 삭제 후 중복 리스트 내 파일이 1개만 존재
				// 중복파일리스트에서 제거
				if (ListCount(&setList[set_num-1]) < 2) {
					for (int i = set_num; i < setIdx; i++) {
						setList[i-1] = setList[i];
					}
					setIdx--;
				}
				// 휴지통으로 파일 이동 후 출력
				for (int i = 0; i < setIdx; i++) {
					showFileList(&setList[i], i);
				}
			}
			else if (strcmp(token[1], "i") == 0) 	// i옵션 설정
			{	
				Info *rminfo;		// 삭제된 파일 정보
				int except;		// y/n 이외의 입력이 들어온 경우

				if (ListFirst(&setList[set_num-1], &pinfo)) {
					for (int i = 0; i < set_num; i++) {
						char ans[2];		// 삭제 여부 입력

						// 현재 파일 삭제 여부 결정
						memset(ans, 0, 1);
						printf("Delete \"%s\"? [y/n] ", pinfo->path);
						fgets(ans, 1, stdin);
						ans[1] = '\0';

						if (ans[0] == 'y' || ans[0] == 'Y') {		// 파일 삭제
							rminfo = ListRemove(&setList[set_num-1]);
							unlink(rminfo->path);
							except = FALSE;
							ListNext(&setList[set_num-1], &pinfo);
						}
						else if (ans[0] == 'n' || ans[0] == 'N') {	// 파일 유지
							except = FALSE;
							ListNext(&setList[set_num-1], &pinfo);
							continue;
						}
						else {	// 이외의 입력이 들어온 경우
							except = TRUE;
							break;
						}
					}
				}

				// y/n 이외의 입력이 들어온 경우
				if (except == TRUE)
					break;
				else  {
					// 삭제 후 중복 리스트 내 파일이 1개만 존재
					// 중복파일리스트에서 제거
					if (ListCount(&setList[set_num-1]) < 2) {
						for (int i = set_num; i < setIdx; i++) {
							setList[i-1] = setList[i];
						}
						setIdx--;
					}
					else {	// 현재 중복리스트에 남은 파일이 2개 이상
						showFileList(&setList[set_num-1], set_num-1);
					}
				}
			}
			else if (strcmp(token[1], "f") == 0) {	// f옵션 설정
				time_t max = 0;
				Info * temp;		// 최근 수정 파일 정보 임시 저장


				// 최종 수정시간 동일할	시 이전의 값 유지
				if (ListFirst(&setList[set_num-1], &pinfo)) {
					while (ListCount(&setList[set_num-1]) > 1) 
					{
						char *sub;
						char newpath[PATHMAX];

						// 현재 파일이 최근 수정된 파일인 경우
						if (max < pinfo->mtime) {
							max = pinfo->mtime;
							temp = pinfo;
						}
						else {	// 수정 시간이 오래된 경우
							// 중복 파일 리스트에서 수정 시간이 오래된 파일 제거
							ListRemove(&setList[set_num-1]);
							unlink(pinfo->path);
						}

						ListNext(&setList[set_num-1], &pinfo);
					}
				}
				// 삭제 결과 출력
				printf("Left file in #%d : %s (%s)\n\n", set_num, temp->path, printTime(temp->mtime));

				// 삭제 후 중복 리스트 내 파일이 1개만 존재
				// 중복파일리스트에서 제거
				if (ListCount(&setList[set_num-1]) < 2) {
					for (int i = set_num; i < setIdx; i++) {
						setList[i-1] = setList[i];
					}
					setIdx--;
				}
				// 나머지 중복 파일 리스트 출력
				for (int i = 0; i < setIdx; i++) {
					showFileList(&setList[i], i);
				}
			}
			else {
				fprintf(stderr, "option error\n");
			}
		}

		// 파일 리스트에 할당된 메모리 해제
		if (ListFirst(&list, &pinfo)) {
			ListRemove(&list);
			while (ListNext(&list, &pinfo))
				ListRemove(&list);
		}
		// 중복 파일 리스트의 각 세트별로 할당된 메모리 해제
		for (int i = 0; i < setIdx; i++) {
			if (ListFirst(&setList[i], &pinfo)) {
				pinfo = ListRemove(&setList[i]);
				free(pinfo);

				while (ListNext(&setList[i], &pinfo)) {
					pinfo = ListRemove(&setList[i]);
					free(pinfo);
				}
			}
		}
	}
	exit(0);
}

// 파일 1개의 sha1해시값을 구하는 함수
void get_sha1Hash(char *filename, unsigned char *sh)
{
	SHA_CTX c;
	int i, err = 0;
	int fd;					// 읽어올 파일에 대한 파일 디스크립터
	static unsigned char buf[BUFSIZE];	// 읽어올 파일의 데이터
	FILE *IN;

	// 인자로 지정된 파일 오픈
	IN = fopen(filename,"r");
	if (IN == NULL)
	{
		fprintf(stderr, "fopen error for %s\n", filename);
		return;
	}

	fd = fileno(IN);	// 파일 디스크립터 생성
	SHA1_Init(&c);		// sha1_ctx 구조체 초기화
	for (;;)
	{
		i = read(fd, buf, BUFSIZE);	// BUFSIZE만큼 파일 데이터 패딩
		if (i <= 0) 
			break;
		SHA1_Update(&c, buf, (unsigned long)i);	// 패딩된 데이터에 대한 해시값 부여 
	}
	SHA1_Final(&(sh[0]),&c);	// 해시 추출 결과를 sh에 저장
	fclose(IN);
}
void put_sha1Hash(unsigned char *sh)	// sha1 해시값 출력
{
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		printf("%02x", sh[i]);
	}
}
// 중복 파일 리스트 탐색: 해시값이 바뀌는 지점에서 분할
int get_identicalFiles(List *plist, int setIdx)
{
	Info * pinfo;

	if (ListFirst(plist, &pinfo)) {
		// 현재 파일 = 첫번째 파일, 다음 파일의 해시값도 동일
		if (memcmp(pinfo->hash, plist->cur->next->data->hash, HASHMAX) == 0) {
			ListInsert(&setList[setIdx], pinfo);
		}

		while (ListNext(plist, &pinfo) && plist->cur->next != NULL) {
			// 직전 파일 해시값 != 현재 파일 해시값이고  현재 해시값 == 다음 해시값(중복 리스트의 시작)
			if (memcmp(pinfo->hash, plist->before->data->hash, HASHMAX) != 0 
					&& memcmp(pinfo->hash, plist->cur->next->data->hash, HASHMAX) == 0) {
				ListInsert(&setList[setIdx], pinfo);
			}
			// 직전 해시값 == 현재 해시값 == 다음 해시값
			else if (memcmp(pinfo->hash, plist->before->data->hash, HASHMAX) == 0 
					&& memcmp(pinfo->hash, plist->cur->next->data->hash, HASHMAX) == 0) {
				ListInsert(&setList[setIdx], pinfo);
			}
			// 직전 해시값 == 현재 해시값이고 현재 해시값 != 다음 해시값(중복 리스트의 끝)
			else if (memcmp(pinfo->hash, plist->before->data->hash, HASHMAX) == 0 
					&& memcmp(pinfo->hash, plist->cur->next->data->hash, HASHMAX) != 0) {
				ListInsert(&setList[setIdx], pinfo);
				setIdx++;
			}	
			else;
		}
	}
	return setIdx;
}

// 시작 디렉토리의 모든 하위 파일 및 디렉토리 순회
// 명령행 인자의 조건을 충족하는 전체 파일 리스트 생성
void searchFiles(char *dirpath, int depth, List *plist) 
{
	int count, idx = 0;
	char *ptr;
	char curpath[PATHMAX];		// 현재 경로 저장
	unsigned char sha1[HASHMAX];	// md5 해시값 저장용
	Queue listq;			// 너비우선탐색용 큐
	Info *pinfo;			// 리스트에 저장할 파일 정보
	struct stat statbuf;
	struct dirent **namelist;	// 현재 디렉토리 내 하위 파일 리스트
	int start = 0;

	QueueInit(&listq);

	// 시작 디렉토리의 값을 큐에 저장
	pinfo = (Info *)malloc(sizeof(Info));
	strcpy(pinfo->path, dirpath);
	pinfo->depth = depth;
	Enqueue(&listq, pinfo);

	// 현재 큐가 비어 있는지 확인
	while (!QEmpty(&listq)) {
		// 큐에 저장된 가장 오래된 데이터 가져오기
		Info *curInfo = Dequeue(&listq);

		if (access(curInfo->path, F_OK) != 0) {
			// for Linux Permission denied
			if (errno == 13)
				return;
			fprintf(stderr, "access error for %s\n", dirpath);
			exit(1);
		}

		// 디렉토리 내 하위 파일 이름을 사전순 정렬
		if ((count = scandir(curInfo->path, &namelist, NULL, alphasort)) < 0) {
			fprintf(stderr, "scandir error for %s\n", curInfo->path);
			exit(1);
		} 
		else if (count == 0)
			return;

		// 파일 개수만큼 하위 파일 탐색
		for (int i = start; i < count; i++) {
			if (!strcmp(namelist[i]->d_name, ".") ||
					!strcmp(namelist[i]->d_name, ".."))
				continue;

			// 시작 디렉토리가 루트 디렉토리인지 여부에 따른 하위 경로 설정 방식
			if (strcmp(curInfo->path, "/") == 0) {
				if (strcmp(namelist[i]->d_name, "proc") == 0 ||
						strcmp(namelist[i]->d_name, "run") == 0 ||
						strcmp(namelist[i]->d_name, "sys") == 0) {
					continue;
				}
				sprintf(curpath, "%s%s", curInfo->path, namelist[i]->d_name);
			} 
			else {
				sprintf(curpath, "%s/%s", curInfo->path, namelist[i]->d_name);
			}

			// 현재 파일의 정보 가져오기
			if ((lstat(curpath, &statbuf) < 0) && (!access(curpath, F_OK))) {
				fprintf(stderr, "lstat error for %s\n", curpath);
				exit(1);
			}

			// 정규파일인 경우 파일의 크기 및 해시값을 리스트에 저장
			if (S_ISREG(statbuf.st_mode)) {
				// 파일의 크기가 지정된 범위에 해당
				if (statbuf.st_size >= minsize && statbuf.st_size <= maxsize) {
					// 확장자명을 따로 지정한 경우
					if (strcmp(ext, "") != 0) {
						// 파일의 확장자명이 일치하는 경우
						if ((ptr = strrchr(curpath, '.')) != NULL && strcmp(ext, ptr) == 0) {
							pinfo = (Info *)malloc(sizeof(Info));
							strcpy(pinfo->path, curpath);
							get_sha1Hash(curpath, sha1);
							memcpy(pinfo->hash, sha1, sizeof(unsigned char) * HASHMAX);
							pinfo->size = statbuf.st_size;
							pinfo->mtime = statbuf.st_mtime;
							pinfo->atime = statbuf.st_atime;
							pinfo->depth = curInfo->depth + 1;

							// 연결리스트에 저장
							ListInsert(plist, pinfo);
						}
					} 
					else { // 확장자명을 따로 지정하지 않은 경우
						get_sha1Hash(curpath, sha1);
						pinfo = (Info *)malloc(sizeof(Info));
						strcpy(pinfo->path, curpath);
						memcpy(pinfo->hash, sha1, sizeof(unsigned char) * HASHMAX);
						pinfo->size = statbuf.st_size;
						pinfo->mtime = statbuf.st_mtime;
						pinfo->atime = statbuf.st_atime;
						pinfo->depth = curInfo->depth + 1;
						ListInsert(plist, pinfo);
					}
				}
			} 
			else if (S_ISDIR(statbuf.st_mode)) {	// 디렉토리인 경우 큐에 추가
				pinfo = (Info *)malloc(sizeof(Info));
				strcpy(pinfo->path, curpath);
				pinfo->depth = curInfo->depth + 1;
				Enqueue(&listq, pinfo);
			} 
			else
				;
			// 동일 파일 내 하위 디렉토리가 다른 하위디렉토리의 절대경로에 연결되는 것 방지
			strcpy(curpath, curInfo->path);
		}
	}
}
// 파일리스트 출력
void showFileList(List *pset, int setnum)
{
	struct stat statbuf;
	int index = 1;
	Info * pinfo;

	if (ListFirst(pset, &pinfo)) {
		// 각 중복 파일 리스트의 세트 번호, 파일 크기, 해시값 출력
		printf("--- Identical files #%d (", setnum+1);
		putCommaToSize(pinfo->size);
		printf(" bytes - ");
		put_sha1Hash(pinfo->hash);
		printf(") ----\n");

		// 중복 파일 리스트에 저장된 파일 정보 출력
		printf("[%d] %s (mtime : %s) (atime : %s)\n", index++, pinfo->path, 
				printTime(pinfo->mtime), printTime(pinfo->atime));
		while (ListNext(pset, &pinfo)) {
			printf("[%d] %s (mtime : %s) (atime : %s)\n", index++, pinfo->path, 
					printTime(pinfo->mtime), printTime(pinfo->atime));
		}
		printf("\n");
	}
}
// 정렬 기준 설정(오름차순)
int sort_identical(LData a, LData b)
{
	// 정렬 우선순위: 파일 크기>해시값>절대경로 길이>파일명>최종 수정시간>접근시간 순
	// 크기 정렬
	if (a->size > b->size)
		return 1;		// 내림차순
	else if (a->size < b->size)
		return -1;		// 오름차순
	else {
		// 해시값 기준 정렬: 문자열처럼 사전순
		if (memcmp(a->hash, b->hash, HASHMAX) > 0)
			return 1;
		else if (memcmp(a->hash, b->hash, HASHMAX) < 0)
			return -1;
		else {
			// 절대경로 길이 기준 정렬
			if (a->depth > b->depth)
				return 1;
			else if (a->depth < b->depth)
				return -1;
			else {	// 파일명 기준 정렬
				if (strcmp(a->path, b->path) > 0)
					return 1;
				else if (strcmp(a->path, b->path) < 0)
					return -1;
			}
		}
	}
	return 0;
}
