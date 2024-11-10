## How to build
Build system is cmake, minimum version is 3.5. If you don't have it - install.
1. Create build folder in project and go into it.\n
    You can use this commands:
    ```
    mkdir build
    cd build
    ```

2. Preset CMake.\n
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

\n\tLanguage description in EBNF:\n
<program> = "program" <identifier> ";" <body>\n
\n
<body> = "start" <variables> <statements> "finish" 
\n<variables> = "var" <variable_list> ";" 
\n<variable_list> = <type> <identifier> {"," <type> <identifier>} 
\n<type> = "int16_t" | "string" | "bool"
\n<statements> = <statement> {";" | <statement>} 
\n
\n<statement> = <compound_statement> | <conditional_statement> | <goto_statement> | <loop> | <assignment> | <input> | <output>
\n
\n<compound_statement> = "start" <variables> <statements> "finish" 
\n<conditional_statement> = "if" "(" <condition> ")" ("goto" <label> ";" | "goto" <label> ";" "goto" <label> ";") 
\n<goto_statement> = "goto" <label> ";" 
\n<label> = <identifier> (";" | <compound_statement>)
\n
\n<loop> = "for" <identifier> "to" <expression> "next" <compound_statement>
\n
\n<input> = "get" <identifier> ";" <output> = "put" <identifier> ";"
\n
\n<assignment> = <identifier> ":=" <expression> ";"
\n
\n<expression> = <term> <additive_operator> <term> 
\n<term> = <factor> <multiplicative_operator> <factor> 
\n<factor> = <number> | <string_literal> | <boolean_literal> | <identifier> | "(" <expression> ")"
\n
\n<condition> = <logical_expression> { <logical_operator> "(" <logical_expression> ")" } 
\n<logical_expression> = <expression> <comparison_operator> <expression> | "!!" "(" <expression> <comparison_operator> <expression> ")" 
\n
\n<additive_operator> = "+" | "-" 
\n<multiplicative_operator> = "*" | "/" | "%" 
\n<comparison_operator> = "=" | "<>" | "le" | "ge"
\n
\n<identifier> = <lowercase_letter> {<lowercase_letter> | <uppercase_letter> | <digit> | <_>} 
\n<lowercase_letter> = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" \n<uppercase_letter> = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" \n<digit> = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" 
\n
\n<number> = <digit> {<digit>} 
\n<string_literal> = """ {<character> | """} 
\n<boolean_literal> = "true" | "false" 
\n<logical_operator> = "&&" | "||" 
\n<character> = any character

\n Here is table of terminal symbols and keywords

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