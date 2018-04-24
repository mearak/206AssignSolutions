#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCN 1000
#define NUMDAYS 7
#define NUMMONTHS 12
#define MAXLABEL 100

int main( int argc, char *argv[] )
{
	if( argc < 3 )
	{
		printf( "Usage: %s day_spacing first_day\n", argv[0] );
		return EXIT_FAILURE;
	}


	int DAYSIZE;
	DAYSIZE = atoi(argv[1])+4;	
	
	int day_of_week = atoi(argv[2]) - 1;

	char days[7][100] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	char months[12][100] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

	int row;
	int calwidth = (DAYSIZE-1)*NUMDAYS+1;
	int day;
	int month;
	int this_day = 0;

	for( month=0; month<NUMMONTHS; month++ )
	{
		printf( "|" );
		for( row=0; row<calwidth-2; row++ )
		{
			printf( "-" );
		}

		printf( "|\n" );
		
		int month_len = strlen(months[month]);
		int first_blanks = (calwidth-month_len-2)/2;
		
		printf( "|" );
		for(row=0; row<first_blanks; row++)
			printf(" ");
		printf( "%s", months[month] );

		for(row=first_blanks+month_len; row<calwidth-2; row++)
			printf(" ");

		printf("|\n|");
		for( row=0; row<calwidth-2; row++ )
		{
			printf( "-" );
		}

		printf( "|\n" );

		for( day=0; day<NUMDAYS; day++ )
		{
			printf("| ");
			for( row=0; row<DAYSIZE-4 && row<strlen(days[day]); row++ )
				printf( "%c", days[day][row] );

			for( ; row<DAYSIZE-3; row++ )
				printf( " " );		
	
		}
		printf("|\n|");
		for( row=0; row<calwidth-2; row++ )
		{
			printf( "-" );
		}

		printf( "|\n" );

		int blank_start_days = day_of_week;
		for( blank_start_days = day_of_week; blank_start_days > 0; blank_start_days-- )
		{
			printf( "|" );
			for( row=0; row<DAYSIZE-2; row++ )
				printf(" ");
		}

		this_day = 1;
		while( this_day<=30 )
		{
			printf( "| " );
			printf( "%d", this_day++ );	
			
			for( row=0; row<DAYSIZE-3-( this_day>10 ? 2 : 1); row++ )
				printf(" ");	
			day_of_week++;

			if( day_of_week == 7 )
			{
				day_of_week = 0;
				if( this_day <= 30 )
					printf("|\n");
	
			}
		} 

		int blank_end_days = day_of_week == 0 ? 8 : day_of_week;
		for( ; blank_end_days < 7; blank_end_days++ )
		{
			printf( "|" );
			for( row=0; row<DAYSIZE-2; row++ )
				printf(" ");

		}

		printf("|\n");

	}

	printf("|");
	for( row=0; row<calwidth-2; row++ )
	{
		printf( "-" );
	}
	printf("|\n");


	
	return EXIT_SUCCESS;

}
