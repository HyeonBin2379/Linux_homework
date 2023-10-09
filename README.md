# Linux_homework
2022년 1학기 리눅스시스템프로그래밍 설계과제 저장용

(2023.10.7)  P2_보고서.pdf 추가
(2023.10.10) P2 ssu_sdup 파일 사용 관련 설명서 추가

## P2 ssu_sdup 개요
* 지정된 디렉토리 및 그 하위 디렉토리 내 md5 또는 sha1 해시값이 동일한, 2개 이상 존재하는 파일의 검색 및 삭제 기능 제공
* 내용상으로는 서로 동일하지만 파일명만 변경된 파일들도 중복된 파일로 검색 가능
* 해시값이 동일한 파일별로 해시값, 파일 크기, 저장 경로, 마지막 수정 및 접근 시간 정보 출력
* 리눅스 터미널에서 관리자 계정으로 접속 시 root 디렉토리 이하 모든 파일을 탐색 가능(이 경우, 크기범위 지정 필수)
* 검색된 파일 중 지정한 파일 삭제 및 삭제 옵션 지정 가능
  
## ssu_sdup 파일 실행 방법
1. P2 디렉토리 내 모든 파일 다운로드 후 리눅스 환경에서 실행
(가급적 P2 디렉토리 전체를 다운로드하기를 권장함)
2. 리눅스 터미널 실행 후 현재 디렉토리를 P2 디렉토리로 이동
3. P2 디렉토리에서 다음의 명령어 입력(실행파일명: ssu_sdup)
```
$ ./ssu_sdup
```

## Makefile 실행 설명(./ssu_sdup을 이용한 실행이 불가능할 경우)
* P2 디렉토리 내부에서 Makefile 실행
* 실행 파일이 없는 경우
```
$ make all
```
* 기존 오브젝트 파일 및 실행 파일 삭제 후 다시 컴파일 시 아래 명령어를 순서대로 입력
```
$ make clean
$ make all
```

## ssu_sdup 기능 설명 및 사용법
### [1. 도움말 출력 기능]
```
학번>> help
```
1. help 또는 유효하지 않은 명령어 입력 시 ssu_sdup의 명령어 사용법 출력
  
### [2. 디렉토리 내 중복 파일 검색 기능]
```
학번>> fmd5 확장자명 최소크기 최대크기 디렉토리명
```
1. fmd5: md5 해시값을 기준으로 동일한 파일끼리 묶어서 검색 결과 출력
2. 확장자명: * 입력 시 모든 파일 검색, *.(확장자) 입력 시 지정된 확장자를 갖는 파일만 검색
3. 최소크기 & 최대크기: 탐색할 파일의 크기 범위 지정
4. 디렉토리명: 탐색을 수행할 디렉토리

```
학번>> fsha1 확장자명 최소크기 최대크기 디렉토리명
```
1. fsha1: sha1 해시값을 기준으로 동일한 파일끼리 묶어서 검색 결과 출력
2. 확장자명: * 입력 시 모든 파일 검색, *.(확장자) 입력 시 지정된 확장자를 갖는 파일만 검색
3. 최소크기 & 최대크기: 탐색할 파일의 크기 범위 지정
4. 디렉토리명: 탐색을 수행할 디렉토리

### [3. 탐색 결과 중 삭제할 파일 선택]
#### 기본 설명
* 중복 파일 검색 직후 출력되는 프롬프트에서 삭제할 파일 및 삭제 옵션 지정 가능
* 1개의 중복 파일 리스트의 인덱스 개수는 최소 2개(한 리스트 내 파일이 1개 이하일 시 검색 결과에서 제외)

#### 삭제 파일 선택
```
>> 파일묶음번호 삭제옵션 [인덱스번호]
```
1. 파일묶음번호: 삭제하고자 하는 파일에 해당하는 리스트의 번호 선택(해시값을 기준으로 여러 개의 리스트 생성)
2. 삭제옵션
   * d: 선택한 리스트 내 지정된 인덱스에 해당하는 중복 파일 삭제
   * i: 선택한 리스트 내 각 중복 파일마다 삭제 여부 확인 메시지를 출력하여 삭제 여부 확인(일부만 구현)
   * f: 선택된 리스트 내 파일 중 가장 최근에 수정된 파일을 제외한 나머지 중복 파일 삭제 후 선택된 리스트만 제외된 검색결과 다시 출력
   * t: 선택된 리스트 내 파일 중 가장 최근에 수정된 파일을 제외한 나머지 중복 파일을 휴지통으로 이동
3. 인덱스번호: d옵션 선택 시에만 추가 입력 가능, 추가 입력 가능 한 리스트 내 해당 인덱스의 파일을 삭제

#### 삭제 파일 선택 종료
```
>> exit
```

### [4. 프로그램 실행 종료]
```
학번>> exit
```

### 추가 정보
* P2_보고서.pdf 파일 참조
