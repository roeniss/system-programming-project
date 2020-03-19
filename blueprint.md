# 프로젝트1 설계도

## 계획

최종작성일 2020.03.19

### 전체적인 그림

쉘진입 --> (입력 --> 명령어 실행) 반복

### 구현해야 하는 것들

#### x. 공통 요구사항

- Shell 명령어는 모두 소문자로만 인식합니다.
- 인자로 사용되는 숫자는 모두 16 진수입니다. 16 진수 입력 시에는 알파벳 대소문자 모두 사용 가능해야 합니다.
- 잘못된 명령어나 필요한 인자를 지정하지 않은 경우, 또는 범위를 벗어나는인자에 대해 적절한 에러처리를 할 수 있어야 합니다.

#### a. 입력

- [x] 입력 단어 받기
- [ ] 입력 단어 trim (좌우 여백 제거)
- [ ] 입력 단어가 없거나 명령어 수행이 끝났을 경우 (+ quit가 아닐경우) 다음 입력 단어를 받을 수 있게 대기하기

#### b. 명령어

- 명령어가 여러 조각일 경우가 있으니, 첫 조각만 먼저 살펴보아야 한다.
- 각각의 명령어는 별도의 함수에서 처리하도록 한다.
- "일반적으로 history 를 친다면 그것도 명령어로 가정하므로, 빈 경우는 없습니다." --> 따라서 명령어가 '정상적이라면' history에 추가하는게 명령어 실행보다 우선되어야 함.

- [ ] h\[elp\] 구현
- [ ] d\[ir\] 구현 --> "dirent.h, sys/stat.h 를 참조합니다"
- [ ] q\[uit\] 구현 --> sub function에서 바로 종료가 되나?
- [ ] hi\[story\] 구현 --> "정의되지 않은 명령어 및 잘못된 명령어가 입력되었을 경우 history 에 추가하지않습니다" / "현재까지 사용한 명령어들을 순서대로 번호와 함께 보여준다. 가장 최근 사용한 명령어가 리스트의 하단에 오도록 한다" / "만약 history 가 비어있다면 아무것도 출력하지 않고 다시 입력 프롬프트로 돌아옵니다." / "linked list"
- [ ] du\[mp\] [start, end] 구현 --> TBD
- [ ] e\[dit\] address, value 구현 --> TBD
- [ ] f\[ill\] start, end, value 구현 --> TBD
- [ ] reset 구현 --> TBD
- [ ] opcode mnemonic 구현 --> "명령어를 입력하면 해당하는 opcode 를 출력한다." / "sicsim 프로그램을 실행시킬 때 opcode.txt 파일의 내용을 읽어 들여서 아래와 같은 모양의 Hash Table 을 만들어야 합니다. Hash Table 의 사이즈는 20" / "충돌이 생기는 mnemonic 들은 위의 그림과 같이 반드시 linked list 의 형태로 구현이 되어야 합니다."
- [ ] opcodelist 구현 --> "opcode Hash Table 의 내용을 아래와 같은 형식으로 출력합니다"