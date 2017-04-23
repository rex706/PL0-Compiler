// Alec Psinakis
// COP3402 Spring 2017

#include "PL0.h"

// Find base L levels down
// l stand for L in the instruction format
int base(int l, int base, int stack[])
{
	int b1 = base;
	while (l > 0)
	{
		b1 = stack[b1 + 1];
		l--;
	}
	return b1;
}

// Print generated code in op format.
void printOpInstructions(instruction code[], int size)
{
	printf("Line\tOP\tR\tL\tM\n");

	int i;
	for (i = 0; i < size; i++)
	{
		// Print line number.
		printf("%d\t", i);

		// Print instruction from opcode.
		switch (code[i].op) {

		case ISA_LIT:
			printf("lit\t");
			break;

		case ISA_RTN:
			printf("rtn\t");
			break;

		case ISA_LOD:
			printf("lod\t");
			break;

		case ISA_STO:
			printf("sto\t");
			break;

		case ISA_CAL:
			printf("cal\t");
			break;

		case ISA_INC:
			printf("inc\t");
			break;

		case ISA_JMP:
			printf("jmp\t");
			break;

		case ISA_JPC:
			printf("jpc\t");
			break;

		case ISA_SIO1:
			printf("sio\t");
			break;

		case ISA_SIO2:
			printf("sio\t");
			break;

		case ISA_SIO3:
			printf("sio\t");
			break;

		case ISA_NEG:
			printf("neg\t");
			break;

		case ISA_ADD:
			printf("add\t");
			break;

		case ISA_SUB:
			printf("sub\t");
			break;

		case ISA_MUL:
			printf("mul\t");
			break;

		case ISA_DIV:
			printf("div\t");
			break;

		case ISA_ODD:
			printf("odd\t");
			break;

		case ISA_MOD:
			printf("mod\t");
			break;

		case ISA_EQL:
			printf("eql\t");
			break;

		case ISA_NEQ:
			printf("neq\t");
			break;

		case ISA_LSS:
			printf("lss\t");
			break;

		case ISA_LEQ:
			printf("leq\t");
			break;

		case ISA_GTR:
			printf("gtr\t");
			break;

		case ISA_GEQ:
			printf("geq\t");
			break;

		default:
			break;
		}

		// Print R, L, and M.
		printf("%d\t%d\t%d\n", code[i].r, code[i].l, code[i].m);
	}

	printf("\n");
}

int interpret(instruction code[], int print)
{
	// Initialize program counter, stack pointers, and the instruction register.
	int pc = 0;
	int bp = 1;
	int sp = 0;
	instruction ir;

	// Initialize stack.
	int *stack = calloc(MAX_STACK_HEIGHT, sizeof(int));

	// Initialize 16 registers to use.
	int *registers = calloc(16, sizeof(int));

	// Array used for saving the location of activation record separators.
	int *separators = calloc(MAX_STACK_HEIGHT, sizeof(int));

	// String for printing the op code.
	char* op = calloc(3, sizeof(char));

	if (print)
		printf("\t\t\t\t\tpc\tbp\tsp\n");

	int halt = 0, printReg = 0;
	while (1)
	{
		// Fetch instruction.
		ir = code[pc];

		// Increment program counter.
		pc++;

		// Print and execute instruction.
		switch (ir.op) {

		case ISA_LIT:
			strcpy(op, "lit");
			registers[ir.r] = ir.m;
			break;

		case ISA_RTN:
			strcpy(op, "rtn");
			sp = bp - 1;
			bp = stack[sp + 3];
			pc = stack[sp + 4];
			break;

		case ISA_LOD:
			strcpy(op, "lod");
			registers[ir.r] = stack[base(ir.l, bp, stack) + ir.m];
			break;

		case ISA_STO:
			strcpy(op, "sto");
			stack[base(ir.l, bp, stack) + ir.m] = registers[ir.r];
			break;

		case ISA_CAL:
			strcpy(op, "cal");
			stack[sp + 1] = 0;
			stack[sp + 2] = base(ir.l, bp, stack);
			stack[sp + 3] = bp;
			stack[sp + 4] = pc;
			bp = sp + 1;
			pc = ir.m;
			separators[bp] = 1;
			break;

		case ISA_INC:
			strcpy(op, "inc");
			sp = sp + ir.m;
			break;

		case ISA_JMP:
			strcpy(op, "jmp");;
			pc = ir.m;
			break;

		case ISA_JPC:
			strcpy(op, "jpc");
			if (registers[ir.r] == 0)
				pc = ir.m;
			break;

		case ISA_SIO1:
			strcpy(op, "sio");
			printReg = 1;
			break;

		case ISA_SIO2:
			strcpy(op, "sio");
			printf("Input: ");
			scanf("%d", &registers[ir.r]);
			break;

		case ISA_SIO3:
			strcpy(op, "sio");
			halt = 1;
			break;

		case ISA_NEG:
			strcpy(op, "neg");
			registers[ir.r] = registers[ir.l] * -1;
			break;

		case ISA_ADD:
			strcpy(op, "add");
			registers[ir.r] = registers[ir.l] + registers[ir.m];
			break;

		case ISA_SUB:
			strcpy(op, "sub");
			registers[ir.r] = registers[ir.l] - registers[ir.m];
			break;

		case ISA_MUL:
			strcpy(op, "mul");
			registers[ir.r] = registers[ir.l] * registers[ir.m];
			break;

		case ISA_DIV:
			strcpy(op, "div");
			registers[ir.r] = registers[ir.l] / registers[ir.m];
			break;

		case ISA_ODD:
			strcpy(op, "odd");
			registers[ir.r] = registers[ir.r] % 2;
			break;

		case ISA_MOD:
			strcpy(op, "mod");
			registers[ir.r] = registers[ir.l] % registers[ir.m];
			break;

		case ISA_EQL:
			strcpy(op, "eql");
			registers[ir.r] = registers[ir.l] == registers[ir.m];
			break;

		case ISA_NEQ:
			strcpy(op, "neq");
			registers[ir.r] = registers[ir.l] != registers[ir.m];
			break;

		case ISA_LSS:
			strcpy(op, "lss");
			registers[ir.r] = registers[ir.l] < registers[ir.m];
			break;

		case ISA_LEQ:
			strcpy(op, "leq");
			registers[ir.r] = registers[ir.l] <= registers[ir.m];
			break;

		case ISA_GTR:
			strcpy(op, "gtr");
			registers[ir.r] = registers[ir.l] > registers[ir.m];
			break;

		case ISA_GEQ:
			strcpy(op, "geq");
			registers[ir.r] = registers[ir.l] >= registers[ir.m];
			break;

		default:
			break;
		}

		if (print)
		{
			// Print op, R, L, M, pc, bp, sp)
			printf("%s\t%d\t%d\t%d\t%d\t%d\t%d\t", op, ir.r, ir.l, ir.m, pc, bp, sp);

			// Print out the stack.
			for (int i = 0; i <= sp; i++)
			{
				if (separators[i] == 1)
					printf("|\t");

				printf("%d\t", stack[i]);
			}
			printf("\n");

			printf("Reg\t");
			for (int i = 0; i < 16; i++)
			{
				printf("%d\t", registers[i]);
			}
			printf("\n\n");
		}

		// If SIO1 was the instruction and we need to print desired register.
		if (printReg == 1)
		{
			printf("Output: %d\n", registers[ir.r]);
			printReg = 0;
		}

		if (halt == 1)
			break;
	}

	// Free stack, register, and separator arrays from memory.
	free(stack);
	free(registers);
	free(separators);

	return 0;
}