// Alec Psinakis
// COP3402 Spring 2017

#include "PL0.h"

void printSourceCode(char** lines, int size)
{
	// Print header.
	printf("Source Program:\n");

	// Regurgitate source code.
	for (int i = 0; i < size; i++)
	{
		printf("%s", lines[i]);
	}

	printf("\n\n");
}

int main(int argc, char **argv)
{
	// Initialize print command flags.
	int printSource = 0;
	int printLexemes = 0;
	int printAssembly = 0;
	int printVM = 0;

	// Open file from command line.
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		printf("Error: file pointer is null.");
		return 0;
	}

	// Check for argument commands.
	for (int i = 2; i < argc; i++)
	{
		if (strcmp(argv[i], "-l") == 0)
			printLexemes = 1;

		else if (strcmp(argv[i], "-a") == 0)
			printAssembly = 1;

		else if (strcmp(argv[i], "-v") == 0)
			printVM = 1;

		else if (strcmp(argv[i], "-s") == 0)
			printSource = 1;
	}

	// Initialize line counter. 
	int counter = 0;

	// Declare lines array to hold source code.
	char **lines = NULL;

	// Read information until the end of the file is reached.
	while (!feof(fp))
	{
		// If counter = 0, initialize lines array for the fist time.
		if (counter == 0)
			lines = (char**)calloc(counter + 1, sizeof(char*));

		// Reallocate lines to make room for more code.
		else
			lines = (char**)realloc(lines, (counter + 1) * sizeof(char*));

		// Allocate memory for line to be read.
		lines[counter] = (char*)calloc(MAX_STR_LENGTH, sizeof(char));

		// Read in line of code from file.
		fgets(lines[counter], MAX_STR_LENGTH, fp);

		// Increment line counter. 
		counter++;
	}

	// Print out the source code that was just read from the file.
	if (printSource == 1)
		printSourceCode(lines, counter);

	// Run Lexical Analysis to generate lexemes.
	lexeme* lexemes = analyzeCode(lines, counter, printLexemes);

	// Send lexemes to Parser/Code Generator to generate assembly code.
	instruction* code = parserCodeGen(lexemes, printAssembly);

	// Run generated assembly in the virtual machine.
	interpret(code, printVM);

	// Free dynamic variables.
	free(lines);
	free(lexemes);
	free(code);

	return 0;
}