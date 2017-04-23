// Alec Psinakis
// COP3402 Spring 2017

#include "PL0.h"

// List of reserved keywords for the PL/0 language.
char *reserved[12] = { "const", "var", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd" };

// First sets for error recovery.
int blockFirst[8] = { constsym, varsym, procsym, identsym, ifsym, callsym, beginsym, whilesym };
const int blockFirstSize = 8;

int statementFirst[5] = { identsym, callsym, beginsym, ifsym, whilesym };
const int statementFirstSize = 5;

int conditionFirst[6] = { oddsym, plussym, minussym, lparensym, identsym, numbersym };
const int conditionFirstSize = 6;

int expressionFirst[5] = { plussym, minussym, lparensym, identsym, numbersym };
const int expressionFirstSize = 5;

int termFirst[3] = { identsym, numbersym, lparensym };
const int termFirstSize = 3;

int factorFirst[3] = { identsym, numbersym, lparensym };
const int factorFirstSize = 3;

// Follow sets for error recovery.
int blockFollow[2] = { periodsym, semicolonsym };
const int blockFollowSize = 2;

int statementFollow[3] = { periodsym, semicolonsym, endsym };
const int statementFollowSize = 3;

int conditionFollow[2] = { thensym, dosym };
const int conditionFollowSize = 2;

int expressionFollow[13] = { periodsym, semicolonsym, rparensym, endsym, thensym, dosym, eqsym, neqsym, rparensym, geqsym, gtrsym, leqsym, lessym };
const int expressionFollowSize = 13;

int termFollow[15] = { periodsym, semicolonsym, rparensym, plussym, minussym, endsym, thensym, dosym, eqsym, rparensym, geqsym, gtrsym, leqsym, lessym };
const int termFollowSize = 15;

int factorFollow[17] = { periodsym, semicolonsym, rparensym, plussym, minussym, multsym, slashsym , endsym, thensym, dosym, eqsym, rparensym, geqsym, gtrsym, leqsym, lessym };
const int factorFollowSize = 17;

void factor(lexeme* lexemes, symbol* table, instruction* code, parser *p)
{
	if (lexemes[p->token].type == identsym)
	{
		// Find corresponding symbol in the symbol table.
		int i = findSymbol(table, p, lexemes[p->token].lex);

		if (i == -1)
		{
			error(11);
		}

		// Get number of levels down needed.
		int level = p->level - table[i].level;

		// Constant
		if (table[i].kind == 1)
		{
			gen(code, p, ISA_LOD, p->reg, level, table[i].addr);
			p->reg++;
		}
		// Variable
		else if (table[i].kind == 2)
		{
			// Check to make sure variable has been assigned a value before it is used in an operation.
			// Does not work with procedures.
			/*if (table[i].init == 0)
			{
				error(30);
			}*/

			gen(code, p, ISA_LOD, p->reg, level, table[i].addr);
			p->reg++;
		}
		else
		{
			error(21);
		}

		getToken(p);
	}
	else if (lexemes[p->token].type == numbersym)
	{
		gen(code, p, ISA_LIT, p->reg, 0, atoi(lexemes[p->token].lex));
		p->reg++;
		getToken(p);
	}
	else if (lexemes[p->token].type == lparensym)
	{
		getToken(p);
		expression(lexemes, table, code, p);
		if (lexemes[p->token].type != rparensym)
		{
			error(22);
		}
		getToken(p);
	}
	else
	{
		error(23);
	}

	if (p->reg >= 16)
	{
		error(32);
	}
}

void term(lexeme* lexemes, symbol* table, instruction* code, parser *p)
{
	factor(lexemes, table, code, p);
	while (lexemes[p->token].type == multsym || lexemes[p->token].type == slashsym)
	{
		// Save symbol to know which operation to generate later.
		int mathToken = lexemes[p->token].type;

		getToken(p);
		factor(lexemes, table, code, p);

		if (mathToken == multsym)
			gen(code, p, ISA_MUL, p->reg - 2, p->reg - 2, p->reg - 1);
		else
			gen(code, p, ISA_DIV, p->reg - 2, p->reg - 2, p->reg - 1);

		p->reg--;
	}
}

void expression(lexeme* lexemes, symbol* table, instruction* code, parser *p)
{
	if (lexemes[p->token].type == plussym || lexemes[p->token].type == minussym)
	{
		// Save token to be checked for negation later.
		int symbol = lexemes[p->token].type;

		getToken(p);
		term(lexemes, table, code, p);

		// If the saved symbol was a minus sign, generate negation instruction.
		if (symbol == minussym)
			gen(code, p, ISA_NEG, p->reg - 1, p->reg - 1, 0);
	}
	else
	{
		term(lexemes, table, code, p);
	}

	while (lexemes[p->token].type == plussym || lexemes[p->token].type == minussym)
	{
		// Save symbol to know which operation instruction to generate later.
		int symbol = lexemes[p->token].type;

		getToken(p);
		term(lexemes, table, code, p);

		if (symbol == plussym)
			gen(code, p, ISA_ADD, p->reg - 2, p->reg - 2, p->reg - 1);
		else
			gen(code, p, ISA_SUB, p->reg - 2, p->reg - 2, p->reg - 1);

		p->reg--;
	}
}

void condition(lexeme* lexemes, symbol* table, instruction* code, parser *p)
{
	if (lexemes[p->token].type == oddsym)
	{
		getToken(p);
		expression(lexemes, table, code, p);
		gen(code, p, ISA_ODD, p->reg - 1, 0, 0);
	}
	else
	{
		expression(lexemes, table, code, p);
		if (lexemes[p->token].type != eqsym && lexemes[p->token].type != neqsym && lexemes[p->token].type != lessym && lexemes[p->token].type != leqsym
			&& lexemes[p->token].type != gtrsym && lexemes[p->token].type != geqsym)
		{
			error(20);
		}

		int op;
		switch (lexemes[p->token].type)
		{
			case eqsym:
				op = ISA_EQL;
				break;

			case neqsym:
				op = ISA_NEQ;
				break;

			case lessym:
				op = ISA_LSS;
				break;

			case leqsym:
				op = ISA_LEQ;
				break;

			case gtrsym:
				op = ISA_GTR;
				break;

			case geqsym:
				op = ISA_GEQ;
				break;
		}

		getToken(p);
		expression(lexemes, table, code, p);

		// Generate relational operation code after expression for the operation that has been evaluated.
		gen(code, p, op, p->reg - 2, p->reg - 2, p->reg - 1);
		p->reg--;
	}
}

void statement(lexeme* lexemes, symbol* table, instruction* code, parser *p)
{
	if (lexemes[p->token].type == identsym)
	{
		// Find the index for the corresponding indentifier in the symbol table.
		int i = findSymbol(table, p, lexemes[p->token].lex);

		if (i == -1)
		{
			error(11);
		}
		if (table[i].kind != 2)
		{
			error(12);
		}

		// Get number of levels down needed.
		int level = p->level - table[i].level;

		getToken(p);
		if (lexemes[p->token].type != becomessym)
		{
			if (lexemes[p->token].type == eqsym)
			{
				error(1);
			}
			else
			{
				error(3);
			}
		}
		getToken(p);
		expression(lexemes, table, code, p);

		// Generate store instruction:
		// Store value from selected register into the stack L levels down at offset M.
		gen(code, p, ISA_STO, p->reg - 1, level, table[i].addr);
		p->reg--;

		// Variable has been assigned a value.
		table[i].init = 1;
	}
	else if (lexemes[p->token].type == callsym)
	{
		getToken(p);
		if (lexemes[p->token].type != identsym)
		{
			error(14);
		}

		// Find the index for the corresponding indentifier in the symbol table.
		int i = findSymbol(table, p, lexemes[p->token].lex);

		if (i == -1)
		{
			error(11);
		}
		if (table[i].kind != 3)
		{
			error(15);
		}

		// Get number of levels down needed.
		int level = p->level - table[i].level;

 		gen(code, p, ISA_CAL, 0, level, table[i].addr);
		getToken(p);

	}
	else if (lexemes[p->token].type == beginsym)
	{
		getToken(p);
		statement(lexemes, table, code, p);

		while (lexemes[p->token].type == semicolonsym)
		{
			getToken(p);
			statement(lexemes, table, code, p);
		}
		if (lexemes[p->token].type != endsym)
		{
			error(19);
		}
		getToken(p);
	}
	else if (lexemes[p->token].type == ifsym)
	{
		getToken(p);
		condition(lexemes, table, code, p);

		// Save line number and generate if jump statement so jump address can be updated after statement returns with the correct jump address.
		int conditionLineNum = p->instruction;
		gen(code, p, ISA_JPC, p->reg - 1, 0, 0);

		if (lexemes[p->token].type != thensym)
		{
			error(16);
		}

		// jump into else if the 'if' condition is not met. If it is, jump passed else at the end of 'then' statement.

		getToken(p);
		statement(lexemes, table, code, p);

		// Update jump instruction with accurate line number to jump to after generating code for its respective body statement(s).
		code[conditionLineNum].m = p->instruction;

		if (lexemes[p->token].type == elsesym)
		{
			// Save instruciton line number and generate jump instruction to jump passed else if an else statement exists.
			int elseJump = p->instruction;
			gen(code, p, ISA_JMP, 0, 0, 0);

			// Make sure 'if' jumps passed 'else's' jump statement so a loop does not occur.
			code[conditionLineNum].m += 1;

			getToken(p);
			statement(lexemes, table, code, p);
			
			// Update instruction to jump passed else after statement has been generated.
			code[elseJump].m = p->instruction;
		}
	}
	else if (lexemes[p->token].type == whilesym)
	{
		// Save location to jump back to the beginning of the while loop.
		int jumpBack = p->instruction;

		getToken(p);
		condition (lexemes, table, code, p);

		// Save line number and generate while jump statement so jump address can be updated after statement returns with the correct jump address.
		int conditionLineNum = p->instruction;
		gen(code, p, ISA_JPC, p->reg - 1, 0, 0);

		if (lexemes[p->token].type != dosym)
		{
			error(18);
		}
		getToken(p);
		statement(lexemes, table, code, p);

		// Update while jump instruction with accurate line number to jump to after generating code for its respective body statement.
		code[conditionLineNum].m = p->instruction + 1;

		// Generate jump instruction to go back to the beginning of the while loop.
		gen(code, p, ISA_JMP, 0, 0, jumpBack);
	}
	else if (lexemes[p->token].type == readsym)
	{
		getToken(p);
		if (lexemes[p->token].type != identsym)
		{
			error(26);
		}

		// Find symbol in the symbol table.
		int i = findSymbol(table, p, lexemes[p->token].lex);

		if (i == -1)
		{
			error(11);
		}
		if (table[i].kind != 2)
		{
			error(12);
		}

		// Get number of levels down needed.
		int level = p->level - table[i].level;

		getToken(p);
		
		// Generate code to read in a value from the user and store it in it's respective variable.
		gen(code, p, ISA_SIO2, p->reg, 0, 2);
		gen(code, p, ISA_STO, p->reg, level, table[i].addr);

		// Variable has been assigned a value.
		table[i].init = 1;
	}
	else if (lexemes[p->token].type == writesym)
	{
		getToken(p);
		if (lexemes[p->token].type != identsym)
		{
			error(27);
		}

		// Find corresponding symbol in the symbol table.
		int i = findSymbol(table, p, lexemes[p->token].lex);

		if (i == -1)
		{
			error(11);
		}

		// Get number of levels down needed.
		int level = p->level - table[i].level;

		getToken(p);

		// Generate code to load a value from a register and print it to the screen.
		gen(code, p, ISA_LOD, p->reg, level, table[i].addr);
		gen(code, p, ISA_SIO1, p->reg, 0, 1);
	}
}

void block(lexeme* lexemes, symbol* table, instruction* code, parser *p)
{
	// Increment lexical level.
	p->level++;

	// Allocate space for the first four locals in the activation record.
	// (Functional Value, Static Link (SL), Dynamic Link (DL), and Return Address (RA))
	p->stack = 4;

	// Counter for the number of locals encountered starting with the first four.
	int localCount = 4;

	// Flag for determining if one or more constants have been declared and code must be generated to assign values.
	int constDeclare = 0;

	if (lexemes[p->token].type == constsym)
	{
		do 
		{
			// Increment locals counter.
			localCount++;

			// Constant is being declared.
			constDeclare = 1;

			// Add constant to symbol table. 
			table[p->table].kind = 1;
			getToken(p);
			if (lexemes[p->token].type != identsym)
			{
				error(4);
			}

			// Get constant name if valid.
			checkName(lexemes, p, lexemes[p->token].lex);
			strcpy(table[p->table].name, lexemes[p->token].lex);

			getToken(p);
			if (lexemes[p->token].type != eqsym)
			{
				error(3);
			}

			getToken(p);
			if (lexemes[p->token].type != numbersym)
			{
				error(2);
			}

			// Get constant value.
			table[p->table].val = atoi(lexemes[p->token].lex);

			// Get the constant's address in the stack.
			table[p->table].addr = p->stack;

			// Get lexical level constant is being declared on.
			table[p->table].level = p->level;

			getToken(p);
			p->table++;
			p->stack++;

		} while (lexemes[p->token].type == commasym);

		if (lexemes[p->token].type != semicolonsym)
		{
			error(5);
		}
		getToken(p);
	}
	if (lexemes[p->token].type == varsym)
	{
		do 
		{
			localCount++;
			table[p->table].kind = 2;
			getToken(p);

			if (lexemes[p->token].type != identsym)
			{
				error(4);
			}

			// Put variable name into the symbol table if valid.
			checkName(lexemes, p, lexemes[p->token].lex);
			strcpy(table[p->table].name, lexemes[p->token].lex);

			// Get variable address in the stack and add it to the symbol table.
			table[p->table].addr = p->stack;

			// Get lexical level var is being declared on.
			table[p->table].level = p->level;

			getToken(p);
			p->table++;
			p->stack++;

		} while (lexemes[p->token].type == commasym);

		if (lexemes[p->token].type != semicolonsym)
		{
			error(5);
		}
		getToken(p);
	}

	// Generate instruction to allocate space for specified number of locals.	
	gen(code, p, ISA_INC, 0, 0, localCount);

	// Generate code to assign values to constants declared on this lexical level.
	if (constDeclare == 1)
		initConst(table, code, p);

	int procJump = -1;

	while (lexemes[p->token].type == procsym)
	{
		// Save instruction address for the first found procedure in this block to update
		// the generated jump instruction's jump address later.
		if (procJump == -1)
		{
			procJump = p->instruction;

			// Generate instruction to jump to where procedure execution begins.
			// Update M later when its block starts.
			gen(code, p, ISA_JMP, 0, 0, 0);
		}
		
		table[p->table].kind = 3;
		getToken(p);

		if (lexemes[p->token].type != identsym)
		{
			error(4);
		}
		// Get procedure name if valid.
		checkName(lexemes, p, lexemes[p->token].lex);
		strcpy(table[p->table].name, lexemes[p->token].lex);

		// Get lexical level procedure is being declared on.
		table[p->table].level = p->level;

		// Save address to jump to when procedure is called.
		table[p->table].addr = p->instruction;

		getToken(p);
		if (lexemes[p->token].type != semicolonsym)
		{
			error(5);
		}

		p->table++;
		getToken(p);
		
		// Start a new block.
		block(lexemes, table, code, p);

		if (lexemes[p->token].type != semicolonsym)
		{
			error(5);
		}
		getToken(p);

		// Generate return instruction after procedure is finished.
		gen(code, p, ISA_RTN, 0, 0, 0);
	}

	// Update jump instruction jump addresses.
	if (procJump != -1)
		code[procJump].m = p->instruction;	

	statement(lexemes, table, code, p);

	// Decrement lexical level as block is over.
	p->level--;
}

void program(lexeme* lexemes, symbol* table, instruction* code, parser *p)
{
	getToken(p);

	block(lexemes, table, code, p);

	if (lexemes[p->token].type != periodsym)
	{
		error(9);
	}

	// Generate instruction to indicate the program has ended.
	gen(code, p, ISA_SIO3, 0, 0, 3);
}

void initConst(symbol* table, instruction* code, parser* p)
{
	for (int i = 0; i <= p->stack; i++)
	{
		if (table[i].kind == 1 && table[i].init == 0)
		{
			table[i].init = 1;

			// Generate code to assign value to constant.
			gen(code, p, ISA_LIT, p->reg, 0, table[i].val);
			gen(code, p, ISA_STO, p->reg, 0, table[i].addr);
		}
	}
}

void printGeneratedAssembly(instruction* code, int size)
{
	printf("Generated Assembly:\n");

	for (int i = 0; i < size; i++)
	{
		printf("%d %d %d %d\n", code[i].op, code[i].r, code[i].l, code[i].m);
	}
	printf("\n");
}

int findSymbol(symbol* table, parser *p, char* s)
{
	// Look up symbol in the symbol table starting from the current lexi level, working downward.
	for (int j = p->level; j >= 0; j--)
	{
		for (int i = 0; i <= p->table; i++)
		{
			// If the lexical level and symbol names match, return the found index.
			if (table[i].level == j && strcmp(table[i].name, s) == 0)
				return i;
		}
	}

	return -1;
}

void getToken(parser* p) 
{
	p->token++;
}

void checkName(lexeme* lexemes, parser* p, char* name)
{
	for (int i = 0; i < RESERVED_AMOUNT; i++)
	{
		// Check to make sure a reserved word is not being used as an identifier name.
		if (strcmp(name, reserved[i]) == 0)
		{
			error(31);
		}
	}
}

void gen(instruction* code, parser *p, int op, int r, int l, int m)
{
	code[p->instruction].op = op;
	code[p->instruction].r = r;
	code[p->instruction].l = l;
	code[p->instruction].m = m;

	p->instruction++;
}

void error(int e) {

	switch (e) {

		case 1:
			printf("Error 1: Use '=' instead of ':='.\n");
			break;

		case 2:
			printf("Error 2: = must be followed by a number.\n");
			break;

		case 3:
			printf("Error 3: Identifier must be followed by '='.\n");
			break;

		case 4:
			printf("Error 4: const, var, procedure must be followed by identifier.\n");
			break;

		case 5:
			printf("Error 5: Semicolon or comma missing.\n");
			break;

		case 6:
			printf("Error 6: Incorrect symbol after procedure declaration.\n");
			break;

		case 7:
			printf("Error 7: Statement expected.\n");
			break;

		case 8:
			printf("Error 8: Incorrect symbol after statement part in block.\n");
			break;

		case 9:
			printf("Error 9: Period expected.\n");
			break;

		case 10:
			printf("Error 10: Semicolon between statements missing.\n");

		case 11:
			printf("Error 11: Undeclared identifier.\n");
			break;

		case 12:
			printf("Error 12: Assignment to constant or procedure is not allowed.\n");
			break;

		case 13:
			printf("Error 13: Assignment operator expected.\n");

		case 14:
			printf("Error 14: call must be followed by an identifier.\n");
			break;

		case 15:
			printf("Error 15: Call of a constant or variable is meaningless.\n");
			break;

		case 16:
			printf("Error 16: 'then' expected.\n");
			break;

		case 17:
			printf("Error 17: Semicolon or '}' expected.\n");
			break;

		case 18:
			printf("Error 18: 'do' expected.\n");
			break;

		case 19:
			printf("Error 19: Incorrect symbol following statement.\n");
			break;

		case 20:
			printf("Error 20: Relational operator expected.\n");
			break;

		case 21:
			printf("Error 21: Expression must not contain a procedure identifier.\n");
			break;

		case 22:
			printf("Error 22: Right parenthesis missing.\n");
			break;

		case 23:
			printf("Error 23: The preceding factor cannot begin with this symbol.\n");
			break;

		case 24:
			printf("Error 24: An expression cannot begin with this symbol.\n");
			break;

		case 26:
			printf("Error 26: Identifier must follow read.\n");
			break;

		case 27:
			printf("Error 27: Identifier must follow write.\n");
			break;

		case 30:
			printf("Error 30: Use of an unassigned variable.\n");
			break;

		case 31:
			printf("Error 31: Reserved keyword cannot be used as an identifier name.\n");
			break;

		case 32:
			// Probably unrecoverable.
			printf("Error 32: All registers already in use.\n");
			break;

		case 33:
			printf("Error 33: Token not in FIRST set.\n");
			return;

		case 34:
			printf("Error 34: Token not in FOLLOW set.\n");
			return;

		default:
			break;
	}

	exit(1);
}

void test(lexeme* lexemes, parser* p, int* set, int size)
{
	// Get the number of tokens from the first lexeme index.
	int tokenAmt = lexemes[0].count;

	if (size == 0)
		return;

	while (p->token <= tokenAmt)
	{
		for (int i = 0; i < size; i++)
		{
			// If token is in the set, return to parsing.
			if (lexemes[p->token].type == set[i])
				return;
		}

		getToken(p);
	}

	// Reached end of file, no more lexemes to parse.
	printf("Error 33: EOF found early.\n");
	exit(1);
}

instruction* parserCodeGen(lexeme* lexemes, int print)
{
	// Allocate memory for the symbol table. 
	symbol* table = calloc(MAX_TABLE_SIZE, sizeof(symbol));

	// Initialize struct to store generated code.
	instruction* code = calloc(MAX_CODE_LENGTH, sizeof(instruction));

	// Intitiliaze parser struct to hold parser for the lexemes, table, code, stack, and registers.
	parser p;
	p.instruction = 0;
	p.reg = 0;
	p.stack = 0;
	p.table = 0;
	p.token = -1;
	p.level = -1;
	p.error = 0;

	// Begin recursive descent parser and generate code in-line.
	program(lexemes, table, code, &p);

	if (p.error == 0)
		printf("No errors, program is syntactically correct.\n\n");
	else
		printf("\n");

	// Print generated assembly code if the '-a' command was used.
	if (print)
		printGeneratedAssembly(code, p.instruction);

	return code;
}