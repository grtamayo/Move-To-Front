/*
Filename:     gtmtf3.c, move-to-front
Written by:   Gerald R. Tamayo, (August 4, 2023).
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

unsigned char *block;

void copyright( void );
void compress( unsigned char * block );
void decompress( unsigned char * block );

void usage( void )
{
	fprintf(stderr, "\n Usage: gtmtf3 c|d infile outfile\n"
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
	init_put_buffer();
	
	alloc_mtf(MTF_SIZE);
	block = (unsigned char *) malloc( sizeof(unsigned char) * 32768 );
	if ( !block ) {
		fprintf(stderr, "\n Error alloc: read block.");
		exit (0);
	}
	
	if ( mode == COMPRESS ){
		fprintf(stderr, "\n MTF encoding ( %s to %s ) ...", argv[2], argv[3]);
		compress(block);
	}
	else if ( mode == DECOMPRESS ){
		fprintf(stderr, "\n Decoding ( %s to %s ) ...",  argv[2], argv[3]);
		decompress(block);
	}
	flush_put_buffer();
	
	fprintf(stderr, "done. ");
	
	free_put_buffer();
	free_mtf_table();
	fclose( gIN );
	fclose( pOUT );
	
	fprintf(stderr, " in %3.2f secs.\n", (double) (clock()-start_time)/CLOCKS_PER_SEC );
	return 0;
}

void copyright( void )
{
	fprintf(stderr, "\n Written by: Gerald R. Tamayo, 2023\n");
}

void compress( unsigned char * block )
{
	int c, n, nread;
	
	while ( (nread=fread(block, 1, 32768, gIN)) ) {
		n = 0;
		while ( n < nread ){
			c = block[n];
			pfputc( mtf(c) );
			n++;
		}
	}
}

void decompress( unsigned char *block )
{
	int c, n, nread;
	
	while ( (nread=fread(block, 1, 32768, gIN)) ) {
		n = 0;
		while ( n < nread ){
			c = block[n];
			pfputc( get_mtf_c(c) );
			n++;
		}
	}
}
