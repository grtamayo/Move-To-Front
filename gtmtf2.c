/*
Filename:     gtmtf2.c, move-to-front
Written by:   Gerald R. Tamayo, (January 20, 2023).
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  /* C99 */
#include <string.h>
#include <time.h>
#include "gtbitio2.c"
#include "mtf.c"

#define MTF_SIZE 256

enum {
	/* modes */
	COMPRESS,
	DECOMPRESS,
};

void copyright( void );
void   compress( void );
void decompress( void );

void usage( void )
{
	fprintf(stderr, "\n Usage: gtmtf2 c|d infile outfile\n"
		"\n Commands:\n  c = Encoding.\n  d = Decoding.\n"
	);
	copyright();
	exit(0);
}

int main( int argc, char *argv[] )
{
	float ratio = 0.0;
	int mode = -1;
	char *cmd = NULL;
	
	clock_t start_time = clock();
	
	if ( argc != 4 ) usage();
	init_buffer_sizes( (1<<15) );
	
	cmd = argv[1];
	while ( cmd ){
		switch( *cmd ) {
			case 'c':
				if ( mode == -1 ) mode = COMPRESS; else usage();
				if ( *(cmd+1) != 0 ) usage(); cmd++; break;
			case 'd':
				if ( mode == -1 ) mode = DECOMPRESS; else usage();
				if ( *(cmd+1) != 0 ) usage(); cmd++; break;
			case 0: cmd = NULL; break;
			default : usage();
		}
	}
	
	if ( (gIN=fopen( argv[2], "rb" )) == NULL ) {
		fprintf(stderr, "\nError opening input file.");
		return 0;
	}
	
	if ( (pOUT=fopen( argv[3], "wb" )) == NULL ) {
		fprintf(stderr, "\nError opening output file.");
		return 0;
	}
	
	alloc_mtf(MTF_SIZE);
	
	if ( mode == COMPRESS ){
		init_get_buffer();
		init_put_buffer();
		fprintf(stderr, "\n MTF encoding ( %s to %s ) ...", 
			argv[2], argv[3] );
		compress();
	}
	else if ( mode == DECOMPRESS ){
		init_get_buffer();
		init_put_buffer();
		fprintf(stderr, "\n Decoding ( %s to %s ) ...", 
			argv[2], argv[3] );
		decompress();
	}
	flush_put_buffer();
	
	fprintf(stderr, "done. ");
	
	free_get_buffer();
	free_put_buffer();
	free_mtf_table();
	if ( gIN ) fclose( gIN );
	if ( pOUT ) fclose( pOUT );
	
	fprintf(stderr, " in %3.2f secs.\n", (double) (clock()-start_time) / CLOCKS_PER_SEC );
	return 0;
}

void copyright( void )
{
	fprintf(stderr, "\n Written by: Gerald R. Tamayo, 2023\n");
}

void compress( void )
{
	int c;
	
	while ( (c=gfgetc()) != EOF ){
		pfputc(mtf(c));
	}
}

void decompress( void )
{
	int c;
	
	while ( (c=gfgetc()) != EOF ){
		pfputc(get_mtf_c(c));
	}
}
