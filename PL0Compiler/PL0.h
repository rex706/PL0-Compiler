// Alec Psinakis
// COP3402 Spring 2017

// Compiler for the PL/0 language with registers written in C.

// Error recovery not implemented.

#ifndef _PL0_H_
#define _PL0_H_

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STACK_HEIGHT 2000	// Maximum amount of items that can be in the stack at once during VM execution.
#define MAX_CODE_LENGTH 500		// Maximum amount of generated code allowed by the parser for a single program.
#define MAX_CHAR_LENGTH 11		// Maximum number of chars for identifiers.      
#define MAX_STR_LENGTH 256		// Maximum length of strings.
#define MAX_TABLE_SIZE 10000	// Maximum size for the symbol table.
#define MAX_DIGIT_LENGTH 5		// Maximum number of digits an assignment can have.  
#define MAX_LEXI_LEVELS 3		// Maximum number of lexical levels allowed. (May not be used)
#define RESERVED_AMOUNT 12		// Number of reserved words in the PL/0 language.

// An enumeration representing all the possible values of the ISA OPcode.
enum ISA_OPCODE_VALUE
{
	ISA_LIT = 1, // Loads a constant value (literal) M into Register R.	
	ISA_RTN = 2, // Returns from a subroutine and restore the caller environment.	
	ISA_LOD = 3, // Load value into a selected register from the stack location at offset M from L lexicographical levels down.
	ISA_STO = 4, // Store value from a selected register in the stack location at offset M from L lexicographical levels down.
	ISA_CAL = 5, // Call procedure at code index M (generates new Activation Record and pc <- M).
	ISA_INC = 6, // Allocate M locals (increment sp by M). First four are Functional Value, Static Link (SL), Dynamic Link (DL), and Return Address (RA).	
	ISA_JMP = 7, // Jump to instruction M.
	ISA_JPC = 8, // Jump to instruction M if R = 0.	
	ISA_SIO1 = 9, // Write a register to the screen.	
	ISA_SIO2 = 10, // Read in input from the user and store it in a register.	
	ISA_SIO3 = 11, // End of program(program stops running).	
	ISA_NEG = 12, // Negate operation.	
	ISA_ADD = 13, // Add two values together.	
	ISA_SUB = 14, // Subtract two values from each other.	
	ISA_MUL = 15, // Multiply two values together.	
	ISA_DIV = 16, // Divide two values.	
	ISA_ODD = 17, // Check if a certian value is odd.
	ISA_MOD = 18, // Mod of two values.
	ISA_EQL = 19, // Checks if two values are equal and puts the result in the output register.
	ISA_NEQ = 20, // Checks if two values are not equal. 1 if true, 0 otherwise.
	ISA_LSS = 21, // Checks if one value is less than another. 1 if true, 0 otherwise.
	ISA_LEQ = 22, // Less than or equal to. 1 if true, 0 otherwise.
	ISA_GTR = 23, // Greater than. 1 if true, 0 otherwise.
	ISA_GEQ = 24 // Greater than or equal to. 1 if true, 0 otherwise.
};

// Internal representation of PL/0 Symbols.
typedef enum {

	nulsym = 1,
	identsym = 2,
	numbersym = 3,
	plussym = 4,
	minussym = 5,
	multsym = 6,
	slashsym = 7,
	oddsym = 8,
	eqsym = 9,
	neqsym = 10,
	lessym = 11,
	leqsym = 12,
	gtrsym = 13,
	geqsym = 14,
	lparensym = 15,
	rparensym = 16,
	commasym = 17,
	semicolonsym = 18,
	periodsym = 19,
	becomessym = 20,
	beginsym = 21,
	endsym = 22,
	ifsym = 23,
	thensym = 24,
	whilesym = 25,
	dosym = 26,
	callsym = 27,
	constsym = 28,
	varsym = 29,
	procsym = 30,
	writesym = 31,
	readsym = 32,
	elsesym = 33

} token_type;

typedef struct {

	int count;	// Number of generated instructions will be stored in the frist index of the structure.
	int op;		// Opcode
	int r; 		// Register
	int l;		// Lexicographical level or a register in arithmetic and logic code R[j].
	int m;		// Depending on the operators it indicates:
				// - A number (code: LIT, INC).
				// - A program address(code: JMP, JPC, CAL).
				// - A data address(code: LOD, STO)
				// - A register in arithmetic and logic code R[k].
				// (e.g.ADD R[1], R[2], R[3])
} instruction;

typedef struct
{
	char lex[MAX_STR_LENGTH];	// Literal token representation.
	int type;					// Token type enum.
	int count;					// Number of tokens created. Stored in the fist index of struct.
} lexeme;

typedef struct
{
	int kind; 		// const = 1, var = 2, proc = 3
	char name[10];	// name up to 11 chars
	int val; 		// number (ASCII value) 
	int level; 		// L level
	int addr; 		// M address
	int init;		// 1 if variable has been assigned a value, 0 otherwise.
} symbol;

typedef struct
{
	int token;			// Current lexeme index / current token.
	int table;			// Current symbol table index.
	int stack;			// Current stack index.
	int reg;			// Current register index. 
	int instruction;	// Current instruction index / line number.
	int level;			// Current lexical level.
	int error;			// 1 if errors encountered, 0 otherwise.
} parser;

// List of reserved words in PL/0.
extern char *reserved[12];

// First sets for parser error recovery.
extern int blockFirst[8];
extern const int blockFirstSize;

extern int statementFirst[5];
extern const int statementFirstSize;

extern int conditionFirst[6];
extern const int conditionFirstSize;

extern int expressionFirst[5];
extern const int expressionFirstSize;

extern int termFirst[3];
extern const int termFirstSize;

extern int factorFirst[3];
const int factorFirstSize;

// Follow sets for parser error recovery.
extern int blockFollow[2];
extern const int blockFollowSize;

extern int statementFollow[3];
extern const int statementFollowSize;

extern int conditionFollow[2];
extern const int conditionFollowSize;

extern int expressionFollow[13];
extern const int expressionFollowSize;

extern int  termFollow[15];
extern const int termFollowSize;

extern int factorFollow[17];
extern const int factorFollowSize;

// Parser function declares.
void checkName(lexeme* lexemes, parser* p, char* name);
void initConst(symbol* table, instruction* code, parser* p);
void factor(lexeme* lexemes, symbol* table, instruction* code, parser *p);
void term(lexeme* lexemes, symbol* table, instruction* code, parser *p);
void expression(lexeme* lexemes, symbol* table, instruction* code, parser *p);
void condition(lexeme* lexemes, symbol* table, instruction* code, parser *p);
void statement(lexeme* lexemes, symbol* table, instruction* code, parser *p);
void block(lexeme* lexemes, symbol* table, instruction* code, parser *p);
void program(lexeme* lexemes, symbol* table, instruction* code, parser *p);
void gen(instruction* code, parser *p, int op, int r, int l, int m);
int findSymbol(symbol* table, parser *p, char* s);
void getToken(parser* p);
void error(int e);
void test(lexeme* lexemes, parser* p, int* set, int size);
instruction* parserCodeGen(lexeme* lexemes, int print);

// Lexer function declares.
void printTable(lexeme* table, int size);
void printList(lexeme* table, int size);
lexeme* analyzeCode(char** lines, int size, int printLexTable, int printLexList);

// VM function declares.
int base(int l, int base, int stack[]);
void printOpInstructions(instruction code[], int size);
int interpret(instruction code[], int printOp, int printExec);

// Compiler function declare.
void printSourceCode(char** lines, int size);

#endif