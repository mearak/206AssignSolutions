#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void do_output( int rmax, int nmax, float a, float b){

	FILE *shape_file = fopen( "shape_template.txt", "r" );
	FILE *output_file = stdout; //fopen( "shape_out.txt", "w" );

	char line_in[1000];

	while( fgets(line_in,1000,shape_file) ){
	
		char *pos = line_in;

		while(*pos){

			if( *pos == '#' ){
				char tok_name[100];
				char *tok_pos = tok_name;
				pos++;
				while( *pos && *pos != '#' ){
					*tok_pos++ = *pos++;
				}

				if( *pos != '#' ){
					printf( "Token parsing failed!\n" );
					exit(-1);
				}

				pos++;
				*tok_pos = '\0';

				if( strcmp( tok_name, "RMAX" ) == 0 ){
					fprintf( output_file, "%d", rmax );
				}
				else if( strcmp( tok_name, "NMAX" ) == 0 ){ 
					fprintf( output_file, "%d", nmax );
				}
				else if( strcmp( tok_name, "A" ) == 0 ){ 
					fprintf( output_file, "%f", a );
				}
				else if( strcmp( tok_name, "B" ) == 0 ){ 
					fprintf( output_file, "%f", b );
				}

			}
			
			fputc(*pos,output_file);
			pos++;
		}
	}

	fclose(shape_file);
	//fclose(output_file);

	//system( "gnuplot -p shape_out.txt" );
}

int main( int argc, char *argv[] ){
	
	int     rmax = 5;
	int     nmax = 100;
	float	a = atof(argv[2]);
	float	b = atof(argv[3]);


	do_output(rmax, nmax, a, b);

	/*
	printf( "Enter new values for a and b separated by spaces: \n" );
	
	while(scanf("%f %f", &a, &b)){

		printf( "Current displayed values are: a=%f, b=%f.\n", a, b );	
		do_output(rmax, nmax, -a/100.0, -b/100.0);
		printf( "Enter new values for a and b separated by spaces: \n" );
	}
	*/
	return 0;
}
