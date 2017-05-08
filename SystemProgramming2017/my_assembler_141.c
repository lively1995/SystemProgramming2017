/*
* 화일명 : my_assembler.c
* 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
* 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
*
*/

/*
*
* 프로그램의 헤더를 정의한다.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* my_assembler 프로그램의 의존적인 데이터들이 정의된 곳이다.
*/

#include "my_assembler_141.h"

/* -----------------------------------------------------------------------------------
* 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
* 매계 : 실행 파일, 어셈블리 파일
* 반환 : 성공 = 0, 실패 = < 0
* 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
*		   또한 중간파일을 생성하지 않는다.
* -----------------------------------------------------------------------------------
*/


int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: Failed to initialize of program.\n");
		return -1;
	}

	if (assem_pass1() < 0) {
		printf("assem_pass1: Failed to pass1 process. \n");
		return -1;
	}
	if (assem_pass2() < 0) {
		printf(" assem_pass2: Failed to pass2 process. \n");
		return -1;
	}

	my_print(litcnt);
	make_objectcode_output("output_141");

	return 0;
}
/* -----------------------------------------------------------------------------------
* 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
* 매계 : 없음4t
* 반환 : 정상종료 = 0 , 에러 발생 = -1
* 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
*		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
*		   구현하였다.
* -----------------------------------------------------------------------------------
*/

int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}


/* -----------------------------------------------------------------------------------
* 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
*        생성하는 함수이다.
* 매계 : 기계어 목록 파일
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : 기계어 목록파일 형식은 다음과 같다.
*
*	===============================================================================
*		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
*	===============================================================================
*
* -----------------------------------------------------------------------------------
*/

int init_inst_file(char *inst_file)
{
	for (int i = 0; i < MAX_INST; i++) {			//inst_table 메모리할당
		inst_table[i] = (struct inst*)malloc(sizeof(inst));
	}
	inst_index = 0;

	FILE *FILE = NULL;
	char str[128];	//라인별로 한줄씩 저장할 임시 버퍼
	char *p_token = malloc(sizeof(char) * 128);	//토큰버퍼

	FILE = fopen(inst_file, "r");
	if (FILE != NULL) {
		while (!feof(FILE)) {
			fgets(str, sizeof(str), FILE);	//txt파일로부터 한 줄 씩 읽기 

			if (str[strlen(str) - 1] == '\n')	//마지막 line을 제외한 string의 개행을 null로 수정(마지막 line의 끝엔 개행이 없으므로)
				str[strlen(str) - 1] = '\0';

			strcpy(p_token, strtok(str, "\t"));	//p_token에 탭을 기준으로 토큰별로 나눠 저장
			inst_table[inst_index]->operator = (char *)malloc(sizeof(str));	//operator(char *) 메모리 할당
			strcpy(inst_table[inst_index]->operator , p_token);	//operator에 토큰 저장
			for (int i = 0; i < 3; i++) {
				strcpy(p_token, strtok(NULL, "\t"));			//operator 이후의 토큰 분리하여 저장
				if (i == 0)
					inst_table[inst_index]->format = atoi(p_token);	//format 저장
				else if (i == 1) {
					inst_table[inst_index]->opcode = strtol(p_token, NULL, 16);	//16진수 형태로 저장
				}
				else
					inst_table[inst_index]->ops = atoi(p_token);	//op랜드 갯수 저장
			}
			inst_index++;
		}
	}
	else {
		printf("ERROR : Failed to Read file. ");
		return -1;
	}

	fclose(FILE);
	return 0;
}

/* -----------------------------------------------------------------------------------
* 설명 : 어셈블리 할 소스코드를 읽어오는 함수이다.
* 매계 : 어셈블리할 소스파일명
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/


int init_input_file(char *input_file)
{
	for (int i = 0; i < MAX_LINES; i++) {
		token_table[i] = (struct token_unit*)malloc(sizeof(struct token_unit));
	}
	line_num = 0;	//input파일을 저장할 line number

	FILE *FILE = NULL;
	char str[128];	//라인별로 한줄씩 저장할 임시 버퍼
	char *p_token = malloc(sizeof(char) * 128);	//토큰버퍼

	FILE = fopen(input_file, "r");
	if (FILE != NULL) {
		while (!feof(FILE)) {
			fgets(str, sizeof(str), FILE); //txt파일로부터 한 줄 씩 읽기 

			if (str[strlen(str) - 1] == '\n')	//마지막 line을 제외한 string의 개행을 null로 수정(마지막 line의 끝엔 개행이 없으므로)
				str[strlen(str) - 1] = '\0';

			input_data[line_num] = (char*)malloc(sizeof(str));
			strcpy(input_data[line_num], str);	//라인별 input data 저장
			line_num++;
		}
	}
	else {
		printf("ERROR : Failed to Read file. ");
		return -1;
	}

	fclose(FILE);
	return 0;
}

/* -----------------------------------------------------------------------------------
* 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
*        패스 1로 부터 호출된다.
* 매계 : 소스코드의 라인번호
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
* -----------------------------------------------------------------------------------
*/

int token_parsing(int index)
{
	/* add your code here */
	char *str = (char*)malloc(sizeof(char) * 128);	//버퍼
	char *p_token = (char*)malloc(sizeof(char) * 50);		//토큰
	char *opbuf = (char*)malloc(30);				//operand 버퍼
	char *op_token = (char*)malloc(10);				//operand 토큰
	strcpy(str, input_data[index]);					//한 라인 씩 str변수에 복사

	token_table[index]->Addr = -1;				//인스트럭션 주소 초기화

	//label 파싱
	token_table[index]->label = (char*)malloc(sizeof(char) * 7);	//label 메모리 할당
	if (str[0] == '.') {		//주석처리
		token_table[index]->label[0] = '.';	//주석일 경우 label에 저장
		token_table[index]->operator = (char*)malloc(strlen(p_token) + 1);
		return 0;
	}
	else if (str[0] != '\t') {	//첫번째 문자가 탭이 아닐 경우 (탭일 경우엔 label이 없음)
		strcpy(p_token, strtok(str, "\t"));	//label
		strcpy(token_table[index]->label, p_token);
	}

	//operator 파싱
	if (str[0] == '\t') {		//label이 없는 line은 첫번째 토큰이 operator이므로 우선처리
		token_table[index]->label[0] = '\0';
		strcpy(p_token, strtok(str, "\t"));		//operator
		token_table[index]->operator = (char*)malloc(strlen(p_token) + 1);
		strcpy(token_table[index]->operator, p_token);
	}
	else {
		strcpy(p_token, strtok(NULL, "\t"));
		token_table[index]->operator = (char*)malloc(strlen(p_token) + 1);
		strcpy(token_table[index]->operator, p_token);
	}
	if (strcmp(token_table[index]->operator, "RSUB") == 0) {	//RSUB일 경우 operand가 없고 comment만 있는 line이므로 예외처리
		strcpy(p_token, strtok(NULL, "\t"));
		token_table[index]->comment = (char*)malloc(strlen(p_token) + 1);
		strcpy(token_table[index]->comment, p_token);

		for (int i = 0; i < MAX_OPERAND; i++) {		//초기화
			token_table[index]->operand[i] = (char*)malloc(8);
			token_table[index]->operand[i][0] = '\0';
		}
		return 0;
	}
	p_token = strtok(NULL, "\t");
	if (p_token != NULL) {
		strcpy(opbuf, p_token);					//comment부터 처리하기 위해 operand 토큰을 따로 저장
	}
	else
		opbuf = NULL;

	//comment 파싱
	p_token = strtok(NULL, "\t");
	if (p_token != NULL) {			//comment가 있을 경우
		token_table[index]->comment = (char*)malloc(strlen(p_token) + 1);
		strcpy(token_table[index]->comment, p_token);
	}
	else {							//comment가 없을 경우
		token_table[index]->comment = (char*)malloc(sizeof(p_token));
		token_table[index]->comment[0] = '\0';
	}

	//operand 파싱
	for (int i = 0; i < MAX_OPERAND; i++) {		//초기화
		token_table[index]->operand[i] = (char*)malloc(8);
		token_table[index]->operand[i][0] = '\0';
	}
	if (opbuf != NULL) {		//operand가 존재
		if (strchr(opbuf, ',') == NULL) {	//단일 operand
			strcpy(token_table[index]->operand[0], opbuf);
		}
		else {
			strcpy(op_token, strtok(opbuf, ","));		//1번째 operand
			strcpy(token_table[index]->operand[0], op_token);

			strcpy(op_token, strtok(NULL, ","));		//2번째 operand
			strcpy(token_table[index]->operand[1], op_token);

			op_token = strtok(NULL, ",");				//3번째 operand가 있는지 검사
			if (op_token != NULL) {
				strcpy(token_table[index]->operand[2], op_token);
			}
		}
	}

	return 0;
}
/* -----------------------------------------------------------------------------------
* 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 문자열
* 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/

int search_opcode(char *str)
{
	for (int i = 0; i < inst_index; i++) {
		if (str[0] == '+') {									//+가 붙는 extended일 경우
			if (strcmp(inst_table[i]->operator, &str[1]) == 0)	//+ 다음 번지주소를 매개변수로 사용
				return i;
		}
		else if (strcmp(inst_table[i]->operator, str) == 0)		//일반 operator
			return i;
	}
	return -1;
}

/* -----------------------------------------------------------------------------------
* 설명 : 입력 문자열이 SYMTAB에 들어있는지 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 label
* 반환 : 정상종료 = 해당 symbol의 인덱스, 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/

int search_symbol(char *str)
{
	for (int i = 0; i < token_line; i++) {
		if (strcmp(sym_table[i].symbol, str) == 0)
			return i;
	}
	return -1;
}

/* -----------------------------------------------------------------------------------
* 설명 : 입력 문자열 SYMTAB에 들어있는지,현 섹션과 일치하는지 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 label, 현섹션번호
* 반환 : 정상종료 = 해당 symbol의 인덱스, 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/

int search_symbol2(char *str, int sect)
{
	for (int i = 0; i < token_line; i++) {
		if (strcmp(sym_table[i].symbol, str) == 0) {
			if (sym_table[i].section == sect)
				return i;
		}
	}
	return -1;
}
/* -----------------------------------------------------------------------------------
* 설명 : 입력 문자열이 LITTAB에 들어있는지 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 literal
* 반환 : 정상종료 = 해당 literal의 인덱스, 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/

int search_literal(char *str)
{
	for (int i = 0; i < token_line; i++) {
		if (strcmp(lit_table[i].literal, str) == 0)
			return i;
	}
	return -1;
}

/* -----------------------------------------------------------------------------------
* 설명 : 입력 문자열이 inst_table에 들어있는지 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 토큰테이블의 operator
* 반환 : 정상종료 = 해당 기계어 코드의 인덱스, 에러 < 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/

int search_inst(char *str)
{
	for (int i = 0; i < inst_index; i++) {
		if (strcmp(inst_table[i]->operator, str) == 0)
			return i;
	}
	return -1;
}
/* -----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	token_line = 0;
	litcnt = 0;
	int startaddr = 0, opnum = 0, symnum, j = 0, k = 0, csectnum = 0;
	char litbuf[5];
	char *str = (char*)malloc(sizeof(char) * 128);	//버퍼
	char *p_token = (char*)malloc(sizeof(char) * 50);		//토큰

	for (int i = 0; i < line_num; i++) {
		if (token_parsing(i) < 0) {									//토큰 테이블 작성
			return -1;
		}
		token_line++;
	}

	for (int i = 0; i < token_line; i++) {							//리터럴테이블 초기화
		lit_table[i].addr = -1;
		lit_table[i].literal[0] = '\0';
	}

	int i = 0;

	if (strcmp(token_table[i]->operator, "START") == 0) {		//프로그램 시작 시
		locctr = atoi(token_table[i]->operand[0]); //LOCCTR start 주소로 초기화
		startaddr = locctr;
		token_table[i]->Addr = locctr;
		i++;
		sectaddr[0][0] = startaddr;
	}
	else {
		locctr = 0;
	}
	while (strcmp(token_table[i]->operator, "END") != 0) {					//프로그램이 끝날 때 까지 루프
		if (token_table[i]->label[0] != '.') {	//주석이 아닐 경우 
			if (strcmp(token_table[i]->operator, "CSECT") == 0) {
				sectaddr[csectnum][1] = locctr;
				locctr = 0;			//새 섹션 시작 시 LOCCTR 0으로 초기화
				sym_table[search_symbol(token_table[i]->label)].section = ++csectnum;
				sym_table[search_symbol(token_table[i]->label)].addr = 0;
				token_table[i]->Addr = locctr;	//인스트럭션 주소 초기화
				sectaddr[csectnum][0] = locctr;
			}
			if (token_table[i]->label[0] != '\0') {				//label이 존재
				symnum = search_symbol(token_table[i]->label);
				if (symnum < 0) {		//SYMTAB에 존재하지 않을 경우				
					strcpy(sym_table[j].symbol, token_table[i]->label);			//심볼테이블 라벨 저장
					sym_table[j].addr = locctr;									//심볼테이블 주소 저장
					sym_table[j].section = csectnum;							//심볼테이블 섹션번호 저장
					token_table[i]->Addr = locctr;					//instruction 주소 저장
					j++;
				}
				else {             //SYMTAB에 label이 존재할 경우
					if (sym_table[symnum].section == csectnum) {
						sym_table[symnum].addr = locctr;			//심볼테이블 주소 저장				//RDREC 주소 저장된다!
					}
					else {
						strcpy(sym_table[j].symbol, token_table[i]->label);
						sym_table[j].addr = locctr;
						sym_table[j].section = csectnum;
						j++;
					}

					token_table[i]->Addr = locctr;	//instruction 주소 저장		
				}
			}
			else {		//label이 없을 경우
				if (strcmp(token_table[i]->operator, "EXTDEF") == 0 || strcmp(token_table[i]->operator, "EXTREF") == 0) {	//외부참조 EXTDEF일 경우
					symnum = 0;
					for (int idx = 0; idx < MAX_OPERAND; idx++) {
						if (token_table[i]->operand[idx][0] != '\0') {	//operand 존재할 경우
							symnum = search_symbol(token_table[i]->operand[idx]);
							if (symnum < 0) {		//SYMTAB에 존재하지 않을 경우				
								strcpy(sym_table[j].symbol, token_table[i]->operand[idx]);			//심볼테이블 라벨 저장								
								sym_table[j].section = csectnum;		//섹션번호 저장
								j++;
							}
							else {		//SYMTAB에 존재할 경우
								if (sym_table[symnum].section == csectnum)	//현재 파싱하는 섹션과 심볼테이블에 EXTDEF에 의해 저장된 operand 섹션이 일치할 경우
									sym_table[symnum].addr = locctr;	//해당 심볼 addr에 주소 저장

							}
						}
					}
				}
				else if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG 발견 시 LITTAB참조
					while (lit_table[litcnt].literal[0] != '\0') {	//LTORG 발견까지 LITTAB에 등록된 리터럴 조회 
						lit_table[litcnt].addr = locctr;			//해당 literal주소 등록
						locctr += lit_table[litcnt].size;	//literal의 값
						litcnt++;
					}
				}
				else
					token_table[i]->Addr = locctr;
			}

			opnum = search_opcode(token_table[i]->operator);
			if (opnum >= 0) {		//명령어가 맞으면
				if (inst_table[opnum]->format == 34) {		//
					if (token_table[i]->operator[0] == '+') {		//+가 붙은 4형식일 경우
						locctr += 4;
					}
					else
						locctr += 3;
				}
				else if (inst_table[opnum]->format == 2) {	//2형식이면
					locctr += 2;
				}
				else	//1형식이면
					locctr += 1;
			}
			else if (strcmp(token_table[i]->operator, "WORD") == 0) {
				locctr += 3;
			}
			else if (strcmp(token_table[i]->operator, "RESW") == 0) {
				locctr += 3 * (atoi(token_table[i]->operand[0]));
			}
			else if (strcmp(token_table[i]->operator, "RESB") == 0) {
				locctr += atoi(token_table[i]->operand[0]);
			}
			else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
				locctr += 1;
			}
			else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
				locctr += 1;
			}
			else if (strcmp(token_table[i]->operator, "EQU") == 0) {
				if (token_table[i]->operand[0][0] != '*') {			//*(현재 locctr 값)이 아니라면
					char op_token[20];			//임시 토큰
					char *tmp = (char*)malloc(10);
					int opaddr1, opaddr2;
					strcpy(op_token, token_table[i]->operand[0]);	//값을 계산해야하는 operand를 버퍼에 복사
					if (strchr(op_token, '+')) {                    //+식일 경우
						tmp = strtok(op_token, "+");					//연산기호를 기준으로 토큰 분리
						opaddr1 = sym_table[search_symbol(tmp)].addr;	//첫번째 operand의 주소를 찾아 opaddr1에 대입(SYMTAB활용)
						tmp = strtok(NULL, "+");						//위와 동일
						opaddr2 = sym_table[search_symbol(tmp)].addr;
						token_table[i]->Addr = opaddr1 - opaddr2;		//연산 후의 주소값을 해당 토큰 인스트럭션 주소에 대입
						sym_table[search_symbol(tmp)].addr = token_table[i]->Addr;	//해당 심볼의 주소에 대입
					}
					else if (strchr(op_token, '-')) {               //-식일 경우
						tmp = strtok(op_token, "-");					//연산기호를 기준으로 토큰 분리
						opaddr1 = sym_table[search_symbol(tmp)].addr;	//첫번째 operand의 주소를 찾아 opaddr1에 대입(SYMTAB활용)
						tmp = strtok(NULL, "-");						//위와 동일
						opaddr2 = sym_table[search_symbol(tmp)].addr;
						token_table[i]->Addr = opaddr1 - opaddr2;		//연산 후의 주소값을 해당 토큰 인스트럭션 주소에 대입
						sym_table[search_symbol(token_table[i]->label)].addr = token_table[i]->Addr; //해당 심볼의 주소에 대입
					}
				}
			}
			int m = 3, n = 0;
			if (token_table[i]->operand[0][0] == '=') {
				int litnum = search_literal(token_table[i]->operand[0]);
				if (litnum < 0) {	//LITTAB에 없을 경우 (중복 방지)
					strcpy(lit_table[k].literal, token_table[i]->operand[0]);	//LITTAB에 '='로 시작하는 literal 등록

					while (lit_table[k].literal[m] != '\'') {				//=C또는 =X' 이후(배열[3]부터) '이 나올때까지 
						litbuf[n++] = lit_table[k].literal[m++];		//한 캐릭터씩 litbuf에 저장
					}
					litbuf[n] = '\0';										//끝에 null문자 추가

					if (lit_table[k].literal[1] == 'C') {					//=C' ', 캐릭터일 경우
						int index = 0;
						char *C_buf[10];
						while (litbuf[index] != '\0') {
							C_buf[index] = (char*)malloc(6);
							sprintf(C_buf[index], "%X", litbuf[index]);     //한글자씩16진수로 출력하여
							strcat(lit_table[k].litdata, C_buf[index]);     //litdata에 저장
							index++;
						}
					}
					else
						strcpy(lit_table[k].litdata, litbuf);					//literal Data를 추출하여 테이블에 저장

					if (strchr(lit_table[k].literal, 'C'))
						lit_table[k].size = (strlen(lit_table[k].litdata) / 2);
					else
						lit_table[k].size = (strlen(lit_table[k].litdata) - 1); //=X일 경우 숫자 2캐릭터당 1바이트
					k++;
				}

			}
		}
		i++;
	}

	int formatnum;
	if (strcmp(token_table[token_line - 1]->operator, "END") == 0) { //프로그램이 끝났을 경우 LITTAB에 리터럴 주소 등록
		int tmp_locctr = 0;		//임시 로케이션 카운터
		for (j = litcnt; j < token_line; j++) {
			if (lit_table[j].literal[0] != '\0') {
				tmp_locctr += token_table[token_line - 2]->Addr;	//END 이전 명령어 주소값
				formatnum = inst_table[token_line - 2]->format;		//ENd 이전 명령어의 형식 크기 확인
				if (formatnum == 34) {		//3,4형식
					tmp_locctr += 3;
				}
				else if (formatnum == 2) {  //2형식
					tmp_locctr += 2;
				}
				lit_table[j].addr = tmp_locctr;
				sectaddr[csectnum][1] = lit_table[j].addr + lit_table[j].size;
			}
		}
	}

	return 0;
}

/* -----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/

static int assem_pass2(void)
{
	int i = 0, instnum = -1, sectnum = 0;

	while (i < token_line) {
		int extend = 0, notpc = 0, signcheck = 0;
		token_table[i]->nixbpe = 0;
		token_table[i]->obcode = 0;
		if (token_table[i]->label[0] == '.') {//주석일 경우
			i++;
			continue;
		}
		if (strcmp(token_table[i]->operator, "CSECT") == 0) {	//다음 섹터참조
			sectnum++;
			i++;
			continue;
		}

		if (token_table[i]->operator[0] == '+') {		//extended 연산일 경우
			instnum = search_inst(&(token_table[i]->operator[1])); //첫번째문자+을 뺀 해당 operator가 기계어 명령어인지 확인
			extend = 1;
			token_table[i]->nixbpe += 1;
			notpc = 1;
		}
		else
			instnum = search_inst(token_table[i]->operator);	//해당 operator가 기계어 명령어인지 확인


		if (instnum >= 0) {	//명령어가 instruction set에 있는 명령어인지 확인
			if (inst_table[instnum]->format == 2) {	//2형식일 경우
				token_table[i]->obcode = inst_table[instnum]->opcode << 8;	//opcode를 shift한 뒤 저장
				if (inst_table[instnum]->ops == 1) {	//operand가 1개일 경우 
					for (int j = 0; j < 9; j++) {
						if (strcmp(token_table[i]->operand[0], reg_table[j]) == 0) {
							token_table[i]->obcode += j << 4;	//해당 레지스터 번호 저장
							break;
						}
					}
				}
				else {  //레지스터를 2개 이상 사용할 경우
					for (int j = 0; j < 9; j++) {
						if (strcmp(token_table[i]->operand[0], reg_table[j]) == 0) {
							token_table[i]->obcode += j << 4;	//첫번째 레지스터 번호 저장
							break;
						}
					}
					for (int j = 0; j < 9; j++) {
						if (strcmp(token_table[i]->operand[1], reg_table[j]) == 0) {
							token_table[i]->obcode += j;	//두번째 레지스터 번호 저장
							break;
						}
					}
				}
				i++;
				continue;
			}
			else {//3,4형식일 경우
				int litnum = -1, litcheck = 0;
				if (token_table[i]->operand[0][0] == '#') {	//immediate 연산일 경우
					token_table[i]->nixbpe += 1 << 4;	//ni=01
					if ('0' <= token_table[i]->operand[0][1] && '9' >= token_table[i]->operand[0][1]) {		//#뒤가 0~9인 숫자일 경우	
						token_table[i]->obcode += inst_table[instnum]->opcode << 16 | token_table[i]->nixbpe << 12 | atoi(&token_table[i]->operand[0][1]);
						i++;
						continue;
					}
					else {
						signcheck = 1;
					}
				}
				else if (token_table[i]->operand[0][0] == '@') {	//indirect 연산일 경우
					token_table[i]->nixbpe += 1 << 5;	//ni=10
					signcheck = 1;
				}
				else		//SIC/XE 연산
					token_table[i]->nixbpe += 3 << 4;	//ni=11

				for (int j = 0; j <= inst_table[instnum]->ops; j++) {	//X레지스터를 사용할 경우
					if (strcmp(token_table[i]->operand[j], "X") == 0) {
						token_table[i]->nixbpe += 1 << 3;	//indexing 연산
						break;
					}
				}

				if (token_table[i]->operand[0][0] == '\0') {
					;	//operand가 없을 경우 p=0;
				}
				else if (!extend)		//extend가 아니면 PC
					token_table[i]->nixbpe += 1 << 1;	//PC Relative, p=1;


				int symnum = -1;
				if (signcheck) {
					symnum = search_symbol(&token_table[i]->operand[0][1]);	//#, @ 기호가 붙을 경우
				}
				else if (token_table[i]->operand[0][0] == '=') {	//Literal 연산일 경우
					litnum = search_literal(token_table[i]->operand[0]);
					litcheck = 1;
				}
				else {
					symnum = search_symbol(token_table[i]->operand[0]);
				}

				int reladdr = 0;
				if (symnum >= 0) {	//operatnd가 심볼테이블에 존재할 경우
					if (sectnum == sym_table[symnum].section) {		//현재 섹션과 심볼이 정의된 섹션이 같을 경우
						reladdr = sym_table[symnum].addr - token_table[i]->Addr - 3;		//심볼 주소 - PC 주소
						if (reladdr < 0)
							reladdr = 0x0FFF & reladdr;
					}
					else
					{
						int tmp = -1;
						tmp = search_symbol2(token_table[i]->operand[0], sectnum);
						if (tmp >= 0) {
							reladdr = sym_table[tmp].addr - token_table[i]->Addr - 3;		//심볼 주소 - PC 주소
							if (reladdr < 0)
								reladdr = 0x0FFF & reladdr;
						}
					}

				}
				else if (litcheck) {		//literal 연산일 경우
					reladdr = lit_table[litnum].addr - token_table[i]->Addr - 3;
				}


				if (extend == 1)	//4형식일 경우
					token_table[i]->obcode += inst_table[instnum]->opcode << 24 | token_table[i]->nixbpe << 20 | reladdr;
				else
					token_table[i]->obcode += inst_table[instnum]->opcode << 16 | token_table[i]->nixbpe << 12 | reladdr;

			}
		}
		else if (strcmp(token_table[i]->operator, "WORD") == 0) {
			if ('0' <= token_table[i]->operand[0][0] && '9' >= token_table[i]->operand[0][0]) {		//0~9인 숫자일 경우
				token_table[i]->obcode = atoi(token_table[i]->operand[0]);
			}
			else if (strchr(token_table[i]->operand[0], '+')) {                    //+식일 경우
				char op_token[10];
				strcpy(op_token[10], token_table[i]->operand[0]);
				int tmp = strtok(op_token, "+");
				int symindex = search_symbol(tmp);//연산기호를 기준으로 토큰 분리
				if (sectnum == sym_table[symindex].section) {				//첫번째 심볼이 해당 섹션에 정의된 심볼일 경우
					int opaddr1 = sym_table[symindex].addr;	//첫번째 operand의 주소를 찾아 opaddr1에 대입(SYMTAB활용)
					tmp = strtok(NULL, "+");						//위와 동일
					symindex = search_symbol(tmp);
					if (sectnum == sym_table[symindex].section) {
						int opaddr2 = sym_table[symindex].addr;
						token_table[i]->obcode = opaddr1 + opaddr2;		//연산 후의 주소값을 해당 토큰 인스트럭션 주소에 대입
					}
				}
			}

		}
		else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
			char litbuf[8];
			int j = 2, k = 0;
			while (token_table[i]->operand[0][j] != '\'') {				//=C또는 =X' 이후(배열[3]부터) '이 나올때까지 
				litbuf[k++] = token_table[i]->operand[0][j++];		//한 캐릭터씩 litbuf에 저장
			}
			litbuf[k] = '\0';										//끝에 null문자 추가

			token_table[i]->obcode = strtol(litbuf, NULL, 16);		//X'' 일경우
		}
		else {	//기계어 명령어가 아닐 경우
			i++;
			continue;
		}
		i++;
	}

	return 0;
}

/* -----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/

void make_objectcode_output(char *file_name)
{
	FILE *FILE = NULL;
	char *object_code[50];	//obcode
	char *ref_table[3];
	int i = 0, j = 0;
	int sectnum = 0;	//섹션번호
	int linenum = 0; //object_code의 세로 line, 가로 index
	int length;
	int H_check = 0;
	int T_cnt = 0, T_start = 0;
	char T_code[100];
	char *M_code[30];
	int M_codeindex = 0, M_find = 0, M_len = 0, M_addr = 0, ref_index = -1, ref_index2 = -1;
	for (int m = 0; m < 10; m++) {
		M_code[m] = (char*)malloc(30);
		M_code[m][0] = '\0';
	}
	for (; i < token_line; i++) {
		if (token_table[i]->label[0] == '.') {		//주석
			continue;
		}
		if (strcmp(token_table[i]->operator, "CSECT") == 0) {
			if (T_code[0] != '\0') {  //섹션이 넘어가는 경우 이전에 썻던 T레코드를 작성
				sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,시작주소,길이,코드 작성
				T_start = 0;
				T_cnt = 0;
				T_code[0] = NULL;
				linenum++;
			}

			int m = 0;
			while (M_code[m][0] != '\0') {    //M레코드의 코드 작성
				object_code[linenum] = (char*)malloc(70);
				object_code[linenum][0] = NULL;
				strcpy(object_code[linenum++], M_code[m++]);
			}

			for (int m = 0; m < 10; m++) {  //M_코드 초기화
				M_code[m] = (char*)malloc(30);
				M_code[m][0] = '\0';
			}
			M_codeindex = 0;  //M_code 인덱스 초기화

			object_code[linenum] = (char*)malloc(70);
			object_code[linenum][0] = NULL;
			if (sectnum == 0) {
				sprintf(object_code[linenum++], "E%06X", sectaddr[sectnum][0]); //첫 섹션은 돌아갈 시작주소
			}
			else
				strcpy(object_code[linenum++], "E");  //아니라면 돌아갈 시작주소를 모르기때문에 비움.
			H_check = 0;
			sectnum++;
			continue;
		}
		length = 0;

		//H레코드
		if (H_check == 0) {
			object_code[linenum] = (char*)malloc(70);
			length = sprintf(object_code[linenum], "H");
			if (sectnum == 0)
				length += sprintf(object_code[linenum] + length, "%-6s", token_table[i]->label); //프로그램 이름
			else
				length += sprintf(object_code[linenum] + length, "%-6s", token_table[i - 4]->label); //섹션 이름. 주석크기+EXTREF=3+1=4 총 4만큼떨어져있어 i-5
			length += sprintf(object_code[linenum] + length, "%06X", sectaddr[sectnum][0]);//시작주소
			length += sprintf(object_code[linenum] + length, "%06X", sectaddr[sectnum][1] - sectaddr[sectnum][0]); //길이 = 마지막주소-시작주소
			length = 0;
			linenum++;
			H_check = 1;
		}

		//D레코드
		if (strcmp(token_table[i]->operator, "EXTDEF") == 0)
		{
			object_code[linenum] = (char*)malloc(70);
			length = sprintf(object_code[linenum], "D");
			for (int j = 0; j < MAX_OPERAND; j++) {
				if (token_table[i]->operand[j][0] != '\0') {
					length += sprintf(object_code[linenum] + length, "%-6s", token_table[i]->operand[j]);
					length += sprintf(object_code[linenum] + length, "%06X", sym_table[search_symbol(token_table[i]->operand[j])].addr);
				}
			}
			length = 0;
			linenum++;
			continue;
		}
		else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {  //R레코드
			object_code[linenum] = (char*)malloc(70);
			length = sprintf(object_code[linenum], "R");
			for (int j = 0; j < 3; j++) {
				ref_table[j] = (char*)malloc(10);
				ref_table[j][0] = '\0';
			}
			for (int j = 0; j < MAX_OPERAND; j++) {
				if (token_table[i]->operand[j][0] != '\0') {
					strcpy(ref_table[j], token_table[i]->operand[j]);
					length += sprintf(object_code[linenum] + length, "%-6s", ref_table[j]);
				}
			}
			length = 0;
			linenum++;
			continue;
		}

		//T레코드
		object_code[linenum] = (char*)malloc(70);
		int isinst = search_inst(token_table[i]->operator);
		int isob = isobcode(token_table[i]->operator);
		int ob_len = obcode_len(isinst, i);  //해당 obcode의 길이 파악
		char ref_tmp[10], ref_tmp2[10];

		if (isob == 1) { //obcode가 있는 경우	
			if (T_cnt + ob_len >= 60) {		//60칼럼 이상일 경우
				sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,시작주소,길이,코드 작성
				T_start = 0;
				T_cnt = 0;
				T_code[0] = NULL;
				linenum++;
			}
			if (T_cnt == 0) { //T레코드 첫 시작주소 판별
				T_start = token_table[i]->Addr;
			}

			T_cnt += sprintf(T_code + T_cnt, "%0*X", ob_len, token_table[i]->obcode);

			char op_token[20];			//임시 토큰
			strcpy(op_token, token_table[i]->operand[0]);	//값을 계산해야하는 operand를 버퍼에 복사
			char* tmp = (char*)malloc(10);
			for (int r = 0; r < 3; r++) {
				if (strcmp(op_token, ref_table[r]) == 0) {  //레퍼런스 테이블에 있는지 확인
					if (ref_table[r][0] != '\0') {
						M_len = token_table[i]->Addr + 1;
						M_find = 1;
						ref_index = r;
						break;
					}
				}
				else if (strchr(token_table[i]->operand[0], '-')) {  //ref-ref일 경우
					tmp = strtok(op_token, "-");					//연산기호를 기준으로 토큰 분리
					for (int re = 0; re < 3; re++) {
						if (strcmp(tmp, ref_table[re]) == 0) {  //레퍼런스 테이블에 있는지 확인
							if (ref_table[r][0] != '\0') {
								M_len = token_table[i]->Addr;
								M_find = 2;
								strcpy(ref_tmp, ref_table[re]);
								ref_index = re;
								break;
							}
						}
					}
					tmp = strtok(NULL, "-");						//두번째 ref
					for (int re = 0; re < 3; re++) {
						if (strcmp(tmp, ref_table[re]) == 0) {  //레퍼런스 테이블에 있는지 확인
							if (ref_table[r][0] != '\0') {
								M_len = token_table[i]->Addr;
								strcpy(ref_tmp2, ref_table[re]);
								ref_index2 = re;
								break;
							}
						}
					}
					break;
				}

			}

		}
		else if (strcmp(token_table[i]->operator, "RESW") == 0 || strcmp(token_table[i]->operator, "RESW") == 0) { //object_code가 연속적이지 않을 경우
			if (T_code[0] != NULL) {
				sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,시작주소,길이,코드 작성
				T_start = 0;
				T_cnt = 0;
				T_code[0] = NULL;
				linenum++;//obcode가 연속적이지 않은 경우
				continue;
			}
		}
		else if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG일경우 추가
			for (; j < litcnt; j++) {
				if (lit_table[j].addr != -1) {
					if (T_cnt + lit_table[j].size >= 60) {		//60칼럼 이상일 경우
						sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,시작주소,길이,코드 작성
						T_start = 0;
						T_cnt = 0;
						T_code[0] = NULL;
						linenum++;
					}
					if (T_cnt == 0) { //T레코드 첫 시작주소 판별
						T_start = lit_table[j].addr;
					}

					T_cnt += sprintf(T_code + T_cnt, "%s", lit_table[j].litdata);
				}
			}
			sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,시작주소,길이,코드 작성
			T_start = 0;
			T_cnt = 0;
			T_code[0] = NULL;
			linenum++;
			continue;
		}


		//M레코드
		object_code[linenum] = (char*)malloc(70);
		if (M_find == 1) {  //ref를 4형식으로 참조했을 경우
			M_code[M_codeindex] = (char*)malloc(30);
			sprintf(M_code[M_codeindex], "M%06X05+%s", M_len, ref_table[ref_index]); //위치,n번째부터참조인지,+,레퍼런스
			M_codeindex++;
			M_find = 0;
		}
		else if (M_find == 2) { //연산이 있는 m레코드일 경우 ref-ref
			for (int re = 0; re < 2; re++) {
				M_code[M_codeindex] = (char*)malloc(30);
				if (re == 0)
					sprintf(M_code[M_codeindex], "M%06X06+%s", M_len, ref_tmp); //위치,n번째부터참조인지,+,레퍼런스
				else
					sprintf(M_code[M_codeindex], "M%06X06-%s", M_len, ref_tmp2); //위치,n번째부터참조인지,-,레퍼런스
				M_codeindex++;
			}
			M_find = 0;
		}


		//END, 프로그램이 끝났을 때
		if (strcmp(token_table[i]->operator, "END") == 0) {
			for (j = litcnt; j < token_line; j++) {
				if (lit_table[j].addr != -1) {
					if (T_cnt + lit_table[j].size >= 60) {		//60칼럼 이상일 경우
						sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,시작주소,길이,코드 작성
						T_start = 0;
						T_cnt = 0;
						T_code[0] = NULL;
						linenum++;
					}
					if (T_cnt == 0) { //T레코드 첫 시작주소 판별
						T_start = lit_table[j].addr;
					}

					T_cnt += sprintf(T_code + T_cnt, "%s", lit_table[j].litdata);
					break;
				}
			}
			//마지막T레코드 작성
			sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code);
			linenum++;
			//마지막M레코드 작성
			int m = 0;
			while (M_code[m][0] != '\0') {    //M레코드의 코드 작성
				object_code[linenum] = (char*)malloc(70);
				object_code[linenum][0] = NULL;
				strcpy(object_code[linenum++], M_code[m++]);
			}
			//마지막E레코드 작성
			object_code[linenum] = (char*)malloc(70);
			if (sectnum == 0) {
				sprintf(object_code[linenum++], "E%06X", sectaddr[sectnum][0]); //첫 섹션은 돌아갈 시작주소
			}
			else
				strcpy(object_code[linenum++], "E");  //아니라면 돌아갈 시작주소를 모르기때문에 비움.

			object_code[linenum] = (char*)malloc(70);
			object_code[linenum][0] = '\0';
		}
	}

	FILE = fopen(file_name, "w");
	if (FILE != NULL) {
		int i = 0;
		while (object_code[i][0] != '\0') {
			fprintf(FILE, "%s\n", object_code[i++]);
		}
	}
	fclose(FILE);
}

/* -----------------------------------------------------------------------------------
* 설명 : pass1, pass2를 거쳐 만든 immediate Data를 화면으로 출력해주는 함수이다.
* 매계 : LTORG 연산 후 아직 주소를 배정받지 못한 리터럴들의 첫번재LITTAB 인덱스
* 반환 : 없음
* -----------------------------------------------------------------------------------
*/
void my_print(int litcnt) {
	int isop, issym, j = 0, count = 0;

	for (int i = 0; i < line_num; i++) {
		if (token_table[i]->label[0] == '.')		//주석은 개행 후 스킵
			continue;

		//Addr 출력
		if (token_table[i]->Addr != -1) {
			if (strcmp(token_table[i]->operator, "CSECT") == 0) {
				printf("\t");
			}
			else
				printf("%04X\t", token_table[i]->Addr);
		}
		else
			printf("\t");

		//label 출력
		if (token_table[i]->label[0] != '\0')
			printf("%s	", token_table[i]->label);
		else
			printf("\t");

		//operator 출력
		if (token_table[i]->operator[0] != '\0') {
			printf("%s	", token_table[i]->operator);
			if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG 인 경우
				for (; j < litcnt; j++) {
					if (lit_table[j].addr != -1) {
						printf("\n%04X\t*\t%s\t\t\t", lit_table[j].addr, lit_table[j].literal); //LITTAB에 주소값이 배정된 모든 리터럴 출력
						printf("%s\n", lit_table[j].litdata);
					}
				}
				continue;
			}
		}
		//operand 출력
		if (token_table[i]->operand[0][0] != '\0')
			printf("%s", token_table[i]->operand[0]);
		for (int j = 1; j < MAX_OPERAND; j++) {
			if (token_table[i]->operand[j][0] != '\0') {
				printf(",%s", token_table[i]->operand[j]);
			}
		}
		printf("\t\t");

		//objectcode 코드 출력
		int isinst = search_inst(token_table[i]->operator);
		int isob = isobcode(token_table[i]->operator);
		if (isob == 1) {
			if (token_table[i]->operator[0] == '+') {   //4형식
				printf("%06X", token_table[i]->obcode);
			}
			else if (isinst >= 0) {
				if (inst_table[isinst]->format == 34) { //3형식
					printf("%06X", token_table[i]->obcode);
				}
				else
					printf("%02X", token_table[i]->obcode);  //2형식
			}
			else {
				if (token_table[i]->obcode == 0)
					printf("%06X", token_table[i]->obcode);
				else
					printf("%02X", token_table[i]->obcode);
			}
		}

		//END 후 literal 출력
		if (strcmp(token_table[i]->operator, "END") == 0) { //프로그램이 끝났을 경우 literal 출력
			for (j = litcnt; j < token_line; j++) {
				if (lit_table[j].literal[0] != '\0') {
					printf("\n%04X\t*\t%s\t\t\t", lit_table[j].addr, lit_table[j].literal); //LITTAB에 주소값이 배정된 모든 리터럴 출력
					printf("%s\n", lit_table[j].litdata);
				}
			}
		}
		printf("\n");
	}
}

/* -----------------------------------------------------------------------------------
* 설명 : 해당 토큰테이블의 obcode가 출력할 obcode인지 확인해주는 함수이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 출력할 obcode : 1, 출력하지 말아야할 obcode : 0
* 주의 :
*
* -----------------------------------------------------------------------------------
*/

int isobcode(char* str) {
	if (strcmp(str, "EXTDEF") == 0 || strcmp(str, "EXTREF") == 0 || strcmp(str, "RESW") == 0
		|| strcmp(str, "RESB") == 0 || strcmp(str, "CSECT") == 0 || strcmp(str, "START") == 0
		|| strcmp(str, "END") == 0 || strcmp(str, "EQU") == 0 || strcmp(str, "LTORG") == 0) {
		return 0;
	}
	return 1;
}

/* -----------------------------------------------------------------------------------
* 설명 : 해당 obcode의 형식을 따져 칼럼의 크기를 알려주는 함수이다.
* 매계 : isinst : inst_table의 index, i : 인덱스
* 반환 : 해당 obcode의 칼럼 크기
* 주의 :
*
* -----------------------------------------------------------------------------------
*/
int obcode_len(int isinst, int i) {
	char T_code[10];
	if (token_table[i]->operator[0] == '+') {   //4형식
		return sprintf(T_code, "%08X", token_table[i]->obcode);
	}
	else if (isinst >= 0) {
		if (inst_table[isinst]->format == 34) { //3형식
			return sprintf(T_code, "%06X", token_table[i]->obcode);
		}
		else
			return sprintf(T_code, "%02X", token_table[i]->obcode);  //2형식
	}
	else {
		if (token_table[i]->obcode == 0)
			return sprintf(T_code, "%06X", token_table[i]->obcode);  //값이0인 경우
		else
			return sprintf(T_code, "%02X", token_table[i]->obcode);
	}
}