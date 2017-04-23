# PL/0 Compiler

========================

Compiler for the PL/0 language with registers written in C.

ERROR RECOVERY NOT IMPLEMENTED.

------------------------

Compiler will read any text file from the command line specified by argv[1] of any name,
take in any amount of the available commands in any order separated by spaces,
and will output to a text file of any name. 

Compiling in Linux Environment:

gcc -o [name] PL0Compiler.c LexiAnalyze.c ParserCodeGen.c PL0VM.c

Running in Linux Environment:

./[name] [input] [commands] > [output]

Print commands:

-s - print input source code.

-l - print list of lexemes.

-a - print generated assembly code.

-v - print virtual machine execution trace.

Remark:

A more detailed print statement of the lexeme table can be enabled at the end of LexiAnalyze.c.

Compile and cammand examples:

./compile input.txt –l –a –v -s		Print all types of output to the console.

./compile input.txt –v				Print only the VM execution trace to the console.

./compile input.txt					Print nothing to the console except for "in" and "out".

"/Cases" directory contains various test cases and examples that can be used with the compiler.

------------------------

EBNF of PL/0:


program :: = block "." .

block :: = const-declaration var-declaration procedure-declaration statement.

const-declaration :: = ["const" ident "=" number{ "," ident "=" number } ";"].

var-declaration :: = ["var "ident{ "," ident } ";"].

procedure-declaration :: = { "procedure" ident ";" block ";" }

statement :: = [ident ":=" expression

				| "call" ident
				
				| "begin" statement{ ";" statement } "end"
				
				| "if" condition "then" statement["else" statement]
				
				| "while" condition "do" statement
				
				| "read" ident
				
				| "write" expression
				
				| e] .
				
condition :: = "odd" expression

				| expression  rel-op  expression.
				
rel-op :: = "=" | "<>" | "<" | "<=" | ">" | ">=".

expression :: = ["+" | "-"] term{ ("+" | "-") term }.

term :: = factor{ ("*" | "/") factor }.

factor :: = ident | number | "(" expression ")".

number :: = digit{ digit }.

ident :: = letter{ letter | digit }.

digit :: = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9".

letter :: = "a" | "b" | … | "y" | "z" | "A" | "B" | ... | "Y" | "Z".


Wirth’s rules for EBNF:

[] means an optional item.

{ } means repeat 0 or more times.

Terminal symbols are enclosed in quote marks.

A period is used to indicate the end of the definition of a syntactic class.


------------------------

Lexical Conventions for PL/0:

A numerical value is assigned to each token (internal representation) as follows: 

nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,  

slashsym = 7, oddsym = 8,  eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, 

gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, 

periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, 

whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, 

writesym = 31, readsym = 32, elsesym = 33.


Reserved Words: const, var, procedure, call, begin, end, if, then, else, while, do, read, write.

Special Symbols: '+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'.

Identifiers: identsym = letter (letter | digit)* 

Numbers: numbersym = (digit)+

Invisible Characters: tab, white spaces, newline

Comments denoted by: /* . . . */


------------------------

Error messages for the tiny PL/0 Parser:

1.	Use = instead of :=.
2.	= must be followed by a number.
3.	Identifier must be followed by =.
4.	const, var, procedure must be followed by identifier.
5.	Semicolon or comma missing.
6.	Incorrect symbol after procedure declaration.
7.	Statement expected.
8.	Incorrect symbol after statement part in block.
9.	Period expected.
10.	Semicolon between statements missing.
11.	Undeclared identifier.
12.	Assignment to constant or procedure is not allowed.
13.	Assignment operator expected.
14.	call must be followed by an identifier.
15.	Call of a constant or variable is meaningless.
16.	then expected.
17.	Semicolon or } expected.
18.	do expected.
19.	Incorrect symbol following statement.
20.	Relational operator expected.
21.	Expression must not contain a procedure identifier.
22.	Right parenthesis missing.
23.	The preceding factor cannot begin with this symbol.
24.	An expression cannot begin with this symbol.
25.	This number is too large.
26.	Identifier must follow read.
27.	Identifier must follow write.
28.	Procedures not yet supported.
29.	Invalid character detected.
30.	Use of an unassigned variable.
31.	Reserved keyword cannot be used as an identifier name.
32.	All registers already in use.
33.	Identifier name is too long.
34.	End comment was never opened.
35.	Comment never closed.

------------------------

Author:
Alec Psinakis