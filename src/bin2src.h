#ifndef BIN2SRC_H_
#define BIN2SRC_H_

#define VERSION     "1.0"

#define DEFAULT_ITD 4
#define DEFAULT_OUT "out.asm"
#define DEFAULT_KWD "!byte"
#define DEFAULT_SEP ','

char *newstr( char *initial_str );
void print_info();
void print_help();

#endif // BIN2SRC_H_
