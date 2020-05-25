# System Programming (2020-1) Projects (1 - 6)

## 프로젝트1 설계도 (쉘)

### 참고사항

- 쉘진입 --> (입력 --> 명령어 실행) 반복
- 각각의 명령어는 독립적인 h, c 파일로 구성되어있다.

### 구현해야 하는 것들

#### x. 공통 요구사항

- Shell 명령어는 모두 소문자로만 인식합니다.
- 인자로 사용되는 숫자는 모두 16 진수입니다. 16 진수 입력 시에는 알파벳 대소문자 모두 사용 가능해야 합니다.
- 잘못된 명령어나 필요한 인자를 지정하지 않은 경우, 또는 범위를 벗어나는인자에 대해 적절한 에러처리를 할 수 있어야 합니다.

#### a. 입력

- [x] 입력 단어 받기
- [x] 입력 단어 left-trim (좌측 제거)
- [x] 입력 단어가 없거나 명령어 수행이 끝났을 경우 (+ quit가 아닐경우) 다음 입력 단어를 받을 수 있게 대기하기

#### b. 명령어

- 명령어가 여러 조각일 경우가 있으니, 첫 조각만 먼저 살펴보아야 한다.
- 각각의 명령어는 별도의 함수에서 처리하도록 한다.
- "일반적으로 history 를 친다면 그것도 명령어로 가정하므로, 빈 경우는 없습니다." --> 따라서 명령어가 '정상적이라면' history에 추가하는게 명령어 실행보다 우선되어야 함.

- [x] h\[elp\] 구현
- [x] d\[ir\] 구현 --> "dirent.h, sys/stat.h 를 참조합니다"
- [x] q\[uit\] 구현 --> sub function에서 바로 종료가 되나?
- [x] hi\[story\] 구현 --> "정의되지 않은 명령어 및 잘못된 명령어가 입력되었을 경우 history 에 추가하지않습니다" / "현재까지 사용한 명령어들을 순서대로 번호와 함께 보여준다. 가장 최근 사용한 명령어가 리스트의 하단에 오도록 한다" / "만약 history 가 비어있다면 아무것도 출력하지 않고 다시 입력 프롬프트로 돌아옵니다." / "linked list"
- [x] du\[mp\] [start, end] 구현
- [x] e\[dit\] address, value 구현
- [x] f\[ill\] start, end, value 구현
- [x] reset 구현
- [x] opcode mnemonic 구현 --> "명령어를 입력하면 해당하는 opcode 를 출력한다." / "sicsim 프로그램을 실행시킬 때 opcode.txt 파일의 내용을 읽어 들여서 아래와 같은 모양의 Hash Table 을 만들어야 합니다. Hash Table 의 사이즈는 20" / "충돌이 생기는 mnemonic 들은 위의 그림과 같이 반드시 linked list 의 형태로 구현이 되어야 합니다."
- [x] opcodelist 구현 --> "opcode Hash Table 의 내용을 아래와 같은 형식으로 출력합니다"

- [x] 리팩토링

### 제출 요구사항

- `sp학번_proj1` 디렉토리 내에 (1) 소스파일, (2) Makefile, (3) document.docx, (4) README, (5) opcode.txt 파일 추가
- 디렉토리를 tar로 압축해 한 파일로 만들어 제출 (no `-z` option)
- 바이너리 파일 제출 금지 (`make clean`)

- [x] source files
- [x] Makefile
- [x] document.docx
- [x] README
- [x] opcode.txt
- [x] 압축파일 생성하는 스크립트 만들기

- [x] 최종제출

---

## 프로젝트2 설계도 (어셈블러)

### 참고사항

"본 프로젝트는 완벽한 어셈블러를 구현하는 것이 아닙니다. 기본 테스트 케이스(2_5.asm)과 기본 테스트 케이스를 조금 변형한 테스트 케이스를 사용할 것이기 때문에 예외처리가 아닌 기능 구현에 중점을 두시기 바랍니다."

\*발생할 수 있는 에러들

1. 정의되지 않은 변수명이 사용되었을 경우 ✅
2. 변수명이 중복으로 사용되었을 경우 ✅
3. 정의되지 않은 instruction이 사용되었을 경우 ✅

\* Supported Assembly Directives (as-is format)

```text
name    START   abs_expr
        END     (1st_exec)
symbol  BYTE    Hex_or_Char_data    # Occupy many bites as needed
symbol  WORD    integer             # Reserves only 1 word
symbol  RESB    integer
symbol  RESW    integer
        BASE    symbol              # "Now B reg has the address of the given symbol, So use it for Specifying Target Address"
        NOBASE                      # "Now assembler must not use B reg for specifying TA"
```

+) BASE Directives는 Assembler가 (1) B 레지스터를 "사용할 수 있는지", 그리고 만약 그렇다면 (2) 그 값은 "무엇인지"를 지정한다. 그런데 지정하는 게 실제 할당하는 게 아니라.. 말하자면 "어셈블러의 별도 테이블에 별도로 기록만" 해두는 것이기 때문에, 나중에 '정말로 머신코드가 수행될 때'에는 B 레지스터에 아무것도 기록되어 있지 않다. B 레지스터가 제 구실을 할 수 있게 하려면, `LDB #symbol` 라인이 한 줄 더 필요하다.

### 구현해야 하는 것들

- [x] help
- [x] type filename
- [x] assemble filename : "소스파일에 에러 존재 시 라인/에러내용 명시", ".lst, .obj", "라인 번호는 5 배수 단위로 지정하여 출력", "각 문자열은 30자 이내", "label은 영문/숫자, 맨 앞은 숫자 불가"
- [x] symbol

### 제출 요구사항

- [x] source files (_.c, _.h)
- [x] Makefile (make clean 구현 필수)
- [x] document.docx : "이번에는 XE 소스 assemble이 주된 기능이니 만큼 이에 대한 프로그램 흐름이나 알고리즘 설명 (어떻게 구현하였는지)을 꼭 넣어주시기 바랍니다"
- [x] README
- [x] opcode.txt, 2_5.asm

이상의 파일들이 필요하고,

- [x] 압축 파일 조건1 : "sp학번\_proj2 이름의 디렉터리를 만들고, 이 디렉터리에 5에서 언급한 제출물을 넣어서 디렉터리를 tar로 압축하여 한 파일로 만든 후 과제란에 과제를 제출"
- [x] 압축 파일 조건2 : "tar 파일로 압축할 때 지난 프로젝트와 동일하게 -z 옵션을 사용하지 않습니다"
- [x] 압축 파일 조건3 : 바이너리 등 요구되지 않은 파일 반드시 제외

- [x] 최종제출

---

## 프로젝트3 설계도 (링커 & 로더)

### 참고사항

### 구현해야 하는 것들

- [x] `progaddr`
- [x] `loader`
- [x] `bp`
- [x] `run`

#### 제출 요구사항 (필요 파일)

- [x] 프로그램 소스 및 헤더파일
- [x] Makefile
- [x] 프로그램 다큐멘테이션 리포트 (\*반드시 예제 파일에 준해서 작성할 것)
- [x] 프로그램의 컴파일 방법 및 실행방법에 대한 간단한 내용을 적은 README
- [x] 테스트 파일 (proga.obj, progb.obj, progc.obj, copy.obj)

#### 제출 방식

`sp20140424_proj3` 이름의 디렉터리를 만들고, 여기에 위에서 설명한 모든 파일들을 넣은 후, 디렉터리를 tar로 (하단 명령어 참조) 압축하여 한 파일로 만들어 사이버캠퍼스 과제란에 압축한 파일을 제출하시기 바랍니다. (\*압축파일 내에 반드시 디렉터리가 포함되어 있어야 하며, 바이너리파일 및 코어파일을 제외할 것.)

```sh
tar -cvf sp20140424_proj3.tar ./*
```

- [x] 최종제출
