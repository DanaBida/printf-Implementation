/* caspl-lab-1.c
 * Limited versions of printf
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* the states in the printf state-machine */
enum printf_state {
  st_printf_init,
  st_printf_meta_char,
  st_printf_percent,
  st_printf_octal2,
  st_printf_octal3,
  st_printf_array
};

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int print_int_helper_unsigned(unsigned int n, int radix, const char *digit) {
  int result;

  if (n < radix) {
    putchar(digit[n]);
    return 1;
  }
  else {
    result = print_int_helper_unsigned(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}

int print_int_unsigned(unsigned int n, int radix, const char * digit) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

  if (n > 0) {
    return print_int_helper_unsigned(n, radix, digit);
  }
  if (n == 0) {
    putchar('0');
    return 1;
  }
  else {
    putchar('-');
    return 1 + print_int_helper_unsigned(-n, radix, digit);
  }
}

int print_int_helper(int n, int radix, const char *digit) {
  int result;

  if (n < radix) {
    putchar(digit[n]);
    return 1;
  }
  else {
    result = print_int_helper(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}

int print_int(int n, int radix, const char * digit) //prints n in radix 'radix', and returns the number of chars that printed
{
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

  if (n > 0) {
    return print_int_helper(n, radix, digit);
  }
  if (n == 0) {
    putchar('0');
    return 1;
  }
  else {
    putchar('-');
    return 1 + print_int_helper(-n, radix, digit);
  }
}
  
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    char* fs;
    char* octal_char;
} state_args;


struct state_result {
  int printed_chars;
  enum printf_state new_state;
};

////////////////////////init_state_handler//////////////////////

void defult_init_handler(struct state_result *res , state_args* state){

	putchar(*(state->fs));
	++(res->printed_chars);
}

void init_handler_percentChar(struct state_result *res , state_args* state){

	res->new_state = st_printf_percent;
}

void init_handler_double_slash(struct state_result *res , state_args* state){

	res->new_state = st_printf_meta_char;
}

struct state_result init_state_handler(va_list* args, int* out_printed_chars, state_args* state){

	struct state_result res;
    res.printed_chars = *out_printed_chars;
    res.new_state = st_printf_init;

  	//define array to handlers functions
   	typedef void (*handlers_init)( struct state_result * , state_args* );
   	handlers_init array_handlers_init[128];
	//init
	for(int i=0;i<128;i++)
		array_handlers_init[i] = defult_init_handler;
	array_handlers_init[(int)'%'] = init_handler_percentChar;
	array_handlers_init[(int)'\\'] = init_handler_double_slash;

    array_handlers_init[(int)*(state->fs)](&res, &(*state));

    return res;
}

////////////////////////meta_char_state_handler//////////////////////

void defult_meta_char_handler(struct state_result *res , state_args* state){

	if (is_octal_char(*(state->fs))){  
        *(state->octal_char) = *(state->fs) - '0'; //adds octal_char_1
        res->new_state = st_printf_octal2;
    }
    else {
        toy_printf("Unknown meta-character: \\%c", *(state->fs));
        exit(-1);
    }
}

void meta_char_handler_double_slash(struct state_result *res , state_args* state){

	 putchar('\\'); //putchar - print that char to the screen
     ++(res->printed_chars);
     res->new_state = st_printf_init;
}

void meta_char_handler_slash_gereshim(struct state_result *res , state_args* state){

	 putchar('\\'); //putchar - print that char to the screen
     ++(res->printed_chars);
     res->new_state = st_printf_init;
}

void meta_char_handler_slash_t(struct state_result *res , state_args* state){

 	putchar('\t');
    ++(res->printed_chars);
    res->new_state = st_printf_init;
}

void meta_char_handler_T(struct state_result *res , state_args* state){

 	(res->printed_chars) += toy_printf("<tab>\t");
    res->new_state = st_printf_init;
}

void meta_char_handler_f(struct state_result *res , state_args* state){

 	putchar('\f');
    ++(res->printed_chars);
    res->new_state = st_printf_init;
}

void meta_char_handler_F(struct state_result *res , state_args* state){

 	(res->printed_chars) += toy_printf("<formfeed>\f");
    res->new_state = st_printf_init;
}

void meta_char_handler_n(struct state_result *res , state_args* state){

    putchar('\n');
    ++(res->printed_chars);
    res->new_state = st_printf_init;
}

void meta_char_handler_N(struct state_result *res , state_args* state){

    (res->printed_chars) += toy_printf("<newline>\n");
    res->new_state = st_printf_init;
}

void meta_char_handler_r(struct state_result *res , state_args* state){

	putchar('\r');
    ++(res->printed_chars);
    res->new_state = st_printf_init;
}

void meta_char_handler_R(struct state_result *res , state_args* state){

    (res->printed_chars) += toy_printf("<return>\r");
    res->new_state = st_printf_init;
}

void meta_char_handler_c(struct state_result *res , state_args* state){

    (res->printed_chars) += toy_printf("CASPL'2018");
    res->new_state = st_printf_init;
}

void meta_char_handler_C(struct state_result *res , state_args* state){

    (res->printed_chars) += toy_printf("<caspl magic>");
    (res->printed_chars) += toy_printf("\\c");
    res->new_state = st_printf_init;
}


struct state_result meta_char_state_handler(va_list* args, int* out_printed_chars, state_args* state){

   struct state_result res;
   res.printed_chars = *out_printed_chars;
   res.new_state = st_printf_meta_char;

   //define array to handlers functions
   typedef void (*handlers_meta_char)( struct state_result * , state_args* );
   handlers_meta_char array_handlers_meta_char[128];
   //init
	for(int i=0;i<128;i++)
		array_handlers_meta_char[i] = defult_meta_char_handler;
	array_handlers_meta_char[(int)'\\'] = meta_char_handler_double_slash;
	array_handlers_meta_char[(int)'\"'] = meta_char_handler_slash_gereshim;
	array_handlers_meta_char[(int)'\t'] = meta_char_handler_slash_t;
	array_handlers_meta_char[(int)'T'] = meta_char_handler_T;
	array_handlers_meta_char[(int)'f'] = meta_char_handler_f;
	array_handlers_meta_char[(int)'F'] = meta_char_handler_F;
	array_handlers_meta_char[(int)'n'] = meta_char_handler_n;
	array_handlers_meta_char[(int)'N'] = meta_char_handler_N;
	array_handlers_meta_char[(int)'r'] = meta_char_handler_r;
	array_handlers_meta_char[(int)'R'] = meta_char_handler_R;
	array_handlers_meta_char[(int)'c'] = meta_char_handler_c;
	array_handlers_meta_char[(int)'C'] = meta_char_handler_C;

    array_handlers_meta_char[(int)*(state->fs)](&res, &(*state));

    return res;
}

////////////////////////printf_octal2_state_handler//////////////////////


void defult_octal2_handler(struct state_result *res , state_args* state){

	toy_printf("Missing second octal char. Found: \\%c", *(state->fs));
    exit(-1);
}

void octal2_handler_octal_char(struct state_result *res , state_args* state){

	 *(state->octal_char) = (*(state->octal_char) << 3) + (*(state->fs) - '0'); //adds octal_char_2 if not missing
     res->new_state = st_printf_octal3;
}

struct state_result printf_octal2_state_handler(va_list* args, int* out_printed_chars, state_args* state){

    struct state_result res;
   	res.printed_chars = *out_printed_chars;
   	res.new_state = st_printf_octal2;

   	//define array to handlers functions
   	typedef void (*handlers_octal2)( struct state_result * , state_args* );
   	handlers_octal2 array_handlers_octal2[128];
   	//init
	for(int i=0;i<128;i++)
		array_handlers_octal2[i] = defult_octal2_handler;
	array_handlers_octal2[(int)'0'] = octal2_handler_octal_char;
	array_handlers_octal2[(int)'1'] = octal2_handler_octal_char;
	array_handlers_octal2[(int)'2'] = octal2_handler_octal_char;
	array_handlers_octal2[(int)'3'] = octal2_handler_octal_char;
	array_handlers_octal2[(int)'4'] = octal2_handler_octal_char;
	array_handlers_octal2[(int)'5'] = octal2_handler_octal_char;
	array_handlers_octal2[(int)'6'] = octal2_handler_octal_char;
	array_handlers_octal2[(int)'7'] = octal2_handler_octal_char;

	array_handlers_octal2[(int)*(state->fs)](&res, &(*state));

    return res;

}

////////////////////////printf_octal3_state_handler//////////////////////

void defult_octal3_handler(struct state_result *res , state_args* state){

   toy_printf("Missing third octal char. Found: \\%c", *(state->fs));
   exit(-1);
}

void octal3_handler_octal_char(struct state_result *res , state_args* state){

	 *(state->octal_char) = (*(state->octal_char) << 3) + (*(state->fs) - '0');
     putchar(*(state->octal_char));
     ++ (res->printed_chars);
     res->new_state = st_printf_init;
}

struct state_result printf_octal3_state_handler(va_list* args, int* out_printed_chars, state_args* state){
           
	struct state_result res;
   	res.printed_chars = *out_printed_chars;
   	res.new_state = st_printf_octal2;

   	//define array to handlers functions
   	typedef void (*handlers_octal3)( struct state_result * , state_args* );
   	handlers_octal3 array_handlers_octal3[128];
   	//init
	for(int i=0;i<128;i++)
		array_handlers_octal3[i] = defult_octal3_handler;
	array_handlers_octal3[(int)'0'] = octal3_handler_octal_char;
	array_handlers_octal3[(int)'1'] = octal3_handler_octal_char;
	array_handlers_octal3[(int)'2'] = octal3_handler_octal_char;
	array_handlers_octal3[(int)'3'] = octal3_handler_octal_char;
	array_handlers_octal3[(int)'4'] = octal3_handler_octal_char;
	array_handlers_octal3[(int)'5'] = octal3_handler_octal_char;
	array_handlers_octal3[(int)'6'] = octal3_handler_octal_char;
	array_handlers_octal3[(int)'7'] = octal3_handler_octal_char;

	array_handlers_octal3[(int)*(state->fs)](&res, &(*state));

    return res;

}

////////////////////////printf_percent_state_handler//////////////////////

void defult_percent_handler(struct state_result *res , state_args* state , va_list* args){

   toy_printf("Missing third octal char. Found: \\%c", *(state->fs));
   exit(-1);
}

void percent_handler_0_to_9_char(struct state_result *res , state_args* state, va_list* args ){

	int width=0;
	char c;
	int int_value;// saves the value after % if it is int - returns the value with type int of the next argument
    char* string_value;// saves the value after % if it is string

	  if(*(state->fs) != 48){ //not zero
          c = *(state->fs);
          width = (int)(c - '0');// converts width to int    
          string_value = va_arg(*args, char *);
          (res->printed_chars) += toy_printf(string_value);//sequence of chars
          int count =0;//count the length of the string
          for (; (*string_value) != '\0'; ++string_value)
            count++;
          width -= count;//now width is the number of spaces
          for(int i=0; i<width; i++)
               (res->printed_chars) += toy_printf("%s", " ");
          if(width>0)
            (res->printed_chars) += toy_printf("%c", '#');
          ++(state->fs);
          res->new_state= st_printf_init;
        }
        if(*(state->fs) == 48){ //zero
          c = *(++(state->fs));
          width = (int)(c - '0');// converts width to int    
          int_value = va_arg(*args, int);//returns the value with type int of the next argument
          if(int_value < 0){
            (res->printed_chars) += toy_printf("%c", '-');
            width-=2;
          }
          for(int i=0; i<width; i++)
                (res->printed_chars) += toy_printf("%d",0);
          (res->printed_chars) += print_int(-int_value, 10, digit);
          res->new_state = st_printf_init;
          (state->fs)++;
        }
}

void percent_handler_percentChar(struct state_result *res , state_args* state, va_list* args){

    putchar('%');
    ++(res->printed_chars);
    res->new_state = st_printf_init;
}
void percent_handler_d(struct state_result *res , state_args* state, va_list* args){

 	int int_value;
 	int_value= va_arg(*args, int);
   	(res->printed_chars) += print_int(int_value, 10, digit);
    res->new_state = st_printf_init;
}

void percent_handler_b(struct state_result *res , state_args* state, va_list* args){

	int int_value;
 	int_value= va_arg(*args, int);
    res->printed_chars += print_int(int_value, 2, digit);
    res->new_state = st_printf_init;
}

void percent_handler_o(struct state_result *res , state_args* state , va_list* args){

	int int_value;
    int_value= va_arg(*args, int);
    res->printed_chars += print_int(int_value, 8, digit);
    res->new_state = st_printf_init;
}

void percent_handler_x(struct state_result *res , state_args* state , va_list* args){

	int int_value;
 	int_value= va_arg(*args, int);
	res->printed_chars += print_int(int_value, 16, digit);
    res->new_state = st_printf_init;
}

void percent_handler_X(struct state_result *res , state_args* state, va_list* args){

	int int_value;
 	int_value= va_arg(*args, int);
    res->printed_chars += print_int(int_value, 16, DIGIT);
    res->new_state = st_printf_init;
}

void percent_handler_u(struct state_result *res , state_args* state, va_list* args){

	int int_value;
 	int_value= va_arg(*args, int);
    res->printed_chars += print_int_unsigned(int_value, 10, digit);
    res->new_state = st_printf_init;
}


void percent_handler_s(struct state_result *res , state_args* state , va_list* args){

	char* string_value;
 	string_value = va_arg(*args, char *);
    res->printed_chars += toy_printf(string_value);//sequence of chars
    res->new_state = st_printf_init;
}

void percent_handler_c(struct state_result *res , state_args* state ,va_list* args){

	char char_value;
    char_value = (char)va_arg(*args, int); // saves the value after % if it is char
    putchar(char_value);
    ++res->printed_chars;
    res->new_state = st_printf_init;
}

void percent_handler_minus(struct state_result *res , state_args* state, va_list* args){

	int width =0;
	char c;
    int count =0;
    char* string_value;

    c = *(++(state->fs));
    width = (int)(c - '0');
    string_value = va_arg(*args, char *);
    char * p = string_value;//points to the beginning of string
    for (; *p != '\0'; ++p)
        count++;
    width -= count;
    for(int i=0; i<width; i++)
        res->printed_chars += toy_printf("%s", " ");
    res->printed_chars += toy_printf(string_value);//sequence of chars
    (state->fs)++;
    res->new_state = st_printf_init;
}

void percent_handler_A(struct state_result *res , state_args* state, va_list* args){

	res->new_state = st_printf_array;
}


struct state_result printf_percent_state_handler(va_list *args, int* out_printed_chars, state_args* state){
	
    struct state_result res;
   	res.printed_chars = *out_printed_chars;
   	res.new_state = st_printf_percent;

	//define array to handlers functions
   	typedef void (*handlers_percent)( struct state_result * , state_args*, va_list*);
   	handlers_percent array_handlers_percent[128];
   	//init
	for(int i=0;i<128;i++)
		array_handlers_percent[i] = defult_percent_handler;
	for(int i=48; i<=57;i++)
		array_handlers_percent[i] = percent_handler_0_to_9_char;
	array_handlers_percent[(int)'%'] = percent_handler_percentChar;
	array_handlers_percent[(int)'d'] = percent_handler_d;
	array_handlers_percent[(int)'b'] = percent_handler_b;
	array_handlers_percent[(int)'o'] = percent_handler_o;
	array_handlers_percent[(int)'x'] = percent_handler_x;
	array_handlers_percent[(int)'X'] = percent_handler_X;
	array_handlers_percent[(int)'u'] = percent_handler_u;
	array_handlers_percent[(int)'s'] = percent_handler_s;
	array_handlers_percent[(int)'c'] = percent_handler_c;
	array_handlers_percent[(int)'-'] = percent_handler_minus;
	array_handlers_percent[(int)'A'] = percent_handler_A;
	

	array_handlers_percent[(int)*(state->fs)](&res, &(*state) , args);

    return res;

}

////////////////////////printf_array_state_handler//////////////////////

void defult_array_handler(struct state_result *res , state_args* state , va_list* args){

    toy_printf("Unhandled format %%%c...\n", *(state->fs));
    exit(-1);
}

void array_handler_d(struct state_result *res , state_args* state , va_list* args){

	int* integers_array;	
	int array_size =0;

    integers_array = va_arg(*args, int[]);
    array_size = va_arg(*args, int);
    for(int i=0; i<array_size; i++){
        res->printed_chars += toy_printf("%d", integers_array[i]);
        if(i!=array_size-1)
            res->printed_chars += toy_printf(", ");
    }
                  
    res->new_state = st_printf_init;
}

void array_handler_b(struct state_result *res , state_args* state , va_list* args){

	int* integers_array;
	int array_size =0;

    integers_array = va_arg(*args, int[]);
    array_size = va_arg(*args, int);
    for(int i=0; i<array_size; i++){
        res->printed_chars += toy_printf("%b", integers_array[i]);
        if(i!=array_size-1)
            res->printed_chars += toy_printf(", ");
    }
    res->new_state = st_printf_init;
}

void array_handler_o(struct state_result *res , state_args* state , va_list* args){

	int* integers_array;
	int array_size =0;

    integers_array = va_arg(*args, int[]);
    array_size = va_arg(*args, int);
    for(int i=0; i<array_size; i++){
        res->printed_chars += toy_printf("%o", integers_array[i]);
        if(i!=array_size-1)
            res->printed_chars += toy_printf(", ");
    }
    res->new_state = st_printf_init;
}

void array_handler_x(struct state_result *res , state_args* state , va_list* args){

	int* integers_array;
    int array_size =0;

	integers_array = va_arg(*args, int[]);
    array_size = va_arg(*args, int);
    for(int i=0; i<array_size; i++){
        res->printed_chars += toy_printf("%x", integers_array[i]);
        if(i!=array_size-1)
            res->printed_chars += toy_printf(", ");
    }
    res->new_state = st_printf_init;
}

void array_handler_X(struct state_result *res , state_args* state , va_list* args){

	int* integers_array;
    int array_size =0;

    integers_array = va_arg(*args, int[]);
    array_size = va_arg(*args, int);
    for(int i=0; i<array_size; i++){
        res->printed_chars += toy_printf("%X", integers_array[i]);
        if(i!=array_size-1)
            res->printed_chars += toy_printf(", ");
    }
    res->new_state = st_printf_init;
}

void array_handler_s(struct state_result *res , state_args* state , va_list* args){
    
    char** strings_array;
    int array_size =0;

    strings_array = va_arg(*args, char*[]);
    array_size = va_arg(*args, int);
    for(int i=0; i<array_size; i++){
        res->printed_chars += toy_printf("%s", strings_array[i]);
        if(i!=array_size-1)
            toy_printf(", ");
    }
    res->new_state = st_printf_init;
}

void array_handler_c(struct state_result *res , state_args* state , va_list* args){

	char* chars_array;
    int array_size =0;

	chars_array = va_arg(*args, char[]);
    array_size = va_arg(*args, int);
    for(int i=0; i<array_size; i++){
        res->printed_chars += toy_printf("%c", chars_array[i]);
        if(i!=array_size-1)
            res->printed_chars += toy_printf(", ");
    }
    res->new_state = st_printf_init;
}



struct state_result printf_array_state_handler(va_list *args, int* out_printed_chars, state_args* state){

	struct state_result res;
   	res.printed_chars = *out_printed_chars;
   	res.new_state = st_printf_array;

   	res.printed_chars += toy_printf("{");

	//define array to handlers functions
   	typedef void (*handlers_array)( struct state_result * , state_args*, va_list*);
   	handlers_array array_handlers_array[128];
   	//init
	for(int i=0;i<128;i++)
		array_handlers_array[i] = defult_array_handler;
	array_handlers_array[(int)'d'] = array_handler_d;
	array_handlers_array[(int)'b'] = array_handler_b;
	array_handlers_array[(int)'o'] = array_handler_o;
	array_handlers_array[(int)'x'] = array_handler_x;
	array_handlers_array[(int)'X'] = array_handler_X;
	array_handlers_array[(int)'s'] = array_handler_s;
	array_handlers_array[(int)'c'] = array_handler_c;

	array_handlers_array[(int)*(state->fs)](&res, &(*state) , args);

	res.printed_chars += toy_printf("}");

    return res;

}



/* SUPPORTED:
 *   \f, \t, \n, \r -- formfeed, tab, newline, return
 *   \F, \T, \N, \R -- extensions: visible versions of the above
 *   \c -- extension: CASPL'2018
 *   \C -- extension: visible version of the above
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */


int toy_printf(char *fs, ...) 
{

  int chars_printed = 0;//counts how many chars is printing and returns it
  char octal_char; //base 8 - digits 0 to 7 are used
  va_list args;// the list with and after fs of strings
  enum printf_state state;
  va_start(args, fs);// init the list args by the strings in and after fs
  state = st_printf_init;//the state of printing : init / meta_char / percent
  
  state_args s;
  int handler_printed_chars;
  s.fs=fs;
  s.octal_char=&octal_char;

  struct state_result temp;

  	//define array to handlers functions
  	typedef struct state_result (*handlers)( va_list*, int*, state_args* ); //declares a variable called handlers that points to a function of this type
	handlers array_handlers[6];
	//init
	array_handlers[0] = init_state_handler;
	array_handlers[1] = meta_char_state_handler;
	array_handlers[2] = printf_percent_state_handler;
	array_handlers[3] = printf_octal2_state_handler;
	array_handlers[4] = printf_octal3_state_handler;
	array_handlers[5] = printf_array_state_handler;


  for (; *(s.fs) != '\0'; ++(s.fs)) { // looping all the strings in the list args

     handler_printed_chars= 0;
     temp = array_handlers[state](&args, &handler_printed_chars, &s);
     chars_printed += temp.printed_chars;
     state = temp.new_state;

  }

  va_end(args);// after va_start invocation, a corresponding invocation of va_end() must happand.

  return chars_printed;// returs the number of chars that have been printed

}
