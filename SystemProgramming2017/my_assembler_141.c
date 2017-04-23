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
	
	if(assem_pass1() < 0 ){
	printf("assem_pass1: Failed to pass1 process. \n") ;
	return -1 ;
	}
	if(assem_pass2() < 0 ){
	printf(" assem_pass2: Failed to pass2 process. \n") ;
	return -1 ;
	}
	
	make_opcode_output("output_141");

	//make_objectcode_output("output");
	
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
//		token_table[index]->operator[0] = '.';
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
/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	FILE *FILE = NULL;
	FILE = fopen(file_name, "w");

	if (FILE != NULL) {
		int isop;	//serach_opcode return값을 받는 변수

		for (int i = 0; i < line_num; i++) {
			//label 출력
			if (token_table[i]->label[0] == '.')		//주석은 개행 후 스킵
				continue;
			else if (token_table[i]->label[0] != '\0')
				fprintf(FILE, "%s	", token_table[i]->label);
			else
				fprintf(FILE, "\t");

			//operator 출력
			if (token_table[i]->operator[0] != '\0')
				fprintf(FILE, "%s	", token_table[i]->operator);

			//operand 출력
			if (token_table[i]->operand[0][0] != '\0')
				fprintf(FILE, "%s", token_table[i]->operand[0]);
			for (int j = 1; j < MAX_OPERAND; j++) {
				if (token_table[i]->operand[j][0] != '\0') {
					fprintf(FILE, ",%s", token_table[i]->operand[j]);
				}
			}
			fprintf(FILE, "\t");

			//opcode 출력

			isop = search_opcode(token_table[i]->operator);	//일반 operator
			if (isop >= 0) {
				fprintf(FILE, "\t\t%02X", inst_table[isop]->opcode);	//해당 opcode 출력
			}
			fprintf(FILE, "\n");
		}
	}
	else {
		printf("ERROR : Failed to Read file.");
		exit(0);
	}

	fclose(FILE);
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
	int startaddr = 0, opnum = 0, symnum, j = 0, k = 0, csectnum=0;
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

	int i=0;
	
	if (strcmp(token_table[i]->operator, "START") == 0) {		//프로그램 시작 시
		locctr = atoi(token_table[i]->operand[0]); //LOCCTR start 주소로 초기화
		startaddr = locctr;
		token_table[i]->Addr = locctr;
		i++;
	}
	else {
		locctr = 0;
		}
	while(strcmp(token_table[i]->operator, "END") != 0){					//프로그램이 끝날 때 까지 루프
		if (token_table[i]->label[0] != '.') {	//주석이 아닐 경우 
			if (token_table[i]->label[0] != '\0') {				//label이 존재
				symnum = search_symbol(token_table[i]->label);
				if (symnum < 0) {		//SYMTAB에 존재하지 않을 경우				
					strcpy(sym_table[j].symbol, token_table[i]->label);			//심볼테이블 라벨 저장
					if (sym_table[symnum].section == csectnum)
						sym_table[j].addr = locctr;									//심볼테이블 주소 저장
					sym_table[j].section = csectnum;							//심볼테이블 섹션번호 저장
					token_table[i]->Addr =locctr;					//instruction 주소 저장
					j++;
				}
				else {             //SYMTAB에 label이 존재할 경우
					if (sym_table[symnum].section == csectnum)
						sym_table[symnum].addr = locctr;			//심볼테이블 주소 저장				//RDREC 주소 저장된다!
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
								if(sym_table[symnum].section == csectnum)	//현재 파싱하는 섹션과 심볼테이블에 EXTDEF에 의해 저장된 operand 섹션이 일치할 경우
									sym_table[symnum].addr = locctr;	//해당 심볼 addr에 주소 저장
								
							}
						}
					}
				}
				//else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {	//외부참조 EXTREF일 경우
				//	symnum = 0;
				//	for (int idx = 0; idx < MAX_OPERAND; idx++) {
				//		if (token_table[i]->operand[idx][0] != '\0') {	//operand 존재할 경우
				//			symnum = search_symbol(token_table[i]->operand[idx]);
				//			if (symnum < 0) {		//SYMTAB에 존재하지 않을 경우				
				//				strcpy(sym_table[j].symbol, token_table[i]->operand[idx]);			//심볼테이블 라벨 저장								
				//				sym_table[j].section = ++csectnum;
				//				j++;
				//			}
				//			else {		//SYMTAB에 존재할 경우
				//				sym_table[symnum].addr = locctr;	//해당 심볼 addr에 주소 저장
				//			}
				//		}
				//	}
				//}
				else if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG 발견 시 LITTAB참조
					lit_table[0].addr = locctr;			//해당 literal주소 등록
														//strlen(lit_table[0].literal)-4;	//'' 사이 값 어떻게 불러올까?
					locctr += strlen(lit_table[0].literal) - 4;	//EOF 길이만큼 +
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
				 locctr += 3*(atoi(token_table[i]->operand[0]));
			}
			else if (strcmp(token_table[i]->operator, "RESB") == 0) {
				locctr += atoi(token_table[i]->operand[0]);
			}
			else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
				locctr += 1;
			}
			else if (strcmp(token_table[i]->operator, "CSECT") == 0) {
				locctr = 0;			//새 섹션 시작 시 LOCCTR 0으로 초기화
				sym_table[search_symbol(token_table[i]->label)].section = ++csectnum;
				sym_table[search_symbol(token_table[i]->label)].addr = 0;
				token_table[i]->Addr = locctr;	//인스트럭션 주소 초기화
			}
			else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
				locctr += 1;
			}else if (strcmp(token_table[i]->operator, "EQU") == 0){
				if (token_table[i]->operand[0][0] != '*') {			//*(현재 locctr 값)이 아니라면
					char op_token[20];			//임시 토큰
					char *tmp=(char*)malloc(10);	
					int opaddr1, opaddr2;
					strcpy(op_token, token_table[i]->operand[0]);	//값을 계산해야하는 operand를 버퍼에 복사
					tmp = strtok(op_token, "+-*/");					//사칙연산기호를 기준으로 토큰 분리
					opaddr1 = sym_table[search_symbol(tmp)].addr;	//첫번째 operand의 주소를 찾아 opaddr1에 대입(SYMTAB활용)
					tmp = strtok(NULL, "+-*/");						//위와 동일
					opaddr2 = sym_table[search_symbol(tmp)].addr;
					token_table[i]->Addr = opaddr1 - opaddr2;		//연산 후의 주소값을 해당 토큰 인스트럭션 주소에 대입
				}
			}
			
			if (token_table[i]->operand[0][0] == '=') {
				int litnum= search_literal(token_table[i]->operand[0]);
				if (litnum <0) {	//LITTAB에 없을 경우 (중복 방지)
					strcpy(lit_table[k].literal, token_table[i]->operand[0]);	//LITTAB에 '='로 시작하는 literal 등록
					k++;
				}
			}
		}
		i++;
	}

	my_print();
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

	/* add your code here */
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
	/* add your code here */
}

void my_print() {
	int isop, issym, j=0, count=0, litcnt=0;

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
		//if (token_table[i]->label[0] == '.')		//주석은 개행 후 스킵
		//	continue;
		if (token_table[i]->label[0] != '\0')
			printf("%s	", token_table[i]->label);
		else
			printf("\t");

		//operator 출력
		
		if (token_table[i]->operator[0] != '\0') {
			printf("%s	", token_table[i]->operator);
			if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG 인 경우
				for (; j < token_line; j++) {
					if (lit_table[j].addr != -1) {
						printf("\n%04X\t*\t%s\n", lit_table[j].addr, lit_table[j].literal); //LITTAB에 주소값이 배정된 모든 리터럴 출력
						litcnt++;
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
		printf("\t");

		if (strcmp(token_table[i]->operator, "END") == 0) { //프로그램이 끝났을 경우 literal 출력
			int tmp_locctr = 0;		//임시 로케이션 카운터
			for (j = litcnt; j < token_line; j++) {
				if (lit_table[j].literal[0] != '\0') {
					tmp_locctr += token_table[i - 1]->Addr + 3;	//보통 RSUB으로 끝나므로 3형식으로 판단
					printf("\n%04X\t*\t%s\n", tmp_locctr, lit_table[j].literal); //LITTAB에 주소값이 배정된 모든 리터럴 출력
				}
			}
		}
		printf("\n");
	}
}