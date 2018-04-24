#include "A4_sort_helpers.h"
sem_t *letter_sems[27];

void read_all( char *filename ){

	FILE *fp = fopen( filename, "r" );
	int curr_line = 0;
	
	while( curr_line < MAX_NUMBER_LINES && 
           fgets( text_array[curr_line], MAX_LINE_LENGTH, fp ) )
    {
		curr_line++;
	}
	
	text_array[curr_line][0] = '\0';
	fclose(fp);
}

void read_by_letter( char *filename, char first_letter ){

	FILE *fp = fopen( filename, "r" );
	int curr_line = 0;
	text_array[curr_line][0] = '\0';
	
	while( fgets( text_array[curr_line], MAX_LINE_LENGTH, fp ) ){
		if( text_array[curr_line][0] == first_letter ){
			curr_line++;
		}

		if( curr_line == MAX_NUMBER_LINES ){
			sprintf( buf, "ERROR: Attempted to read too many lines from file.\n" );
			write( 1, buf, strlen(buf) );
			break;
		}
	}
	
	text_array[curr_line][0] = '\0';
	fclose(fp);
}

void write_sorted( char *filename ){

	FILE *fp = fopen( filename, "a" );

	if( fp == NULL ){
		sprintf( buf, "ERROR: Unable to open file %s.\n",  filename );
		write(1,buf,strlen(buf));	
	}
	int curr_line = 0;
	while( text_array[curr_line][0] != '\0' ){
		sprintf( buf, "%s",  text_array[curr_line] );
		fwrite(buf,strlen(buf),1,fp);
		curr_line++;
	}
	fclose(fp);
}

void sort_words( ){

	int temp;
	int curr_line = 0;

	while( text_array[curr_line][0] != '\0' ){
		
		char temp[MAX_LINE_LENGTH];
		strcpy( temp, text_array[curr_line] );
		
		char *min="zzzzzzzzzzzzzzzzzzzzzzz";
		int min_spot = -1;
		int curr_line2 = curr_line;

		while( text_array[curr_line2][0] != '\0' ){

			if( strcmp( text_array[curr_line2], min ) < 0 ){
				min      = text_array[curr_line2];
				min_spot = curr_line2;
			}
			curr_line2++;
		}

		if( min_spot != -1 ){
			strcpy( text_array[curr_line], min);
			strcpy( text_array[min_spot], temp );
		}
		curr_line++;
	}
}

int initialize( ){

    FILE *fp = fopen( "temp.txt", "w" );
	fclose(fp);

	for( int letter=0; letter<27; letter++ ){
		char sem_name[2];
		sprintf( sem_name, "sorter_%c", 'a'+letter );
		sem_unlink(sem_name);
		letter_sems[letter] = sem_open( sem_name, O_CREAT | O_EXCL, 0666, 0 );

		if( letter_sems[letter] == SEM_FAILED ){
			sprintf(buf, "Oh dear, something went wrong with read()! %s\n", strerror(errno));
			write(1,buf,strlen(buf));
			return 1;
		}
	}
	sem_post( letter_sems[1] );
    return 0;
}

int process_by_letter( char* input_filename, char first_letter ){

    int id = first_letter - 'a' + 1;
    sem_wait(letter_sems[id]);

    sprintf(buf, "I will sort first letter %c.\n", first_letter  );
    //write(1, buf, strlen(buf));

    read_by_letter( input_filename, first_letter );
    sort_words( text_array );
    write_sorted( "temp.txt" );

    if( id < 26 )
    {
        sem_post(letter_sems[id+1]);		
    }
    else{
        sem_post(letter_sems[0]);
    }
}

int finalize( ){
    
    sem_wait(letter_sems[0]);

    FILE *fp = fopen( "temp.txt", "r" );
    char line[MAX_LINE_LENGTH];
    while( fgets( line, MAX_LINE_LENGTH, fp ) )
        write(1, line, strlen(line));
    fclose(fp);

    sprintf(buf, "Sorting complete!\n" );
	write(1, buf, strlen(buf));
    return 0;
}
