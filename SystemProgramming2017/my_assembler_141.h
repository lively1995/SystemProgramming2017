/*
* my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
*
*/
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

/*
* instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
* 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
* 라인 별로 하나의 instruction을 저장한다.
*/
struct inst_struct {
	char *operator;
	int format;
	unsigned char opcode;
	int ops;
};
typedef struct inst_struct inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
* 어셈블리 할 소스코드를 토큰 단위로 관리하는 테이블이다.
* 관리 정보는 소스 라인 단위로 관리되어진다.
*/
char *input_data[MAX_LINES];
static int line_num;

int label_num;

/*
* 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
* operator는 renaming을 허용한다.
* nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
*/
struct token_unit {
	int Addr;
	char *label;
	char *operator;
	char *operand[MAX_OPERAND];
	char *comment;
	char nixbpe;
	unsigned int obcode;
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;



/*
* 심볼을 관리하는 구조체이다.
* 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
*/
struct symbol_unit {
	char symbol[10];
	int addr;
	int section;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];


/*
* literal을 관리하는 구조체이다.
* 리터럴 테이블은 리터럴 이름, 리터럴의 위치로 구성된다.
*/
struct literal_unit {
	char literal[10];
	int addr;
	char litdata[10];
	int size;
};
typedef struct literal_unit literal;
literal lit_table[MAX_LINES];
int litcnt;

static int locctr;

char *reg_table[9] = { "A", "X", "L", "B", "S", "T", "F", "PC", "SW" };  //레지스터 테이블
int sectaddr[3][2] = {-1,};  //섹션의 시작주소와 끝주소를 저장하는 배열
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
static int assem_pass1(void);
static int assem_pass2(void);
int search_inst(char *str);
int search_opcode(char *str);
int search_symbol(char *str);
int search_symbol2(char *str, int sect);
int search_literal(char *str);
void make_objectcode_output(char *file_name);
void my_print(int);
int isobcode(char* str);
int obcode_len(int isinst, int i);
int token_parsing(int index);