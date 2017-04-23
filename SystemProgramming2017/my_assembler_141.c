/*
* ȭ�ϸ� : my_assembler.c
* ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
* �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
*
*/

/*
*
* ���α׷��� ����� �����Ѵ�.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* my_assembler ���α׷��� �������� �����͵��� ���ǵ� ���̴�.
*/

#include "my_assembler_141.h"

/* -----------------------------------------------------------------------------------
* ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
* �Ű� : ���� ����, ����� ����
* ��ȯ : ���� = 0, ���� = < 0
* ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
*		   ���� �߰������� �������� �ʴ´�.
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
* ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
* �Ű� : ����4t
* ��ȯ : �������� = 0 , ���� �߻� = -1
* ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
*		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
*		   �����Ͽ���.
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
* ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
*        �����ϴ� �Լ��̴�.
* �Ű� : ���� ��� ����
* ��ȯ : �������� = 0 , ���� < 0
* ���� : ���� ������� ������ ������ ����.
*
*	===============================================================================
*		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
*	===============================================================================
*
* -----------------------------------------------------------------------------------
*/

int init_inst_file(char *inst_file)
{
	for (int i = 0; i < MAX_INST; i++) {			//inst_table �޸��Ҵ�
		inst_table[i] = (struct inst*)malloc(sizeof(inst));
	}
	inst_index = 0;

	FILE *FILE = NULL;
	char str[128];	//���κ��� ���پ� ������ �ӽ� ����
	char *p_token = malloc(sizeof(char) * 128);	//��ū����

	FILE = fopen(inst_file, "r");
	if (FILE != NULL) {
		while (!feof(FILE)) {
			fgets(str, sizeof(str), FILE);	//txt���Ϸκ��� �� �� �� �б� 

			if (str[strlen(str) - 1] == '\n')	//������ line�� ������ string�� ������ null�� ����(������ line�� ���� ������ �����Ƿ�)
				str[strlen(str) - 1] = '\0';

			strcpy(p_token, strtok(str, "\t"));	//p_token�� ���� �������� ��ū���� ���� ����
			inst_table[inst_index]->operator = (char *)malloc(sizeof(str));	//operator(char *) �޸� �Ҵ�
			strcpy(inst_table[inst_index]->operator , p_token);	//operator�� ��ū ����
			for (int i = 0; i < 3; i++) {
				strcpy(p_token, strtok(NULL, "\t"));			//operator ������ ��ū �и��Ͽ� ����
				if (i == 0)
					inst_table[inst_index]->format = atoi(p_token);	//format ����
				else if (i == 1) {
					inst_table[inst_index]->opcode = strtol(p_token, NULL, 16);	//16���� ���·� ����
				}
				else
					inst_table[inst_index]->ops = atoi(p_token);	//op���� ���� ����
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
* ���� : ����� �� �ҽ��ڵ带 �о���� �Լ��̴�.
* �Ű� : ������� �ҽ����ϸ�
* ��ȯ : �������� = 0 , ���� < 0
* ���� :
*
* -----------------------------------------------------------------------------------
*/


int init_input_file(char *input_file)
{
	for (int i = 0; i < MAX_LINES; i++) {
		token_table[i] = (struct token_unit*)malloc(sizeof(struct token_unit));
	}
	line_num = 0;	//input������ ������ line number

	FILE *FILE = NULL;
	char str[128];	//���κ��� ���پ� ������ �ӽ� ����
	char *p_token = malloc(sizeof(char) * 128);	//��ū����

	FILE = fopen(input_file, "r");
	if (FILE != NULL) {
		while (!feof(FILE)) {
			fgets(str, sizeof(str), FILE); //txt���Ϸκ��� �� �� �� �б� 

			if (str[strlen(str) - 1] == '\n')	//������ line�� ������ string�� ������ null�� ����(������ line�� ���� ������ �����Ƿ�)
				str[strlen(str) - 1] = '\0';

			input_data[line_num] = (char*)malloc(sizeof(str));
			strcpy(input_data[line_num], str);	//���κ� input data ����
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
* ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
*        �н� 1�� ���� ȣ��ȴ�.
* �Ű� : �ҽ��ڵ��� ���ι�ȣ
* ��ȯ : �������� = 0 , ���� < 0
* ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
* -----------------------------------------------------------------------------------
*/

int token_parsing(int index)
{
	/* add your code here */
	char *str = (char*)malloc(sizeof(char) * 128);	//����
	char *p_token = (char*)malloc(sizeof(char) * 50);		//��ū
	char *opbuf = (char*)malloc(30);				//operand ����
	char *op_token = (char*)malloc(10);				//operand ��ū
	strcpy(str, input_data[index]);					//�� ���� �� str������ ����

	token_table[index]->Addr = -1;				//�ν�Ʈ���� �ּ� �ʱ�ȭ

	//label �Ľ�
	token_table[index]->label = (char*)malloc(sizeof(char) * 7);	//label �޸� �Ҵ�
	if (str[0] == '.') {		//�ּ�ó��
		token_table[index]->label[0] = '.';	//�ּ��� ��� label�� ����
		token_table[index]->operator = (char*)malloc(strlen(p_token) + 1);
//		token_table[index]->operator[0] = '.';
		return 0;
	}
	else if (str[0] != '\t') {	//ù��° ���ڰ� ���� �ƴ� ��� (���� ��쿣 label�� ����)
		strcpy(p_token, strtok(str, "\t"));	//label
		strcpy(token_table[index]->label, p_token);
	}

	//operator �Ľ�
	if (str[0] == '\t') {		//label�� ���� line�� ù��° ��ū�� operator�̹Ƿ� �켱ó��
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
	if (strcmp(token_table[index]->operator, "RSUB") == 0) {	//RSUB�� ��� operand�� ���� comment�� �ִ� line�̹Ƿ� ����ó��
		strcpy(p_token, strtok(NULL, "\t"));
		token_table[index]->comment = (char*)malloc(strlen(p_token) + 1);
		strcpy(token_table[index]->comment, p_token);

		for (int i = 0; i < MAX_OPERAND; i++) {		//�ʱ�ȭ
			token_table[index]->operand[i] = (char*)malloc(8);
			token_table[index]->operand[i][0] = '\0';
		}
		return 0;
	}
	p_token = strtok(NULL, "\t");
	if (p_token != NULL) {
		strcpy(opbuf, p_token);					//comment���� ó���ϱ� ���� operand ��ū�� ���� ����
	}
	else
		opbuf = NULL;

	//comment �Ľ�
	p_token = strtok(NULL, "\t");
	if (p_token != NULL) {			//comment�� ���� ���
		token_table[index]->comment = (char*)malloc(strlen(p_token) + 1);
		strcpy(token_table[index]->comment, p_token);
	}
	else {							//comment�� ���� ���
		token_table[index]->comment = (char*)malloc(sizeof(p_token));
		token_table[index]->comment[0] = '\0';
	}

	//operand �Ľ�
	for (int i = 0; i < MAX_OPERAND; i++) {		//�ʱ�ȭ
		token_table[index]->operand[i] = (char*)malloc(8);
		token_table[index]->operand[i][0] = '\0';
	}
	if (opbuf != NULL) {		//operand�� ����
		if (strchr(opbuf, ',') == NULL) {	//���� operand
			strcpy(token_table[index]->operand[0], opbuf);
		}
		else {
			strcpy(op_token, strtok(opbuf, ","));		//1��° operand
			strcpy(token_table[index]->operand[0], op_token);

			strcpy(op_token, strtok(NULL, ","));		//2��° operand
			strcpy(token_table[index]->operand[1], op_token);

			op_token = strtok(NULL, ",");				//3��° operand�� �ִ��� �˻�
			if (op_token != NULL) {
				strcpy(token_table[index]->operand[2], op_token);
			}
		}
	}

	return 0;
}
/* -----------------------------------------------------------------------------------
* ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� ���ڿ�
* ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
* ���� :
*
* -----------------------------------------------------------------------------------
*/

int search_opcode(char *str)
{
	for (int i = 0; i < inst_index; i++) {
		if (str[0] == '+') {									//+�� �ٴ� extended�� ���
			if (strcmp(inst_table[i]->operator, &str[1]) == 0)	//+ ���� �����ּҸ� �Ű������� ���
				return i;
		}
		else if (strcmp(inst_table[i]->operator, str) == 0)		//�Ϲ� operator
			return i;
	}
	return -1;
}
/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	FILE *FILE = NULL;
	FILE = fopen(file_name, "w");

	if (FILE != NULL) {
		int isop;	//serach_opcode return���� �޴� ����

		for (int i = 0; i < line_num; i++) {
			//label ���
			if (token_table[i]->label[0] == '.')		//�ּ��� ���� �� ��ŵ
				continue;
			else if (token_table[i]->label[0] != '\0')
				fprintf(FILE, "%s	", token_table[i]->label);
			else
				fprintf(FILE, "\t");

			//operator ���
			if (token_table[i]->operator[0] != '\0')
				fprintf(FILE, "%s	", token_table[i]->operator);

			//operand ���
			if (token_table[i]->operand[0][0] != '\0')
				fprintf(FILE, "%s", token_table[i]->operand[0]);
			for (int j = 1; j < MAX_OPERAND; j++) {
				if (token_table[i]->operand[j][0] != '\0') {
					fprintf(FILE, ",%s", token_table[i]->operand[j]);
				}
			}
			fprintf(FILE, "\t");

			//opcode ���

			isop = search_opcode(token_table[i]->operator);	//�Ϲ� operator
			if (isop >= 0) {
				fprintf(FILE, "\t\t%02X", inst_table[isop]->opcode);	//�ش� opcode ���
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
* ���� : �Է� ���ڿ��� SYMTAB�� ����ִ��� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� label
* ��ȯ : �������� = �ش� symbol�� �ε���, ���� < 0
* ���� :
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
* ���� : �Է� ���ڿ��� LITTAB�� ����ִ��� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� literal
* ��ȯ : �������� = �ش� literal�� �ε���, ���� < 0
* ���� :
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
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{	
	token_line = 0;
	int startaddr = 0, opnum = 0, symnum, j = 0, k = 0, csectnum=0;
	char *str = (char*)malloc(sizeof(char) * 128);	//����
	char *p_token = (char*)malloc(sizeof(char) * 50);		//��ū

	for (int i = 0; i < line_num; i++) {
		if (token_parsing(i) < 0) {									//��ū ���̺� �ۼ�
			return -1;
		}
		token_line++;
	}

	for (int i = 0; i < token_line; i++) {							//���ͷ����̺� �ʱ�ȭ
		lit_table[i].addr = -1;
		lit_table[i].literal[0] = '\0';
	}

	int i=0;
	
	if (strcmp(token_table[i]->operator, "START") == 0) {		//���α׷� ���� ��
		locctr = atoi(token_table[i]->operand[0]); //LOCCTR start �ּҷ� �ʱ�ȭ
		startaddr = locctr;
		token_table[i]->Addr = locctr;
		i++;
	}
	else {
		locctr = 0;
		}
	while(strcmp(token_table[i]->operator, "END") != 0){					//���α׷��� ���� �� ���� ����
		if (token_table[i]->label[0] != '.') {	//�ּ��� �ƴ� ��� 
			if (token_table[i]->label[0] != '\0') {				//label�� ����
				symnum = search_symbol(token_table[i]->label);
				if (symnum < 0) {		//SYMTAB�� �������� ���� ���				
					strcpy(sym_table[j].symbol, token_table[i]->label);			//�ɺ����̺� �� ����
					if (sym_table[symnum].section == csectnum)
						sym_table[j].addr = locctr;									//�ɺ����̺� �ּ� ����
					sym_table[j].section = csectnum;							//�ɺ����̺� ���ǹ�ȣ ����
					token_table[i]->Addr =locctr;					//instruction �ּ� ����
					j++;
				}
				else {             //SYMTAB�� label�� ������ ���
					if (sym_table[symnum].section == csectnum)
						sym_table[symnum].addr = locctr;			//�ɺ����̺� �ּ� ����				//RDREC �ּ� ����ȴ�!
					token_table[i]->Addr = locctr;	//instruction �ּ� ����		
				}
			}
			else {		//label�� ���� ���
				if (strcmp(token_table[i]->operator, "EXTDEF") == 0 || strcmp(token_table[i]->operator, "EXTREF") == 0) {	//�ܺ����� EXTDEF�� ���
					symnum = 0;
					for (int idx = 0; idx < MAX_OPERAND; idx++) {
						if (token_table[i]->operand[idx][0] != '\0') {	//operand ������ ���
							symnum = search_symbol(token_table[i]->operand[idx]);
							if (symnum < 0) {		//SYMTAB�� �������� ���� ���				
								strcpy(sym_table[j].symbol, token_table[i]->operand[idx]);			//�ɺ����̺� �� ����								
								sym_table[j].section = csectnum;		//���ǹ�ȣ ����
								j++;
							}
							else {		//SYMTAB�� ������ ���
								if(sym_table[symnum].section == csectnum)	//���� �Ľ��ϴ� ���ǰ� �ɺ����̺� EXTDEF�� ���� ����� operand ������ ��ġ�� ���
									sym_table[symnum].addr = locctr;	//�ش� �ɺ� addr�� �ּ� ����
								
							}
						}
					}
				}
				//else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {	//�ܺ����� EXTREF�� ���
				//	symnum = 0;
				//	for (int idx = 0; idx < MAX_OPERAND; idx++) {
				//		if (token_table[i]->operand[idx][0] != '\0') {	//operand ������ ���
				//			symnum = search_symbol(token_table[i]->operand[idx]);
				//			if (symnum < 0) {		//SYMTAB�� �������� ���� ���				
				//				strcpy(sym_table[j].symbol, token_table[i]->operand[idx]);			//�ɺ����̺� �� ����								
				//				sym_table[j].section = ++csectnum;
				//				j++;
				//			}
				//			else {		//SYMTAB�� ������ ���
				//				sym_table[symnum].addr = locctr;	//�ش� �ɺ� addr�� �ּ� ����
				//			}
				//		}
				//	}
				//}
				else if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG �߰� �� LITTAB����
					lit_table[0].addr = locctr;			//�ش� literal�ּ� ���
														//strlen(lit_table[0].literal)-4;	//'' ���� �� ��� �ҷ��ñ�?
					locctr += strlen(lit_table[0].literal) - 4;	//EOF ���̸�ŭ +
				}
				else
					token_table[i]->Addr = locctr;
			}
			
			opnum = search_opcode(token_table[i]->operator);
			if (opnum >= 0) {		//��ɾ ������
				if (inst_table[opnum]->format == 34) {		//
					if (token_table[i]->operator[0] == '+') {		//+�� ���� 4������ ���
						locctr += 4;
					}
					else
						locctr += 3;
				}
				else if (inst_table[opnum]->format == 2) {	//2�����̸�
					locctr += 2;
				}
				else	//1�����̸�
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
				locctr = 0;			//�� ���� ���� �� LOCCTR 0���� �ʱ�ȭ
				sym_table[search_symbol(token_table[i]->label)].section = ++csectnum;
				sym_table[search_symbol(token_table[i]->label)].addr = 0;
				token_table[i]->Addr = locctr;	//�ν�Ʈ���� �ּ� �ʱ�ȭ
			}
			else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
				locctr += 1;
			}else if (strcmp(token_table[i]->operator, "EQU") == 0){
				if (token_table[i]->operand[0][0] != '*') {			//*(���� locctr ��)�� �ƴ϶��
					char op_token[20];			//�ӽ� ��ū
					char *tmp=(char*)malloc(10);	
					int opaddr1, opaddr2;
					strcpy(op_token, token_table[i]->operand[0]);	//���� ����ؾ��ϴ� operand�� ���ۿ� ����
					tmp = strtok(op_token, "+-*/");					//��Ģ�����ȣ�� �������� ��ū �и�
					opaddr1 = sym_table[search_symbol(tmp)].addr;	//ù��° operand�� �ּҸ� ã�� opaddr1�� ����(SYMTABȰ��)
					tmp = strtok(NULL, "+-*/");						//���� ����
					opaddr2 = sym_table[search_symbol(tmp)].addr;
					token_table[i]->Addr = opaddr1 - opaddr2;		//���� ���� �ּҰ��� �ش� ��ū �ν�Ʈ���� �ּҿ� ����
				}
			}
			
			if (token_table[i]->operand[0][0] == '=') {
				int litnum= search_literal(token_table[i]->operand[0]);
				if (litnum <0) {	//LITTAB�� ���� ��� (�ߺ� ����)
					strcpy(lit_table[k].literal, token_table[i]->operand[0]);	//LITTAB�� '='�� �����ϴ� literal ���
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
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/

static int assem_pass2(void)
{

	/* add your code here */
	return 0;
}

/* -----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
		if (token_table[i]->label[0] == '.')		//�ּ��� ���� �� ��ŵ
			continue;
		
		//Addr ���
		if (token_table[i]->Addr != -1) {
			if (strcmp(token_table[i]->operator, "CSECT") == 0) {
				printf("\t");
			}
			else
				printf("%04X\t", token_table[i]->Addr);

		}
		else
			printf("\t");
		
		//label ���
		//if (token_table[i]->label[0] == '.')		//�ּ��� ���� �� ��ŵ
		//	continue;
		if (token_table[i]->label[0] != '\0')
			printf("%s	", token_table[i]->label);
		else
			printf("\t");

		//operator ���
		
		if (token_table[i]->operator[0] != '\0') {
			printf("%s	", token_table[i]->operator);
			if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG �� ���
				for (; j < token_line; j++) {
					if (lit_table[j].addr != -1) {
						printf("\n%04X\t*\t%s\n", lit_table[j].addr, lit_table[j].literal); //LITTAB�� �ּҰ��� ������ ��� ���ͷ� ���
						litcnt++;
					}
				}
				continue;
			}
		}
		//operand ���
		if (token_table[i]->operand[0][0] != '\0')
			printf("%s", token_table[i]->operand[0]);
		for (int j = 1; j < MAX_OPERAND; j++) {
			if (token_table[i]->operand[j][0] != '\0') {
				printf(",%s", token_table[i]->operand[j]);
			}
		}
		printf("\t");

		if (strcmp(token_table[i]->operator, "END") == 0) { //���α׷��� ������ ��� literal ���
			int tmp_locctr = 0;		//�ӽ� �����̼� ī����
			for (j = litcnt; j < token_line; j++) {
				if (lit_table[j].literal[0] != '\0') {
					tmp_locctr += token_table[i - 1]->Addr + 3;	//���� RSUB���� �����Ƿ� 3�������� �Ǵ�
					printf("\n%04X\t*\t%s\n", tmp_locctr, lit_table[j].literal); //LITTAB�� �ּҰ��� ������ ��� ���ͷ� ���
				}
			}
		}
		printf("\n");
	}
}