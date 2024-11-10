## How to build
Build system is cmake, minimum version is 3.5. If you don't have it - install.
1. Create build folder in project and go into it.
    You can use this commands:
    ```
    mkdir build
    cd build
    ```

2. Preset CMake.
    To preset CMake, write down this command:
    ```
    cmake ..
    ```

3. Build the project:
    ```
    cmake --build .
    ```

## About K13 Compiler
This compiler translate input language K13 into C and build it using gcc to create executable file.
To achive this functionality, compiling is divided by 5 etaps:
1. Lexical analisis: After executing `k13c path_to_file.k13`, it reads all lexems from file and group them. Result is vectors of lexems, string literals and unknown lexems.
2. Syntax analisis: after file was read successfully, lexems and unknown lexems are sent to syntax analizator. If there is any unknown lexem, analizator throws error in line with unknown lexem. After that, it check all lexems for right syntax.
3. Semantic analisis: When program has right syntax, semantic analizator checks if all expressions has right type and all variables are defined in their statements. It return table of labels and variables.
4. Translation: When programs semantic is right, translator create file with program name and format .c.
5. Build execution: Last step is compiling .c file using gcc. Program use system call of gcc and, if it's not found in system, throw error.
### Language description in EBNF:
```
<program> = "program" <identifier> ";" <body>

<body> = "start" <variables> <statements> "finish"

<variables> = "var" <variable_list> ";" 

<variable_list> = <type> <identifier> {"," <type> <identifier>} 

<type> = "int16_t" | "string" | "bool"

<statements> = <statement> {";" | <statement>} 

<statement> = <compound_statement> | <conditional_statement> | <goto_statement> 
                | <loop> | <assignment> | <input> | <output>

<compound_statement> = "start" <variables> <statements> "finish" 

<conditional_statement> = "if" "(" <condition> ")" ("goto" <label> ";" 
                | "goto" <label> ";" "goto" <label> ";") 

<goto_statement> = "goto" <label> ";" 

<label> = <identifier> (";" | <compound_statement>)

<loop> = "for" <identifier> "to" <expression> "next" <compound_statement>

<input> = "get" <identifier> ";" <output> = "put" <identifier> ";"

<assignment> = <identifier> ":=" <expression> ";"

<expression> = <term> <additive_operator> <term>

<term> = <factor> <multiplicative_operator> <factor> 

<factor> = <number> | <string_literal> | <boolean_literal> | <identifier> 
            | "(" <expression> ")"

<condition> = <logical_expression> { <logical_operator> "(" <logical_expression> ")" }

<logical_expression> = <expression> <comparison_operator> <expression> 
                        | "!!" "(" <expression> <comparison_operator> <expression> ")" 

<additive_operator> = "+" | "-"

<multiplicative_operator> = "*" | "/" | "%"

<comparison_operator> = "=" | "<>" | "le" | "ge"

<identifier> = <lowercase_letter> {<lowercase_letter> | <uppercase_letter> | <digit> | <_>}

<lowercase_letter> = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" 
                        | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" 
                        | "x" | "y" | "z" 

<uppercase_letter> = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" 
                        | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" 
                        | "X" | "Y" | "Z" 

<digit> = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" 

<number> = <digit> {<digit>}

<string_literal> = """ {<character> | """}

<boolean_literal> = "true" | "false"

<logical_operator> = "&&" | "||"

<character> = any character
```
### Here is table of terminal symbols and keywords

| Terminal Symbol or Keyword | Meaning                          |
| -------------------------- | -------------------------------- |
| program                    | Start of the program             |
| start                      | Start of statements              |
| var                        | Start of the variable declaration block |
| finish                     | End of statements                |
| get                        | Variable input statement         |
| put                        | Variable output statement        |
| :=                         | Assignment operator              |
| if                         | Conditional statement            |
| goto                       | Go to label                      |
| for                        | Start of loop                    |
| to                         | Loop condition                   |
| next                       | End of loop                      |
| +                          | Addition operator                |
| -                          | Subtraction operator             |
| *                          | Multiplication operator          |
| /                          | Division operator                |
| %                          | Modulo operator                  |
| =                          | Equality operator                |
| <>                         | Inequality operator              |
| le                         | Less than operator               |
| ge                         | Greater than operator            |
| !!                         | Logical NOT                      |
| &&                         | Logical AND                      |
| ||                         | Logical OR                       |
| int16_t                    | Integer type                     |
| bool                       | Boolean type (true or false)     |
| true                       | True                             |
| false                      | False                            |
| string                     | String type                      |
| $$ ...                     | Comment                          |
| ,                          | Separator                        |
| ;                          | End of statement                 |
| (                          | Opening parenthesis              |
| )                          | Closing parenthesis              |
| “                          | String literal boundary          |