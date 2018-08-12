# printf-Implementation

this file include a printf implementation supporting the next identifiers:
d	- signed integer as a decimal number
u	- unsigned integer as a decimal number
b	- unsigned integer as a binary number
o	- unsigned integer as a octal number
x	- unsigned integer as a hexadecimal number. Letter numerals printed in lower case
X	- unsigned integer as a hexadecimal number. Letter numerals printed in upper case
s	- null-terminated string
c	- character according to the ascii table
Ad, As, Ax, etc. - to print arrays of integers, string, hexadecimal integers, etc.
<width>d, %<width> - The width field is a non-negative decimal integer giving the minimum number of characters to be printed. If the output value is shorter than the given width, it is padded to the appropriate width by putting blanks on the right
(or on the left, if the '-' "modifier" character is specified).
%	- the '%' character.

the enum printf_state include the current kind of identifier. for each state there is array of handlers functions.
the functions responsible to printing the appropriate chars and moving on to the next state(char). 

the string that should be printed(fs) is representes by a va_list.
the program looping on the list and invokes the appropriate handler in every itaration.
