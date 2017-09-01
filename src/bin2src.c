#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bin2src.h"

int main( int argc, char *argv[] )
{
    // vars depending on #defines
    char    outfile_name[128]   = DEFAULT_OUT;
    char    keyword[8]          = DEFAULT_KWD;

    int     intendation         = DEFAULT_ITD;
    int     seperator           = DEFAULT_SEP;

    // vars - init just to be "super-safe" :-P
    char    *infile_name        = NULL;
    char    *infile_nopath      = NULL;
    char    *temp_string        = NULL;
    char    *whitespaces        = NULL;

    FILE    *infile             = NULL;
    FILE    *outfile            = NULL;

    int     c                   = 0;
    int     i                   = 0;
    int     infile_size         = 0;
    int     input_data          = 0;
    int     last_byte           = 0;
    int     skipbytes           = 0;
    int     countbytes          = 0;
    int     addoffset           = 0;

    print_info();

    if ( ( argc == 1 ) ||
         ( strcmp( argv[1], "-h" ) == 0 ) ||
         ( strcmp( argv[1], "-help" ) == 0 ) ||
         ( strcmp( argv[1], "-?" ) == 0 ) ||
         ( strcmp( argv[1], "--help" ) == 0 ) )
    {
        print_help();
        exit( EXIT_SUCCESS );
    }

    // getopt cmdline-argument handler
    opterr = 1;

    while ( ( c = getopt ( argc, argv, "a:c:i:k:o:s:" ) ) != -1 )
    {
        switch ( c )
        {
            case 'a':
                if ( sscanf( optarg, "%i", &addoffset) != 1 )
                {
                    printf( "\nError: -a needs an integer value for 8-bit offset\n" );
                    exit( EXIT_FAILURE );
                }
                if ( addoffset > 0xFF )
                {
                    printf( "\nError: -a offset must be 8-bit (255 / 0xFF max)\n" );
                    exit( EXIT_FAILURE );
                }
                break;
            case 'c':
                if ( sscanf( optarg, "%i", &countbytes) != 1 )
                {
                    printf( "\nError: -c needs an integer value for number of bytes\n" );
                    exit( EXIT_FAILURE );
                }
                break;
            case 'k':
                if ( strlen( optarg ) > 7 )
                {
                    printf( "\nError: -k keyword name parameter too long\n" );
                    exit( EXIT_FAILURE );
                }
                else
                {
                    strcpy( keyword, optarg );
                }
                break;
            case 'o':
                if ( strlen( optarg ) > 127 )
                {
                    printf( "\nError: -o outfile parameter too long\n" );
                    exit( EXIT_FAILURE );
                }
                else
                {
                    strcpy( outfile_name, optarg);
                }
                break;
            case 's':
                if ( sscanf( optarg, "%i", &skipbytes) != 1 )
                {
                    printf( "\nError: -s needs an integer value for file offset\n" );
                    exit( EXIT_FAILURE );
                }
                break;
        }
    }

    // make sure a file was given
    if ( ( optind ) == argc )
    {
        printf( "\nError: no file specified.\n" );
        exit( EXIT_FAILURE );
    }

    // create outfile and make sure it's writeable
    outfile = fopen( outfile_name, "w" );
    if ( outfile == NULL )
    {
        printf( "\nError: couldn't create file \"%s\".\n", outfile_name );
        exit( EXIT_FAILURE );
    }
    else
    {
        fclose( outfile );
    }

    // prepare whitespaces string
    whitespaces = malloc( intendation + 1 );
    for ( i = 0; i < intendation; i++ )
    {
        whitespaces[i] = ' ';
    }
    whitespaces[intendation] = '\0';

    // open files
    infile_name = newstr( argv[optind] );
    infile_nopath = basename( infile_name );

    infile = fopen( infile_name, "rb" );
    if ( infile == NULL )
    {
        printf( "\nError: couldn't read file \"%s\".\n", infile_name );
        exit( EXIT_FAILURE );
    }
    outfile = fopen( outfile_name, "a" );

    printf( "input filename:    %s\n", infile_nopath );
    printf( "skip bytes:        %d\n", skipbytes );
    printf( "count bytes:       %d\n", countbytes );
    printf( "add offset:        %d\n", addoffset );
    printf( "output filename:   %s\n", outfile_name );
    printf( "keyword:           %s\n", keyword );
    printf( "intendation:       %d\n", intendation );

    // get infile size and last byte position
    fseek( infile, 0, SEEK_END );
    infile_size = ftell( infile );
    last_byte = infile_size - skipbytes - countbytes - 1;
    rewind( infile );
    if ( countbytes = 0 )
    {
        countbytes = infile_size;
    }

    // forward infile again according to skipbytes
    fseek( infile, skipbytes, 0 );

    // mainloop
    while  ( ( ( input_data = fgetc( infile ) ) != EOF ) && ( i < countbytes ) )
    {
        if ( i = 0 )
        {
            temp_string = malloc( strlen( whitespaces ) +
                                  strlen( keyword ) +
                                  1 );
            sprintf( temp_string, "%s%s", whitespaces, keyword );
            fputs( temp_string, outfile );
            free( temp_string );
        }
        if ( ( i > 0 ) && ( i % 16 = 0 ) )
        {
            temp_string = malloc( "\n" +
                                  strlen( whitespaces ) +
                                  strlen( keyword ) +
                                  1 );
            sprintf( temp_string, "\n%s%s", whitespaces, keyword );
            fputs( temp_string, outfile );
            free( temp_string );
        }
        if ( addoffset > 0 )
        {
            temp_string = malloc( strlen( " $00+$00" ) + 1 );
            sprintf( temp_string, " $%02x+$%02x", input_data, addoffset );
        }
        else
        {
            temp_string = malloc( strlen( " $00" ) + 1 );
            sprintf( temp_string, " $%02x", input_data );
        }
        fputs( temp_string, outfile );
        free( temp_string );
        if ( ( i % 16 != 0 ) && ( i != last_byte ) )
        {
            fputc( seperator, outfile );
        }
        i++;
    }

    free( whitespaces );
    free( infile_name );
    fclose( infile );
    fclose( outfile );

    printf( "done!\n" );

    exit( EXIT_SUCCESS );
}

// string helper functions

char *newstr( char *initial_str )
{
    int num_chars;
    char *new_str;

    num_chars = strlen( initial_str ) + 1;
    new_str = malloc ( num_chars );

    strcpy ( new_str, initial_str );

    return new_str;
}

// screen print stuff

void print_info()
{
    const char* version = VERSION;

    printf( "===============================================================================\n" );
    printf( "bin2src - generates assembly source tables from binary data file - V%s\n", version );
    printf( "                                                            by Spider Jerusalem\n");
    printf( "===============================================================================\n" );
    printf( "\n" );
}

void print_help()
{
    printf( "Description:\n" );
    printf( "============\n" );
  //printf( "===============================================================================\n" );
    printf( "- TODO - \n" );
    printf( "\n" );

    printf( "Usage:\n" );
    printf( "======\n" );
    printf( "   bin2src [options] {file}\n" );
    printf( "\n" );

  //printf( "===============================================================================\n" );
    printf( "Command line options:\n" );
    printf( "=====================\n" );
    printf( "   -a add     : add 8-bit offset to each byte.\n" );
    printf( "                Beware: byte + offset must not exceed 255 / 0xFF in total.\n" );
    printf( "                [default: 0]\n" );
    printf( "   -c count   : number of bytes to be processed (0 ~ all).\n" );
    printf( "                [default: 0]\n" );
    printf( "   -i spaces  : intendation of exported code (space characters).\n" );
    printf( "                [default: 4]\n" );
    printf( "   -k keyword : byte keyword for the assembly source. 8 chars max.\n" );
    printf( "                [default: !byte]\n" );
    printf( "   -o outfile : name of the output file.\n" );
    printf( "                [default: out.asm]\n" );
    printf( "   -s skip    : number of bytes to be skipped. I.e. set -s 2 for .PRG files\n" );
    printf( "                or -s 0x7e for .SID files.\n" );
    printf( "                [default: 0]\n" );
    printf( "\n" );
    printf( "Have fun!\n" );
}
