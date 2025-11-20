# Coding guidelines

##	Introduction

This document describes the coding style used by the electronics dep. software development team. The intention is to have a common and shared coding framework, that would be easier to understand for  any team member. By using this set of rules, the portability of the code is increased and the style helps avoid possible problems with the C or C++ language.

##	Scope

The electronics dep. software development team should have these 3 rules in mind:
1.	Code should be error free.
2.	Code should be easy to maintain.
3.	Code should be easy to understand.

##	General Rules

- Rule 3.1 – The code is written in ANSI C99 / C++14.

- Rule 3.2 – The code and comments is written by using English language and by using UTF8 character  encoding.

- Rule 3.3 – The compiler warnings that cannot be eliminated are commented in the code.

- Rule 3.4 – The header files do not contain any executable code, except of inline code.

- Rule 3.5 – The #include statements do not include any absolute file path. These absolute paths will be set by using the CMake or other project settings.

- Rule 3.6 – The comments are written by using the /* */ style.

##	Naming Rules

- Rule 4.1 – Header file names have the extension .h, while source file names have the extension .c

- Rule 4.2 – File names contain only alphanumeric characters, underscore “_” and minus sign”-“ or period “.”.

- Rule 4.3 – All macros in preprocessor directives are in uppercase characters with words separated by underscore. Example:
    ```
    #define THIS_IS_A_CORRECT_MACRO
    ```
- Rule 4.4 – Enumeration constants are in uppercase with individual words separated by underscores. Example:
    ```
	 typedef enum
     {
	    NUM_1
	    NUM_2
        NUM_3
	 } numbers_t;
    ```

- Rule 4.5 – Variables are written in CamelCase convention, but the first letter of the name is in lowercase.
	Example:
    ```
    uint8_t piNumber = 3.14;
    ```

- Rule 4.6 – The global and local variables name have the following naming convention:
	<PrefixType><Name>, where <PrefixType>  might be:
    -	p: pointer to data
    -	pf: pointer to function
    -	e: enum variables
    -	b: boolean variables
    -	a: arrays
    -	nothing: for everything else

## Code Formatting Rules

- Rule 5.1 – The maximum width of a code line does not exceed 250 characters.

- Rule 5.2 – Last line in a file is an empty line.

- Rule 5.3 – Pairs of matching braces are located in the same column.

- Rule 5.4 – An opening brace is placed on the same indentation level as the preceding line. Example:
    ```
    while(true)
	{
		/* Do anything */
    }
    ```

- Rule 5.5 – Pairs of matching parentheses are either located in the same line or in the same column. Example:
	```
    while (true)
    {
		if ( (a > b) ||
		     ((b > c) && ( c < d ))
		   )
	    {
            /* Do anything */
        }
   }
   ```

- Rule 5.6 – Local variables in functions are declared under the preceding brace “{” at the left margin and  indented at the level of the code. An empty line is placed after the variable declaration and before code.

- Rule 5.7 – The statement body of an if statement, for statement or a while statement always start on the following line and braces are used to guard their body.

- Rule 5.8 – For the following statements the body is idented:
    - Selection statement (if, else, else if, switch)
    - Iteration statement (do…while, while, for)
    - Label statement (case, default)

- Rule 5.9 – Unary operators (++, --, &, ~, !, *) , as well as the structure reference  operators always stick to the variable or expression. Example:
```
    !isChar(ch);
    upbdrive.department;
    pCar->wheelHub;
```

- Rule 5.10 – Each line of code contains a single statement, and each statement does only one thing.

##	Code Robustness Rule

- Rule 6.1 – Header files are guarded from duplicate inclusion by testing for the definition of a value. Example:
    ```
	#ifndef CONFIG_H
    #define CONFIG_H
	/* contents of config.h */
    #endif /* CONFIG_H */
    ```

- Rule 6.2 – Multi-statement macros are wrapped in a do-while loop, so that the macro can appear safely inside if clauses or other expressions that expect a single statement or a statement block. Example:
    ```
	#SWAP(x,y) do
	{
	    /* contents of SWAP */
	} while (0)
    ```

##	Data declaration

- Rule 6.3 – All declarations are unsigned, except when negative values are specifically needed.

- Rule 6.4 – Any variable that is not written before it is read, is explicitly initialized.

- Rule 6.5 – All local variables to a function are declared at the starting of the function.

- Rule 6.6 – No more than 1 declaration per code line is done.

- Rule 6.7 – Local (file-scope) objects (variables or functions) that do not need external linkage are declared as “static”.

- Rule 6.8 – Use volatile qualifier for the data you do not want the compiler to optimize.

##	Expressions

- Rule 6.9 – The parantheses must be used to explicitly define the precedence of operations. Never rely on your memorized precedence of C operators.

- Rule 6.10 – The constants must be placed on the left of equality comparisons.
	Example:
    ```
	(CONSTANT == variable) /* good */
	(variable == CONSTANT) /* bad, not acceptable */
    ```

- Rule 6.11 – When testing incrementing or decrementing counters, the >= or <= operators shall be used instead of == in the comparison statements. This prevents errors caused by the counter getting higher or lower than expected.

- Rule 6.12 – Multiple assignments must not be done. Example:
	```
    uint8_t x = y = z; /* bad, not acceptable */
    ```

##	Functions

- Rule 6.13 – Every function must perform only one task.
- Rule 6.14 – Recursion must not be used.
- Rule 6.15 - Dynamic allocation of memory is forbidden

##	Miscellaneous

- Rule 6.15 – Attempts to divide by zero must be avoided.
- Rule 6.16 – Over/underflow cases must be taken into consideration in the case of calculations.
- Rule 6.17 – Endless loops should be avoided.



