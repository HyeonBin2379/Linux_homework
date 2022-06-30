#ifndef MAIN_H_
#define MAIN_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif
#ifndef CMD_LEN
	#define CMD_LEN 64	// 내장명령문의 최대 길이
#endif
#ifndef TOKEN_CNT
	#define TOKEN_CNT 3	// 문자열 파싱 시 생성되는 토큰 개수
#endif
#ifndef FILE_MAX
	#define FILE_MAX 512
#endif
#ifndef PATH_MAX
	#define PATH_LEN 1024 	// 경로명 최대 길이
#endif
#ifndef PER_LEN
	#define PER_LEN	11
#endif

typedef struct _record
{
	off_t size;
	mode_t modes;
	blkcnt_t blockCnt;
	nlink_t linkCnt;
	uid_t uid;
	gid_t gid;
	time_t access;
	time_t change;
	time_t modify;
	char path[PATH_MAX];
} Info;

typedef Info * LData;	// 리스트에 저장할 데이터 타입

typedef struct _node
{
	LData data;
	struct _node * next;
} Node;

typedef struct _linkedlist
{
	Node * head;
	Node * tail;
	Node * cur;
	Node * before;
	int index;
	int numOfData;
} List;

// ssu_sindex 기본 기능
void ssu_sindex(void);		// ssu_sindex 프로그램 실행 함수
void sindex_find(char *filename, char *pathname);	// find 명령어 수행
void sindex_help(void);		// help 명령어 수행

// 지정한 파일 탐색 관련 함수
void searchFiles(List * plist, Info * pinfo, char * filename, char * path, off_t * psize);
int filter(const struct dirent *info);
// 검색된 파일 속성값 저장, 갱신, 출력 관련
int init_fileInfo(Info * pinfo, char *filename);	
					// 파일 속성 초기화/저장
void show_permission(mode_t * pmode);	
					// 파일 종류 및 접근 권한 정보 출력
void show_fileInfo(List * plist, Info * pinfo);		// 파일 속성 출력
void printTime(time_t * time);		// 연, 월, 일, 시, 분 출력

// find 명령어의 탐색결과 출력용 리스트 관련 함수 
void ListInit(List * plist);			// 리스트 초기화
void ListInsert(List * plist, LData data);	// 리스트 행 추가

int ListFirst(List * plist, LData * pdata);	// 리스트 탐색 시작지점
int ListNext(List * plist, LData *pdata);	// 현재 리스트 갱신

LData ListRemove(List * plist);		// 리스트 행 제거
int ListCount(List * plist);		// 리스트 전체 행 개수

#endif
