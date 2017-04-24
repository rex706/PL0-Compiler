// Alec Psinakis
// COP3402 Spring 2017

#include "PL0.h"

void printTable(lexeme* table, int size)
{
	int i;

	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");

	for (i = 0; i < size; i++)
	{
		printf("%s\t\t%d\n", table[i].lex, table[i].type);
	}

	printf("\n");
}	

void printList(lexeme* table, int size)
{
	int i;

	printf("Lexeme List:\n");

	for (i = 0; i < size; i++)
	{
		if (table[i].type == 2 || table[i].type == 3)
			printf("%d %s ", table[i].type, table[i].lex);
		else
			printf("%d ", table[i].type);
	}

	printf("\n\n");
}

lexeme* analyzeCode(char** lines, int lineCount, int printLexTable, int printLexList)
{
	// Lexeme table index.
	int k = 0;

	// Allocate memory for lexeme table.
	lexeme* table = calloc(MAX_TABLE_SIZE, sizeof(lexeme));

	// If comment begin symbol is encountered, value will be set to 1. 
	int comment = 0;

	for (int i = 0; i < lineCount; i++)
	{
		// Save the length of the current line.
		int lineLength = strlen(lines[i]);

		for (int j = 0; j < lineLength; j++)
		{
			// While comment mode is enabled, ignore characters until the end comment symbol has been reached.
			while (comment == 1)
			{
				// If there are no more characters on this line, move to the next one.
				if (j >= lineLength)
				{
					// Move to the next line.
					i++;

					// If there are no more lines to tokenize, comment was never closed.
					if (i >= lineCount)
					{
						printf("Error 35 (Unrecoverable): Comment never closed.");
						exit(1);
					}

					// Save the length of the new current line.
					lineLength = strlen(lines[i]);

					// Reset j to start from the beginning of the new line.
					j = 0;
				}

				if (lines[i][j] == '*' && lines[i][j + 1] == '/')
				{
					// Disable comment mode.
					comment = 0;

					// Move j to the end of the comment symbol.
					j += 1;
				}
				j++;
			}

			// Ignore invisible symbols.
			if (isspace(lines[i][j]) || lines[i][j] == '\0')
			{
				// Characters covered by the isspace() function.
				// ' '	(0x20)	space(SPC)
				// '\t'	(0x09)	horizontal tab(TAB)
				// '\n'	(0x0a)	newline(LF)
				// '\v'	(0x0b)	vertical tab(VT)
				// '\f'	(0x0c)	feed(FF)
				// '\r'	(0x0d)	carriage return (CR)
			}
			// begin
			else if (lines[i][j] == 'b' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 5 <= lineLength && isspace(lines[i][j + 5])
				&& lines[i][j + 1] == 'e' && lines[i][j + 2] == 'g' && lines[i][j + 3] == 'i' && lines[i][j + 4] == 'n')
			{
				strcpy(table[k].lex, "begin");
				table[k].type = beginsym;
				k++;

				// Move j to the empty space.
				j += 5;
			}
			// const
			else if (lines[i][j] == 'c' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 5 <= lineLength && isspace(lines[i][j + 5]) &&
				lines[i][j + 1] == 'o' && lines[i][j + 2] == 'n' && lines[i][j + 3] == 's' && lines[i][j + 4] == 't')
			{
				// Add const to lexeme table.
				strcpy(table[k].lex, "const");
				table[k].type = constsym;
				k++;

				// Move j to the empty space.
				j += 5;
			}
			// var
			else if (lines[i][j] == 'v' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 3 < lineLength && isspace(lines[i][j + 3]) && lines[i][j + 1] == 'a'
				&& lines[i][j + 2] == 'r')
			{
				// Add var to lexeme table.
				strcpy(table[k].lex, "var");
				table[k].type = varsym;
				k++;

				// Move j to the empty space.
				j += 3;
			}
			// procedure
			else if (lines[i][j] == 'p' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 9 < lineLength && isspace(lines[i][j + 9]) && lines[i][j + 1] == 'r' &&
				lines[i][j + 2] == 'o' && lines[i][j + 3] == 'c' && lines[i][j + 4] == 'e' && lines[i][j + 5] == 'd' && lines[i][j + 6] == 'u'
				&& lines[i][j + 7] == 'r' && lines[i][j + 8] == 'e')
			{
				strcpy(table[k].lex, "proc");
				table[k].type = procsym;
				k++;

				// Move j to the empty space.
				j += 9;
			}
			// call
			else if (lines[i][j] == 'c' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 4 <= lineLength && isspace(lines[i][j + 4]) &&
				lines[i][j + 1] == 'a' && lines[i][j + 2] == 'l' && lines[i][j + 3] == 'l')
			{
				strcpy(table[k].lex, "call");
				table[k].type = callsym;
				k++;

				// Move j to the empty space.
				j += 4;
			}
			// end
			else if (lines[i][j] == 'e' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 3 <= lineLength &&
				(lines[i][j + 3] == '.' || lines[i][j + 3] == ';' || isspace(lines[i][j + 3])) && lines[i][j + 1] == 'n' && lines[i][j + 2] == 'd')
			{
				strcpy(table[k].lex, "end");
				table[k].type = endsym;
				k++;

				// Move j to the end of the symbol.
				j += 2;
			}
			// else
			else if (lines[i][j] == 'e' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 4 < lineLength &&
				isspace(lines[i][j + 4]) && lines[i][j + 1] == 'l' && lines[i][j + 2] == 's' && lines[i][j + 3] == 'e')
			{
				strcpy(table[k].lex, "else");
				table[k].type = elsesym;
				k++;

				// Move j to the empty space.
				j += 4;
			}
			// if
			else if (lines[i][j] == 'i' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 2 < lineLength && isspace(lines[i][j + 2]) && lines[i][j + 1] == 'f')
			{
				strcpy(table[k].lex, "if");
				table[k].type = ifsym;
				k++;

				// Move j to the empty space.
				j += 2;
			}
			// then
			else if (lines[i][j] == 't' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 4 < lineLength &&
				isspace(lines[i][j + 4]) && lines[i][j + 1] == 'h' && lines[i][j + 2] == 'e' && lines[i][j + 3] == 'n')
			{
				strcpy(table[k].lex, "then");
				table[k].type = thensym;
				k++;

				// Move j to the empty space.
				j += 4;
			}
			// odd
			else if (lines[i][j] == 'o' && (j - 1 < 0 || lines[i][j - 1] == ' ') && j + 3 < lineLength && isspace(lines[i][j + 3]) && lines[i][j + 1] == 'd' &&lines[i][j + 2] == 'd')
			{
				strcpy(table[k].lex, "odd");
				table[k].type = oddsym;
				k++;

				// Move j to the empty space.
				j += 3;
			}
			// while
			else if (lines[i][j] == 'w' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 5 < lineLength && isspace(lines[i][j + 5]) && lines[i][j + 1] == 'h'
				&& lines[i][j + 2] == 'i' && lines[i][j + 3] == 'l' && lines[i][j + 4] == 'e')
			{
				// Add while to lexeme table.
				strcpy(table[k].lex, "while");
				table[k].type = whilesym;
				k++;

				// Move j to the empty space.
				j += 5;
			}
			//do
			else if (lines[i][j] == 'd' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 2 < lineLength && isspace(lines[i][j + 2])
				&& lines[i][j + 1] == 'o')
			{
				// Add while to lexeme table.
				strcpy(table[k].lex, "do");
				table[k].type = dosym;
				k++;

				// Move j to the empty space.
				j += 2;
			}
			// read
			else if (lines[i][j] == 'r' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 4 < lineLength && isspace(lines[i][j + 4]) && lines[i][j + 1] == 'e'
				&& lines[i][j + 2] == 'a'&& lines[i][j + 3] == 'd')
			{
				// Add while to lexeme table.
				strcpy(table[k].lex, "read");
				table[k].type = readsym;
				k++;

				// Move j to the empty space.
				j += 4;
			}
			// write
			else if (lines[i][j] == 'w' && (j - 1 < 0 || isspace(lines[i][j - 1])) && j + 5 < lineLength && isspace(lines[i][j + 5]) && lines[i][j + 1] == 'r'
				&& lines[i][j + 2] == 'i' && lines[i][j + 3] == 't' && lines[i][j + 4] == 'e')
			{
				// Add while to lexeme table.
				strcpy(table[k].lex, "write");
				table[k].type = writesym;
				k++;

				// Move j to the empty space.
				j += 5;
			}
			// Assignment :=
			else if (lines[i][j] == ':' && j + 1 < lineLength && lines[i][j + 1] == '=')
			{
				strcpy(table[k].lex, ":=");
				table[k].type = becomessym;
				k++;

				// Move j to the end of the symbol.
				j += 1;
			}
			else if (lines[i][j] == ';')
			{
				strcpy(table[k].lex, ";");
				table[k].type = semicolonsym;
				k++;
			}
			else if (lines[i][j] == '.')
			{
				strcpy(table[k].lex, ".");
				table[k].type = periodsym;
				k++;
			}
			else if (lines[i][j] == ',')
			{
				strcpy(table[k].lex, ",");
				table[k].type = commasym;
				k++;
			}
			else if (lines[i][j] == '+')
			{
				strcpy(table[k].lex, "+");
				table[k].type = plussym;
				k++;
			}
			else if (lines[i][j] == '-')
			{
				strcpy(table[k].lex, "-");
				table[k].type = minussym;
				k++;
			}
			else if (lines[i][j] == '*')
			{
				if (j + 1 < lineLength && lines[i][j + 1] == '/')
				{
					printf("Error 34 (Unrecoverable): End comment symbol but comment was never started on line %d.", i);
					exit(1);
				}

				strcpy(table[k].lex, "*");
				table[k].type = multsym;
				k++;
			}
			else if (lines[i][j] == '/')
			{
				if (j + 1 < lineLength && lines[i][j + 1] == '*')
				{
					// Enable comment mode.
					comment = 1;

					// Move j to the end of the comment symbol.
					j += 1;
				}
				else
				{
					strcpy(table[k].lex, "/");
					table[k].type = slashsym;
					k++;
				}
			}
			else if (lines[i][j] == '(')
			{
				strcpy(table[k].lex, "(");
				table[k].type = lparensym;
				k++;
			}
			else if (lines[i][j] == ')')
			{
				strcpy(table[k].lex, ")");
				table[k].type = rparensym;
				k++;
			}
			else if (lines[i][j] == '=')
			{
				strcpy(table[k].lex, "=");
				table[k].type = eqsym;
				k++;
			}
			// less than
			else if (lines[i][j] == '<')
			{
				// Less than or equal to.
				if (j + 1 < lineLength && lines[i][j + 1] == '=')
				{
					strcpy(table[k].lex, "<=");
					table[k].type = leqsym;

					// Move j to the end of the symbol.
					j += 1;
				}
				// Not equal.
				else if (j + 1 < lineLength && lines[i][j + 1] == '>')
				{
					strcpy(table[k].lex, "<>");
					table[k].type = neqsym;

					// Move j to the end of the symbol.
					j += 1;
				}
				else
				{
					strcpy(table[k].lex, "<");
					table[k].type = lessym;
				}

				k++;
			}
			// greater than
			else if (lines[i][j] == '>')
			{
				// Greater than or equal to.
				if (j + 1 < lineLength && lines[i][j + 1] == '=')
				{
					strcpy(table[k].lex, ">=");
					table[k].type = geqsym;

					// Move j to the end of the symbol.
					j += 1;
				}
				else
				{
					strcpy(table[k].lex, ">");
					table[k].type = gtrsym;
				}

				k++;
			}
			// Get identifier name.
			else if (isalpha(lines[i][j]))
			{
				// Temp string.
				char* s = calloc(MAX_STR_LENGTH, sizeof(char));
				table[k].type = identsym;

				while (j <= lineLength && (isalpha(lines[i][j]) || isdigit(lines[i][j])))
				{
					// Check for valid name length.
					if (strlen(s) >= MAX_CHAR_LENGTH)
					{
						printf("Error 33 (Unrecoverable): Identifier name is too long: %s\nMax length: %d", s, MAX_CHAR_LENGTH);
						exit(1);
					}

					strncat(s, &lines[i][j], 1);
					j++;
				}
				j--;

				strcpy(table[k].lex, s);
				free(s);
				k++;
			}
			// Get number.
			else if (isdigit(lines[i][j]))
			{
				// Temp string.
				char* s = calloc(MAX_STR_LENGTH, sizeof(char));
				table[k].type = numbersym;

				while (j <= lineLength && isdigit(lines[i][j]))
				{
					// Check for valid digit amount.
					if (strlen(s) >= MAX_DIGIT_LENGTH)
					{
						printf("Error 25 (Unrecoverable): Number is too large: %s\nCannot have more than %d digits.", s, MAX_DIGIT_LENGTH);
						exit(1);
					}

					strncat(s, &lines[i][j], 1);
					j++;
				}
				//if (isalpha(lines[i][j]))
				//{
				//	// Variable does not start with letter.
				//	strncat(s, &lines[i][j], 1);
				//	printf("Error 34 (Unrecoverable): Identifier name cannot begin with a number: %s", s);
				//	exit(1);
				//}
				j--;

				strcpy(table[k].lex, s);
				free(s);
				k++;
			}
			// Invalid symbol.
			else
			{
				printf("Error 29 (Unrecoverable): Invalid character: %c on line %d", lines[i][j], i);
				exit(1);
			}
		}
	}

	// Save the number of tokens into the first index of the struct to be used by the parser.
	table[0].count = k;

	// If '-t' print command was used.
	if (printLexTable)
	{
		printTable(table, k);
	}

	// If '-l' print command was used. 
	if (printLexList)
	{
		printList(table, k);
	}

	return table;
}