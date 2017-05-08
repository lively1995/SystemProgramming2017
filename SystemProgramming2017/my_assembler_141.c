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
* ���� : �Է� ���ڿ� SYMTAB�� ����ִ���,�� ���ǰ� ��ġ�ϴ��� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� label, �����ǹ�ȣ
* ��ȯ : �������� = �ش� symbol�� �ε���, ���� < 0
* ���� :
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
* ���� : �Է� ���ڿ��� inst_table�� ����ִ��� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� ��ū���̺��� operator
* ��ȯ : �������� = �ش� ���� �ڵ��� �ε���, ���� < 0
* ���� :
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
	litcnt = 0;
	int startaddr = 0, opnum = 0, symnum, j = 0, k = 0, csectnum = 0;
	char litbuf[5];
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

	int i = 0;

	if (strcmp(token_table[i]->operator, "START") == 0) {		//���α׷� ���� ��
		locctr = atoi(token_table[i]->operand[0]); //LOCCTR start �ּҷ� �ʱ�ȭ
		startaddr = locctr;
		token_table[i]->Addr = locctr;
		i++;
		sectaddr[0][0] = startaddr;
	}
	else {
		locctr = 0;
	}
	while (strcmp(token_table[i]->operator, "END") != 0) {					//���α׷��� ���� �� ���� ����
		if (token_table[i]->label[0] != '.') {	//�ּ��� �ƴ� ��� 
			if (strcmp(token_table[i]->operator, "CSECT") == 0) {
				sectaddr[csectnum][1] = locctr;
				locctr = 0;			//�� ���� ���� �� LOCCTR 0���� �ʱ�ȭ
				sym_table[search_symbol(token_table[i]->label)].section = ++csectnum;
				sym_table[search_symbol(token_table[i]->label)].addr = 0;
				token_table[i]->Addr = locctr;	//�ν�Ʈ���� �ּ� �ʱ�ȭ
				sectaddr[csectnum][0] = locctr;
			}
			if (token_table[i]->label[0] != '\0') {				//label�� ����
				symnum = search_symbol(token_table[i]->label);
				if (symnum < 0) {		//SYMTAB�� �������� ���� ���				
					strcpy(sym_table[j].symbol, token_table[i]->label);			//�ɺ����̺� �� ����
					sym_table[j].addr = locctr;									//�ɺ����̺� �ּ� ����
					sym_table[j].section = csectnum;							//�ɺ����̺� ���ǹ�ȣ ����
					token_table[i]->Addr = locctr;					//instruction �ּ� ����
					j++;
				}
				else {             //SYMTAB�� label�� ������ ���
					if (sym_table[symnum].section == csectnum) {
						sym_table[symnum].addr = locctr;			//�ɺ����̺� �ּ� ����				//RDREC �ּ� ����ȴ�!
					}
					else {
						strcpy(sym_table[j].symbol, token_table[i]->label);
						sym_table[j].addr = locctr;
						sym_table[j].section = csectnum;
						j++;
					}

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
								if (sym_table[symnum].section == csectnum)	//���� �Ľ��ϴ� ���ǰ� �ɺ����̺� EXTDEF�� ���� ����� operand ������ ��ġ�� ���
									sym_table[symnum].addr = locctr;	//�ش� �ɺ� addr�� �ּ� ����

							}
						}
					}
				}
				else if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG �߰� �� LITTAB����
					while (lit_table[litcnt].literal[0] != '\0') {	//LTORG �߰߱��� LITTAB�� ��ϵ� ���ͷ� ��ȸ 
						lit_table[litcnt].addr = locctr;			//�ش� literal�ּ� ���
						locctr += lit_table[litcnt].size;	//literal�� ��
						litcnt++;
					}
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
				if (token_table[i]->operand[0][0] != '*') {			//*(���� locctr ��)�� �ƴ϶��
					char op_token[20];			//�ӽ� ��ū
					char *tmp = (char*)malloc(10);
					int opaddr1, opaddr2;
					strcpy(op_token, token_table[i]->operand[0]);	//���� ����ؾ��ϴ� operand�� ���ۿ� ����
					if (strchr(op_token, '+')) {                    //+���� ���
						tmp = strtok(op_token, "+");					//�����ȣ�� �������� ��ū �и�
						opaddr1 = sym_table[search_symbol(tmp)].addr;	//ù��° operand�� �ּҸ� ã�� opaddr1�� ����(SYMTABȰ��)
						tmp = strtok(NULL, "+");						//���� ����
						opaddr2 = sym_table[search_symbol(tmp)].addr;
						token_table[i]->Addr = opaddr1 - opaddr2;		//���� ���� �ּҰ��� �ش� ��ū �ν�Ʈ���� �ּҿ� ����
						sym_table[search_symbol(tmp)].addr = token_table[i]->Addr;	//�ش� �ɺ��� �ּҿ� ����
					}
					else if (strchr(op_token, '-')) {               //-���� ���
						tmp = strtok(op_token, "-");					//�����ȣ�� �������� ��ū �и�
						opaddr1 = sym_table[search_symbol(tmp)].addr;	//ù��° operand�� �ּҸ� ã�� opaddr1�� ����(SYMTABȰ��)
						tmp = strtok(NULL, "-");						//���� ����
						opaddr2 = sym_table[search_symbol(tmp)].addr;
						token_table[i]->Addr = opaddr1 - opaddr2;		//���� ���� �ּҰ��� �ش� ��ū �ν�Ʈ���� �ּҿ� ����
						sym_table[search_symbol(token_table[i]->label)].addr = token_table[i]->Addr; //�ش� �ɺ��� �ּҿ� ����
					}
				}
			}
			int m = 3, n = 0;
			if (token_table[i]->operand[0][0] == '=') {
				int litnum = search_literal(token_table[i]->operand[0]);
				if (litnum < 0) {	//LITTAB�� ���� ��� (�ߺ� ����)
					strcpy(lit_table[k].literal, token_table[i]->operand[0]);	//LITTAB�� '='�� �����ϴ� literal ���

					while (lit_table[k].literal[m] != '\'') {				//=C�Ǵ� =X' ����(�迭[3]����) '�� ���ö����� 
						litbuf[n++] = lit_table[k].literal[m++];		//�� ĳ���;� litbuf�� ����
					}
					litbuf[n] = '\0';										//���� null���� �߰�

					if (lit_table[k].literal[1] == 'C') {					//=C' ', ĳ������ ���
						int index = 0;
						char *C_buf[10];
						while (litbuf[index] != '\0') {
							C_buf[index] = (char*)malloc(6);
							sprintf(C_buf[index], "%X", litbuf[index]);     //�ѱ��ھ�16������ ����Ͽ�
							strcat(lit_table[k].litdata, C_buf[index]);     //litdata�� ����
							index++;
						}
					}
					else
						strcpy(lit_table[k].litdata, litbuf);					//literal Data�� �����Ͽ� ���̺� ����

					if (strchr(lit_table[k].literal, 'C'))
						lit_table[k].size = (strlen(lit_table[k].litdata) / 2);
					else
						lit_table[k].size = (strlen(lit_table[k].litdata) - 1); //=X�� ��� ���� 2ĳ���ʹ� 1����Ʈ
					k++;
				}

			}
		}
		i++;
	}

	int formatnum;
	if (strcmp(token_table[token_line - 1]->operator, "END") == 0) { //���α׷��� ������ ��� LITTAB�� ���ͷ� �ּ� ���
		int tmp_locctr = 0;		//�ӽ� �����̼� ī����
		for (j = litcnt; j < token_line; j++) {
			if (lit_table[j].literal[0] != '\0') {
				tmp_locctr += token_table[token_line - 2]->Addr;	//END ���� ��ɾ� �ּҰ�
				formatnum = inst_table[token_line - 2]->format;		//ENd ���� ��ɾ��� ���� ũ�� Ȯ��
				if (formatnum == 34) {		//3,4����
					tmp_locctr += 3;
				}
				else if (formatnum == 2) {  //2����
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
	int i = 0, instnum = -1, sectnum = 0;

	while (i < token_line) {
		int extend = 0, notpc = 0, signcheck = 0;
		token_table[i]->nixbpe = 0;
		token_table[i]->obcode = 0;
		if (token_table[i]->label[0] == '.') {//�ּ��� ���
			i++;
			continue;
		}
		if (strcmp(token_table[i]->operator, "CSECT") == 0) {	//���� ��������
			sectnum++;
			i++;
			continue;
		}

		if (token_table[i]->operator[0] == '+') {		//extended ������ ���
			instnum = search_inst(&(token_table[i]->operator[1])); //ù��°����+�� �� �ش� operator�� ���� ��ɾ����� Ȯ��
			extend = 1;
			token_table[i]->nixbpe += 1;
			notpc = 1;
		}
		else
			instnum = search_inst(token_table[i]->operator);	//�ش� operator�� ���� ��ɾ����� Ȯ��


		if (instnum >= 0) {	//��ɾ instruction set�� �ִ� ��ɾ����� Ȯ��
			if (inst_table[instnum]->format == 2) {	//2������ ���
				token_table[i]->obcode = inst_table[instnum]->opcode << 8;	//opcode�� shift�� �� ����
				if (inst_table[instnum]->ops == 1) {	//operand�� 1���� ��� 
					for (int j = 0; j < 9; j++) {
						if (strcmp(token_table[i]->operand[0], reg_table[j]) == 0) {
							token_table[i]->obcode += j << 4;	//�ش� �������� ��ȣ ����
							break;
						}
					}
				}
				else {  //�������͸� 2�� �̻� ����� ���
					for (int j = 0; j < 9; j++) {
						if (strcmp(token_table[i]->operand[0], reg_table[j]) == 0) {
							token_table[i]->obcode += j << 4;	//ù��° �������� ��ȣ ����
							break;
						}
					}
					for (int j = 0; j < 9; j++) {
						if (strcmp(token_table[i]->operand[1], reg_table[j]) == 0) {
							token_table[i]->obcode += j;	//�ι�° �������� ��ȣ ����
							break;
						}
					}
				}
				i++;
				continue;
			}
			else {//3,4������ ���
				int litnum = -1, litcheck = 0;
				if (token_table[i]->operand[0][0] == '#') {	//immediate ������ ���
					token_table[i]->nixbpe += 1 << 4;	//ni=01
					if ('0' <= token_table[i]->operand[0][1] && '9' >= token_table[i]->operand[0][1]) {		//#�ڰ� 0~9�� ������ ���	
						token_table[i]->obcode += inst_table[instnum]->opcode << 16 | token_table[i]->nixbpe << 12 | atoi(&token_table[i]->operand[0][1]);
						i++;
						continue;
					}
					else {
						signcheck = 1;
					}
				}
				else if (token_table[i]->operand[0][0] == '@') {	//indirect ������ ���
					token_table[i]->nixbpe += 1 << 5;	//ni=10
					signcheck = 1;
				}
				else		//SIC/XE ����
					token_table[i]->nixbpe += 3 << 4;	//ni=11

				for (int j = 0; j <= inst_table[instnum]->ops; j++) {	//X�������͸� ����� ���
					if (strcmp(token_table[i]->operand[j], "X") == 0) {
						token_table[i]->nixbpe += 1 << 3;	//indexing ����
						break;
					}
				}

				if (token_table[i]->operand[0][0] == '\0') {
					;	//operand�� ���� ��� p=0;
				}
				else if (!extend)		//extend�� �ƴϸ� PC
					token_table[i]->nixbpe += 1 << 1;	//PC Relative, p=1;


				int symnum = -1;
				if (signcheck) {
					symnum = search_symbol(&token_table[i]->operand[0][1]);	//#, @ ��ȣ�� ���� ���
				}
				else if (token_table[i]->operand[0][0] == '=') {	//Literal ������ ���
					litnum = search_literal(token_table[i]->operand[0]);
					litcheck = 1;
				}
				else {
					symnum = search_symbol(token_table[i]->operand[0]);
				}

				int reladdr = 0;
				if (symnum >= 0) {	//operatnd�� �ɺ����̺� ������ ���
					if (sectnum == sym_table[symnum].section) {		//���� ���ǰ� �ɺ��� ���ǵ� ������ ���� ���
						reladdr = sym_table[symnum].addr - token_table[i]->Addr - 3;		//�ɺ� �ּ� - PC �ּ�
						if (reladdr < 0)
							reladdr = 0x0FFF & reladdr;
					}
					else
					{
						int tmp = -1;
						tmp = search_symbol2(token_table[i]->operand[0], sectnum);
						if (tmp >= 0) {
							reladdr = sym_table[tmp].addr - token_table[i]->Addr - 3;		//�ɺ� �ּ� - PC �ּ�
							if (reladdr < 0)
								reladdr = 0x0FFF & reladdr;
						}
					}

				}
				else if (litcheck) {		//literal ������ ���
					reladdr = lit_table[litnum].addr - token_table[i]->Addr - 3;
				}


				if (extend == 1)	//4������ ���
					token_table[i]->obcode += inst_table[instnum]->opcode << 24 | token_table[i]->nixbpe << 20 | reladdr;
				else
					token_table[i]->obcode += inst_table[instnum]->opcode << 16 | token_table[i]->nixbpe << 12 | reladdr;

			}
		}
		else if (strcmp(token_table[i]->operator, "WORD") == 0) {
			if ('0' <= token_table[i]->operand[0][0] && '9' >= token_table[i]->operand[0][0]) {		//0~9�� ������ ���
				token_table[i]->obcode = atoi(token_table[i]->operand[0]);
			}
			else if (strchr(token_table[i]->operand[0], '+')) {                    //+���� ���
				char op_token[10];
				strcpy(op_token[10], token_table[i]->operand[0]);
				int tmp = strtok(op_token, "+");
				int symindex = search_symbol(tmp);//�����ȣ�� �������� ��ū �и�
				if (sectnum == sym_table[symindex].section) {				//ù��° �ɺ��� �ش� ���ǿ� ���ǵ� �ɺ��� ���
					int opaddr1 = sym_table[symindex].addr;	//ù��° operand�� �ּҸ� ã�� opaddr1�� ����(SYMTABȰ��)
					tmp = strtok(NULL, "+");						//���� ����
					symindex = search_symbol(tmp);
					if (sectnum == sym_table[symindex].section) {
						int opaddr2 = sym_table[symindex].addr;
						token_table[i]->obcode = opaddr1 + opaddr2;		//���� ���� �ּҰ��� �ش� ��ū �ν�Ʈ���� �ּҿ� ����
					}
				}
			}

		}
		else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
			char litbuf[8];
			int j = 2, k = 0;
			while (token_table[i]->operand[0][j] != '\'') {				//=C�Ǵ� =X' ����(�迭[3]����) '�� ���ö����� 
				litbuf[k++] = token_table[i]->operand[0][j++];		//�� ĳ���;� litbuf�� ����
			}
			litbuf[k] = '\0';										//���� null���� �߰�

			token_table[i]->obcode = strtol(litbuf, NULL, 16);		//X'' �ϰ��
		}
		else {	//���� ��ɾ �ƴ� ���
			i++;
			continue;
		}
		i++;
	}

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
	FILE *FILE = NULL;
	char *object_code[50];	//obcode
	char *ref_table[3];
	int i = 0, j = 0;
	int sectnum = 0;	//���ǹ�ȣ
	int linenum = 0; //object_code�� ���� line, ���� index
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
		if (token_table[i]->label[0] == '.') {		//�ּ�
			continue;
		}
		if (strcmp(token_table[i]->operator, "CSECT") == 0) {
			if (T_code[0] != '\0') {  //������ �Ѿ�� ��� ������ ���� T���ڵ带 �ۼ�
				sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,�����ּ�,����,�ڵ� �ۼ�
				T_start = 0;
				T_cnt = 0;
				T_code[0] = NULL;
				linenum++;
			}

			int m = 0;
			while (M_code[m][0] != '\0') {    //M���ڵ��� �ڵ� �ۼ�
				object_code[linenum] = (char*)malloc(70);
				object_code[linenum][0] = NULL;
				strcpy(object_code[linenum++], M_code[m++]);
			}

			for (int m = 0; m < 10; m++) {  //M_�ڵ� �ʱ�ȭ
				M_code[m] = (char*)malloc(30);
				M_code[m][0] = '\0';
			}
			M_codeindex = 0;  //M_code �ε��� �ʱ�ȭ

			object_code[linenum] = (char*)malloc(70);
			object_code[linenum][0] = NULL;
			if (sectnum == 0) {
				sprintf(object_code[linenum++], "E%06X", sectaddr[sectnum][0]); //ù ������ ���ư� �����ּ�
			}
			else
				strcpy(object_code[linenum++], "E");  //�ƴ϶�� ���ư� �����ּҸ� �𸣱⶧���� ���.
			H_check = 0;
			sectnum++;
			continue;
		}
		length = 0;

		//H���ڵ�
		if (H_check == 0) {
			object_code[linenum] = (char*)malloc(70);
			length = sprintf(object_code[linenum], "H");
			if (sectnum == 0)
				length += sprintf(object_code[linenum] + length, "%-6s", token_table[i]->label); //���α׷� �̸�
			else
				length += sprintf(object_code[linenum] + length, "%-6s", token_table[i - 4]->label); //���� �̸�. �ּ�ũ��+EXTREF=3+1=4 �� 4��ŭ�������־� i-5
			length += sprintf(object_code[linenum] + length, "%06X", sectaddr[sectnum][0]);//�����ּ�
			length += sprintf(object_code[linenum] + length, "%06X", sectaddr[sectnum][1] - sectaddr[sectnum][0]); //���� = �������ּ�-�����ּ�
			length = 0;
			linenum++;
			H_check = 1;
		}

		//D���ڵ�
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
		else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {  //R���ڵ�
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

		//T���ڵ�
		object_code[linenum] = (char*)malloc(70);
		int isinst = search_inst(token_table[i]->operator);
		int isob = isobcode(token_table[i]->operator);
		int ob_len = obcode_len(isinst, i);  //�ش� obcode�� ���� �ľ�
		char ref_tmp[10], ref_tmp2[10];

		if (isob == 1) { //obcode�� �ִ� ���	
			if (T_cnt + ob_len >= 60) {		//60Į�� �̻��� ���
				sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,�����ּ�,����,�ڵ� �ۼ�
				T_start = 0;
				T_cnt = 0;
				T_code[0] = NULL;
				linenum++;
			}
			if (T_cnt == 0) { //T���ڵ� ù �����ּ� �Ǻ�
				T_start = token_table[i]->Addr;
			}

			T_cnt += sprintf(T_code + T_cnt, "%0*X", ob_len, token_table[i]->obcode);

			char op_token[20];			//�ӽ� ��ū
			strcpy(op_token, token_table[i]->operand[0]);	//���� ����ؾ��ϴ� operand�� ���ۿ� ����
			char* tmp = (char*)malloc(10);
			for (int r = 0; r < 3; r++) {
				if (strcmp(op_token, ref_table[r]) == 0) {  //���۷��� ���̺� �ִ��� Ȯ��
					if (ref_table[r][0] != '\0') {
						M_len = token_table[i]->Addr + 1;
						M_find = 1;
						ref_index = r;
						break;
					}
				}
				else if (strchr(token_table[i]->operand[0], '-')) {  //ref-ref�� ���
					tmp = strtok(op_token, "-");					//�����ȣ�� �������� ��ū �и�
					for (int re = 0; re < 3; re++) {
						if (strcmp(tmp, ref_table[re]) == 0) {  //���۷��� ���̺� �ִ��� Ȯ��
							if (ref_table[r][0] != '\0') {
								M_len = token_table[i]->Addr;
								M_find = 2;
								strcpy(ref_tmp, ref_table[re]);
								ref_index = re;
								break;
							}
						}
					}
					tmp = strtok(NULL, "-");						//�ι�° ref
					for (int re = 0; re < 3; re++) {
						if (strcmp(tmp, ref_table[re]) == 0) {  //���۷��� ���̺� �ִ��� Ȯ��
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
		else if (strcmp(token_table[i]->operator, "RESW") == 0 || strcmp(token_table[i]->operator, "RESW") == 0) { //object_code�� ���������� ���� ���
			if (T_code[0] != NULL) {
				sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,�����ּ�,����,�ڵ� �ۼ�
				T_start = 0;
				T_cnt = 0;
				T_code[0] = NULL;
				linenum++;//obcode�� ���������� ���� ���
				continue;
			}
		}
		else if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG�ϰ�� �߰�
			for (; j < litcnt; j++) {
				if (lit_table[j].addr != -1) {
					if (T_cnt + lit_table[j].size >= 60) {		//60Į�� �̻��� ���
						sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,�����ּ�,����,�ڵ� �ۼ�
						T_start = 0;
						T_cnt = 0;
						T_code[0] = NULL;
						linenum++;
					}
					if (T_cnt == 0) { //T���ڵ� ù �����ּ� �Ǻ�
						T_start = lit_table[j].addr;
					}

					T_cnt += sprintf(T_code + T_cnt, "%s", lit_table[j].litdata);
				}
			}
			sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,�����ּ�,����,�ڵ� �ۼ�
			T_start = 0;
			T_cnt = 0;
			T_code[0] = NULL;
			linenum++;
			continue;
		}


		//M���ڵ�
		object_code[linenum] = (char*)malloc(70);
		if (M_find == 1) {  //ref�� 4�������� �������� ���
			M_code[M_codeindex] = (char*)malloc(30);
			sprintf(M_code[M_codeindex], "M%06X05+%s", M_len, ref_table[ref_index]); //��ġ,n��°������������,+,���۷���
			M_codeindex++;
			M_find = 0;
		}
		else if (M_find == 2) { //������ �ִ� m���ڵ��� ��� ref-ref
			for (int re = 0; re < 2; re++) {
				M_code[M_codeindex] = (char*)malloc(30);
				if (re == 0)
					sprintf(M_code[M_codeindex], "M%06X06+%s", M_len, ref_tmp); //��ġ,n��°������������,+,���۷���
				else
					sprintf(M_code[M_codeindex], "M%06X06-%s", M_len, ref_tmp2); //��ġ,n��°������������,-,���۷���
				M_codeindex++;
			}
			M_find = 0;
		}


		//END, ���α׷��� ������ ��
		if (strcmp(token_table[i]->operator, "END") == 0) {
			for (j = litcnt; j < token_line; j++) {
				if (lit_table[j].addr != -1) {
					if (T_cnt + lit_table[j].size >= 60) {		//60Į�� �̻��� ���
						sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code); //T,�����ּ�,����,�ڵ� �ۼ�
						T_start = 0;
						T_cnt = 0;
						T_code[0] = NULL;
						linenum++;
					}
					if (T_cnt == 0) { //T���ڵ� ù �����ּ� �Ǻ�
						T_start = lit_table[j].addr;
					}

					T_cnt += sprintf(T_code + T_cnt, "%s", lit_table[j].litdata);
					break;
				}
			}
			//������T���ڵ� �ۼ�
			sprintf(object_code[linenum], "T%06X%02X%s", T_start, T_cnt / 2, T_code);
			linenum++;
			//������M���ڵ� �ۼ�
			int m = 0;
			while (M_code[m][0] != '\0') {    //M���ڵ��� �ڵ� �ۼ�
				object_code[linenum] = (char*)malloc(70);
				object_code[linenum][0] = NULL;
				strcpy(object_code[linenum++], M_code[m++]);
			}
			//������E���ڵ� �ۼ�
			object_code[linenum] = (char*)malloc(70);
			if (sectnum == 0) {
				sprintf(object_code[linenum++], "E%06X", sectaddr[sectnum][0]); //ù ������ ���ư� �����ּ�
			}
			else
				strcpy(object_code[linenum++], "E");  //�ƴ϶�� ���ư� �����ּҸ� �𸣱⶧���� ���.

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
* ���� : pass1, pass2�� ���� ���� immediate Data�� ȭ������ ������ִ� �Լ��̴�.
* �Ű� : LTORG ���� �� ���� �ּҸ� �������� ���� ���ͷ����� ù����LITTAB �ε���
* ��ȯ : ����
* -----------------------------------------------------------------------------------
*/
void my_print(int litcnt) {
	int isop, issym, j = 0, count = 0;

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
		if (token_table[i]->label[0] != '\0')
			printf("%s	", token_table[i]->label);
		else
			printf("\t");

		//operator ���
		if (token_table[i]->operator[0] != '\0') {
			printf("%s	", token_table[i]->operator);
			if (strcmp(token_table[i]->operator, "LTORG") == 0) {		//LTORG �� ���
				for (; j < litcnt; j++) {
					if (lit_table[j].addr != -1) {
						printf("\n%04X\t*\t%s\t\t\t", lit_table[j].addr, lit_table[j].literal); //LITTAB�� �ּҰ��� ������ ��� ���ͷ� ���
						printf("%s\n", lit_table[j].litdata);
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
		printf("\t\t");

		//objectcode �ڵ� ���
		int isinst = search_inst(token_table[i]->operator);
		int isob = isobcode(token_table[i]->operator);
		if (isob == 1) {
			if (token_table[i]->operator[0] == '+') {   //4����
				printf("%06X", token_table[i]->obcode);
			}
			else if (isinst >= 0) {
				if (inst_table[isinst]->format == 34) { //3����
					printf("%06X", token_table[i]->obcode);
				}
				else
					printf("%02X", token_table[i]->obcode);  //2����
			}
			else {
				if (token_table[i]->obcode == 0)
					printf("%06X", token_table[i]->obcode);
				else
					printf("%02X", token_table[i]->obcode);
			}
		}

		//END �� literal ���
		if (strcmp(token_table[i]->operator, "END") == 0) { //���α׷��� ������ ��� literal ���
			for (j = litcnt; j < token_line; j++) {
				if (lit_table[j].literal[0] != '\0') {
					printf("\n%04X\t*\t%s\t\t\t", lit_table[j].addr, lit_table[j].literal); //LITTAB�� �ּҰ��� ������ ��� ���ͷ� ���
					printf("%s\n", lit_table[j].litdata);
				}
			}
		}
		printf("\n");
	}
}

/* -----------------------------------------------------------------------------------
* ���� : �ش� ��ū���̺��� obcode�� ����� obcode���� Ȯ�����ִ� �Լ��̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����� obcode : 1, ������� ���ƾ��� obcode : 0
* ���� :
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
* ���� : �ش� obcode�� ������ ���� Į���� ũ�⸦ �˷��ִ� �Լ��̴�.
* �Ű� : isinst : inst_table�� index, i : �ε���
* ��ȯ : �ش� obcode�� Į�� ũ��
* ���� :
*
* -----------------------------------------------------------------------------------
*/
int obcode_len(int isinst, int i) {
	char T_code[10];
	if (token_table[i]->operator[0] == '+') {   //4����
		return sprintf(T_code, "%08X", token_table[i]->obcode);
	}
	else if (isinst >= 0) {
		if (inst_table[isinst]->format == 34) { //3����
			return sprintf(T_code, "%06X", token_table[i]->obcode);
		}
		else
			return sprintf(T_code, "%02X", token_table[i]->obcode);  //2����
	}
	else {
		if (token_table[i]->obcode == 0)
			return sprintf(T_code, "%06X", token_table[i]->obcode);  //����0�� ���
		else
			return sprintf(T_code, "%02X", token_table[i]->obcode);
	}
}