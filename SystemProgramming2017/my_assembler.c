/*
* ȭ�ϸ� : my_assembler.c
* ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
* �Էµ� ������ �ڵ� ��, ���ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
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

#include "my_assembler.h"

/* -----------------------------------------------------------------------------------
* ���� : ����ڷ� ���� ������� ������ �޾Ƽ� ���ɾ��� OPCODE�� ã�� ����Ѵ�.
* �Ű� : ���� ����, ������� ����
* ��ȯ : ���� = 0, ���� = < 0
* ���� : ���� ������� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
*		   ���� �߰������� �������� �ʴ´�.
* -----------------------------------------------------------------------------------
*/


int main(int args, char *arg[])
{
	if (init_my_assembler()< 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0) {
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	if (assem_pass2() < 0) {
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	
	token_parsing(0);
	token_parsing(1);
//	print_opcode();
	//make_output("output") ; 
}
/* -----------------------------------------------------------------------------------
* ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
* �Ű� : ����4t
* ��ȯ : �������� = 0 , ���� �߻� = -1
* ���� : ������ ���ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
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
* ���� : ������� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
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
	for (int i = 0; i < line_num; i++) {
		token_parsing(i);
	}
	printf("\n");
	for (int i = 0; i < line_num; i++) {		
		//label ���
		if (token_table[i]->label[0] == '.')
			continue;
		else if (token_table[i]->label[0] != NULL)
			printf("%s	", token_table[i]->label);
		else
			printf("\t");

		//operator ���
		if (token_table[i]->operator[0] != NULL)
			printf("%s	", token_table[i]->operator);

		//operand ���
		if (token_table[i]->operand[0][0] != NULL) 
			printf("%s", token_table[i]->operand[0]);
		for (int j = 1; j < 3; j++) {
			if(token_table[i]->operand[j][0] != NULL)
				printf(", %s", token_table[i]->operand[j]);
		}
		printf("\t");
		//opcode ���
		int isop = search_opcode(token_table[i]->operator);
		if (isop >= 0) {
			printf("\t\t%02X", inst_table[isop]->opcode);
		}
		printf("\n");
	}
	return 0;
}

/* -----------------------------------------------------------------------------------
* ���� : ������� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ������� ���ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
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
		inst_table[i] = (struct inst_struct*)malloc(sizeof(inst_struct));
	}
	inst_index = 0;

	FILE *FILE = NULL;
	char str[128];	//���κ��� ���پ� ������ �ӽ� ����
	char *p_token = malloc(sizeof(char)*128);	//��ū��

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
				if (i == 0)	//int
					inst_table[inst_index]->format = atoi(p_token);	//format ����
				else if (i == 1) { //char
					inst_table[inst_index]->opcode = strtol(p_token, NULL,16);
				}
				else //int
					inst_table[inst_index]->ops = atoi(p_token);	//op ���� ����
			}
			inst_index++;
		}
	}
	else {
		printf("ERROR : ������ ���� ���Ͽ����ϴ�. ");
		return -1;
	}

		fclose(FILE);

		//printf("inst_table Ȯ�� ------- \n");
		//for (int i = 0; i < inst_index; i++) {			//inst_table �޸��Ҵ�
		//	printf("%s %d %X %d \n", inst_table[i]->operator, inst_table[i]->format, inst_table[i]->opcode, inst_table[i]->ops);
		//}

	return 0;
}

/* -----------------------------------------------------------------------------------
* ���� : ������� �� �ҽ��ڵ带 �о���� �Լ��̴�.
* �Ű� : ��������� �ҽ����ϸ�
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
	char str[128];
	char *p_token = malloc(sizeof(char)*128);
	
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
		printf("ERROR : ������ ���� ���Ͽ����ϴ�. ");
		return -1;
	}
	fclose(FILE);

//	printf("input_data Ȯ�� ------- \n");
//	for (int i = 0; i < line_num - 1; i++) {			//inst_table �޸��Ҵ�
//		printf("%s \n", input_data[i]);
//	}
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
	char *str = (char*)malloc(sizeof(char)*128);	//����
	char *p_token = (char*)malloc(sizeof(char)*50);		//��ū
	char *opbuf = (char*)malloc(30);				//operand ����
	char *op_token = (char*)malloc(10);				//operand ��ū
	strcpy(str, input_data[index]);
	
	
	
	//label �Ľ�
	if (str[0] == '.') {		//�ּ�ó��
		token_table[index]->label = (char*)malloc(strlen(p_token)+1);
		token_table[index]->label[0] = '.';
		return 0;
	}
	else if (str[0] != '\t') {	//ù��° ���ڰ� ���� �ƴ� ��� (���� ��쿣 label�� ����)
		token_table[index]->label = (char*)malloc(strlen(p_token)+1);	//label �޸��Ҵ�
		strcpy(p_token, strtok(str, "\t"));	//label 
		strcpy(token_table[index]->label, p_token);

	}
	
	//operator �Ľ�
	if (str[0] == '\t') {		//label�� ���� line�� ù��° ��ū�� operator�̹Ƿ� �켱ó��
		token_table[index]->label = (char*)malloc(strlen(p_token)+1);	//label �޸��Ҵ�
		token_table[index]->label[0] = '\0';
		strcpy(p_token, strtok(str, "\t"));		//operator
		token_table[index]->operator = (char*)malloc(strlen(p_token)+1);
		strcpy(token_table[index]->operator, p_token);
	}
	else {
		strcpy(p_token, strtok(NULL, "\t"));
		token_table[index]->operator = (char*)malloc(strlen(p_token)+1);
		strcpy(token_table[index]->operator, p_token);
	}
	if (strcmp(token_table[index]->operator, "RSUB") == 0) {	//RSUB�� ��� operand�� ���� comment�� �ִ� line�̹Ƿ� ����ó��
		strcpy(p_token, strtok(NULL, "\t"));
		token_table[index]->comment = (char*)malloc(strlen(p_token) + 1);
		strcpy(token_table[index]->comment, p_token);

		for (int i = 0; i < 3; i++) {		//�ʱ�ȭ
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
	if ( p_token != NULL) {			//comment�� ���� ���
			token_table[index]->comment = (char*)malloc(strlen(p_token)+1);
			strcpy(token_table[index]->comment, p_token);
	}
	else {							//comment�� ���� ���
		token_table[index]->comment = (char*)malloc(sizeof(p_token));
		token_table[index]->comment[0] = '\0';
	}

	//operand �Ľ�
	for (int i = 0; i < 3; i++) {		//�ʱ�ȭ
		token_table[index]->operand[i] = (char*)malloc(8);
		token_table[index]->operand[i][0] = '\0';
	}
	if (opbuf != NULL) {
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
	else{			//operand�� ���� ���	
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
		if (strcmp(inst_table[i]->operator, str) == 0)
			return i;
	}
	
	return -1;
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

void make_objectcode(char *file_name)
{
	/* add your code here */

}

void print_opcode() {
	printf("input_data Ȯ�� ------- \n");
	for (int i = 0; i < line_num - 1; i++) {			//inst_table �޸��Ҵ�
		printf("%s \n", input_data[i]);
	}
}

