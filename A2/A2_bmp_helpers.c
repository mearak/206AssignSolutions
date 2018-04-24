#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
     
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })     

int bmp_open( char* bmp_filename, unsigned int *width, unsigned int *height, unsigned int *bits_per_pixel, unsigned int *padding, unsigned int *data_size, unsigned int *header_size, unsigned char** img_data ){

  FILE *img_in = fopen( bmp_filename, "rb" );
  
  if( img_in == NULL ){ fprintf( stderr, "Error: could not open bmp file %s for reading.\n", bmp_filename ); return -1; }
  
  unsigned char header_data[6];
  size_t result = fread( header_data, 1, 6, img_in );

  if (result != 6) {fprintf(stderr,"Error reading BMP header for file %s.\n",bmp_filename); return -1;}
  
  if( header_data[0] != 'B' || header_data[1] != 'M' ){fprintf(stderr,"Error: BMP file %s did not start with BM.\n",bmp_filename); return -1;}
  
  *data_size = *((unsigned int*)(header_data+2)); 

  // This tells us how to allocate in and out buffers
  *img_data = (char*)malloc(sizeof(unsigned char)* (*data_size));

  // Skip back to the start and read the full thing
  fseek( img_in, 0, SEEK_SET );
  result = fread( *img_data, 1, *data_size, img_in );
  
  if( result != *data_size ){ fprintf(stderr, "Error reading BMP data for file %s. Read %lu bytes but wanted to read %d.\n", bmp_filename, result, *data_size); return -1;}
  
  *header_size                    = *((unsigned int*)(*img_data+10));
  *width                          = *((unsigned int*)(*img_data+18));
  *height                         = *((unsigned int*)(*img_data+22));
  *bits_per_pixel                 = *((unsigned int*)(*img_data+28));

  *padding = 0;
  // Each row needs to be a multiple of 4 bytes.  
  if ((*width * 3) % 4 != 0) *padding = 4 - ((*width * 3) % 4);

  fclose(img_in);
 
  return 0;
}

void bmp_close( unsigned char **img_data ){

  free( *img_data );
  *img_data = NULL;
}

int bmp_mask( char* input_bmp_filename, char* output_bmp_filename, 
              unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned int y_max,
              unsigned char red, unsigned char green, unsigned char blue )
{

  unsigned int img_width;
  unsigned int img_height;
  unsigned int bits_per_pixel;
  unsigned int data_size;
  unsigned int padding;
  unsigned int header_size;
  unsigned char* img_data    = NULL;
  
  int open_return_code = bmp_open( input_bmp_filename, &img_width, &img_height, &bits_per_pixel, &padding, &data_size, &header_size, &img_data ); 
  
  if( open_return_code ){ printf( "bmp_open failed. Returning from bmp_mask without attempting changes.\n" ); return -1; }
  
  unsigned int num_colors = bits_per_pixel / 8;
  
  if( num_colors < 3 ){ fprintf( stderr, "Error: the bmp file %s does not contain RGB colors. Cannot apply mask.\n", input_bmp_filename ); return -1; }
  
  if( y_max > img_height-1 ){ fprintf( stderr, "Error: the y_max value requested %d is larger than image height minus 1 %d.\n", y_max, img_height-1 ); return -1; }
  
  if( x_max > img_width-1 ){ fprintf( stderr, "Error: the x_max value requested %d is larger than the image width minus 1 %d.\n", x_max, img_width-1 ); return -1; }
  
  unsigned char out_img_data[data_size];
  
  memcpy( out_img_data, img_data, data_size );
  
  unsigned char* out_pixels = out_img_data + header_size;
  unsigned int row, col, color;
  for( row=y_min; row<=y_max; row++ )
    for( col=x_min; col<x_max; col++ )
      for( color=0; color<num_colors; color++ )
      {
        if( color==0 ) out_pixels[(row)*(img_width*num_colors+padding)+(col)*num_colors+color] = red;
        if( color==1 ) out_pixels[(row)*(img_width*num_colors+padding)+(col)*num_colors+color] = green;
        if( color==2 ) out_pixels[(row)*(img_width*num_colors+padding)+(col)*num_colors+color] = blue;
      }

  FILE* img_out = fopen( output_bmp_filename, "wb" );
  
  if( img_out == NULL ){ fprintf( stderr, "Error: failed to open output file %s for writing.\n", output_bmp_filename ); return -1; }
  
  fwrite( out_img_data, 1, data_size, img_out );
  fclose(img_out); 
  bmp_close( &img_data );
  
  return 0;
}         

int bmp_collage( char* bmp_input1, char* bmp_input2, char* bmp_result, int x_offset, int y_offset ){

  unsigned int img_width1;
  unsigned int img_height1;
  unsigned int bits_per_pixel1;
  unsigned int data_size1;
  unsigned int padding1;
  unsigned int header_size1;
  unsigned char* img_data1    = NULL;
  
  int open_return_code = bmp_open( bmp_input1, &img_width1, &img_height1, &bits_per_pixel1, &padding1, &data_size1, &header_size1, &img_data1 ); 
  
  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input1 ); return -1; }
  
  unsigned char *in_pixels1 = img_data1 + header_size1;
  unsigned int num_colors1 = bits_per_pixel1 / 8;

  unsigned int img_width2;
  unsigned int img_height2;
  unsigned int bits_per_pixel2;
  unsigned int data_size2;
  unsigned int padding2;
  unsigned int header_size2;
  unsigned char* img_data2    = NULL;
  
  open_return_code = bmp_open( bmp_input2, &img_width2, &img_height2, &bits_per_pixel2, &padding2, &data_size2, &header_size2, &img_data2 ); 
  
  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input2 ); return -1; }
  
  unsigned int num_colors2 = bits_per_pixel2 / 8;
  
  if( num_colors1 != num_colors2 ){ fprintf( stderr, "Error: the two bmp input files do not have the same number of colors so bmp_collage is not possible.\n" ); return -1; }
  
  unsigned char *in_pixels2 = img_data2 + header_size2;
  
  int min_left   = min( 0, x_offset );
  int max_right  = max( img_width1, x_offset + img_width2 );
  int min_top    = min( 0, y_offset );
  int max_bottom = max( img_height1, y_offset + img_height2 ); 
  
  int output_width = max_right - min_left;
  assert( output_width >= 0 );
  
  int padding = 0;
  // Each row needs to be a multiple of 4 bytes.  
  if ((output_width * 3) % 4 != 0) padding = 4 - ((output_width * 3) % 4);

  int output_height = max_bottom - min_top;
  assert( output_height >= 0 );
  
  int output_data_size = header_size1 + output_height * (output_width*num_colors1 + padding);
  
  unsigned char out_img_data[output_data_size];
  
  memcpy( out_img_data, img_data1, header_size1 );
  memcpy( out_img_data+2, &output_data_size, sizeof(unsigned int) );
  memcpy( out_img_data+18, &output_width, sizeof(unsigned int) );
  memcpy( out_img_data+22, &output_height, sizeof(unsigned int) );
  
  unsigned char* out_pixels = out_img_data + header_size1;
  
  unsigned int out_row, out_col, color;
  unsigned char zeros[num_colors1];
  for( color=0; color<num_colors1; color++)
    zeros[color] = 0;
  
  for( out_row=0; out_row<output_height; out_row++ )
    for( out_col=0; out_col<output_width; out_col++ ){
    
        int in_row2 = out_row + min_top - y_offset;
        int in_col2 = out_col + min_left - x_offset;
        
        int in_row1 = out_row + min_top;
        int in_col1 = out_col + min_left;
        
        unsigned char *source=NULL;
        
        if( in_row2 > 0 && in_row2 < img_height2 && in_col2 > 0 && in_col2 < img_width2 )
            source = in_pixels2 + (in_row2)*(img_width2*num_colors2+padding2)+(in_col2)*num_colors2;
        else if( in_row1 > 0 && in_row1 < img_height1 && in_col1 > 0 && in_col1 < img_width1 )
          source = in_pixels1 + (in_row1)*(img_width1*num_colors1+padding1)+(in_col1)*num_colors1;
        else
          source = zeros;
          
        for( color=0; color<num_colors1; color++ )
          out_pixels[(out_row)*(output_width*num_colors1+padding)+(out_col)*num_colors1+color] = *(source+color);
    }
      
  bmp_close( &img_data1 );
  bmp_close( &img_data2 );
  
  FILE* img_out = fopen( bmp_result, "wb" );
  
  if( img_out == NULL ){ fprintf( stderr, "Error: failed to open output file %s for writing.\n", bmp_result ); return -1; }
  
  fwrite( out_img_data, 1, output_data_size, img_out );
  
  return 0;
    
}                  
