#ifndef FUNCTION_H
#define FUNCTION_H

#define TRUE 1
#define FALSE 0

#define ARGMAX 5		// ssu_sdup 실행 후 입력될 명령행 인자의 최대 개수
#define TOKMAX 3		// 삭제 명령 수행 시 명령행 인자의 최대 개수

#define BUFMAX 1024				// 버퍼의 크기
#define BUFSIZE 1024*16			// 해시 함수에서 한번에 읽어올 파일 데이터의 최대 길이
#define SEC_TO_MICRO 1000000			// 마이크로초 변환
#define FILEMAX 256				// 파일명의 길이
#define PATHMAX 4096				// 경로명의 길이
#define LISTMAX 10000

#ifdef HEADER_MD5_H			
	#define HASHMAX 16		// openssl/md5.h가 정의되어 있으면 해시값 길이는 16
#else
	#define HASHMAX 20		// openssl/sha.h가 정의되어 있으면 해시값 길이는 20
#endif

// 파일 정보
typedef struct _fileinfo
{
	char path[BUFMAX];
	char hash[HASHMAX];
	time_t mtime;
	time_t atime;
	size_t size;
	int depth;
} Info;
typedef Info * LData;

// 연결리스트/큐를 구성할 노드
typedef struct _node
{
	LData data;
	struct _node * next;
} Node;

// 파일에 대한 연결리스트
typedef struct _linkedlist
{
	Node * head;
	Node * cur;
	Node * before;
	int (*comp)(LData d1, LData d2);
	int numOfData;
} LinkedList;
typedef LinkedList List;

// 너비우선탐색용 큐
typedef struct lqueue
{
	Node * front;
	Node * rear;
} LQueue;
typedef LQueue Queue;

int parseCmd(char *cmd, char *argv[]);		// 명령문 파싱
void ssu_searchTime(struct timeval *begin_t, struct timeval *end_t);	// 경과 시간 측정
char *printTime(time_t ptime);		// 시간 정보 출력
void putCommaToSize(long int size);	// 천단위 구분된 파일 크기 출력
void searchFiles(char *dirname, int depth, List * plist);	// 시작 디렉토리 내 하위 파일 순회
int get_identicalFiles(List *setList, int setIdx);		// 전체 파일 리스트 중 중복 파일 리스트 추출
int sort_identical(LData d1, LData d2);				// (중복)파일 리스트 정렬 기준

// 큐 구현
void QueueInit(Queue *pq);		// 큐 초기화
int QEmpty(Queue * pq);			// 비어 있는 큐인지 판별
void Enqueue(Queue * pq, LData data);	// 큐에 노드 추가
LData Dequeue(Queue * pq);		// 큐에서 노드 제외

// 중복 파일 리스트 출력
void showFileList(List *pset, int setnum);

// 연결리스트 구현
void ListInit(List * plist);			// 리스트 초기화
void ListInsert(List * plist, LData data);	// 리스트 행 추가
int ListFirst(List * plist, LData * pdata);	// 리스트 탐색 시작지점 데이터 불러오기
int ListNext(List * plist, LData *pdata);	// 다음 리스트 이동 및 데이터 불러오기
LData ListRemove(List * plist);			// 리스트 행 삭제
int ListCount(List * plist);			// 리스트 전체 행 개수
void SetSortRule(List * plist, int (*comp)(LData d1, LData d2));	// 리스트 정렬 규칙 설정

#endif
