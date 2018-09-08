/* FILE: A2_bmp_helpers.c is where you will code your answers for Assignment 2.
 *
 * Each of the functions below can be considered a start for you.
 *
 * You should leave all of the code as is, except for what's surrounded
 * in comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A2_bmp_headers.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define INFO_LEN 4
#define MAX(a,b) (((a)>(b))?(a):(b))

void read_with_offset(FILE *file, int offset, void *dst, unsigned int size, unsigned int count) {
  fseek(file, offset, SEEK_SET);
  int code = fread(dst, size, count, file);
}

int bmp_open( char* bmp_filename,        unsigned int *width,
              unsigned int *height,      unsigned int *bits_per_pixel,
              unsigned int *padding,     unsigned int *data_size,
              unsigned int *data_offset, unsigned char** img_data ){


  // YOUR CODE FOR Q1 SHOULD REPLACE EVERYTHING FROM HERE
  FILE *bmp = fopen(bmp_filename, "r");
  read_with_offset(bmp, 2, data_size, INFO_LEN, 1);
  read_with_offset(bmp, 10, data_offset, INFO_LEN, 1);
  read_with_offset(bmp, 18, width, INFO_LEN, 1);
  read_with_offset(bmp, 22, height, INFO_LEN, 1);
  read_with_offset(bmp, 28, bits_per_pixel, INFO_LEN, 1);
  *padding = (4 - (((*bits_per_pixel) / 8 * (*width)) & 7)) % 4;
  unsigned int row_len = ((*bits_per_pixel) / 8 * (*width)) + (*padding);
  *img_data = malloc((*data_offset) + row_len * (*height));
  read_with_offset(bmp, 0, *img_data, (*data_offset) + row_len * (*height), 1);
  fclose(bmp);
  // TO HERE

  return 0;
}

// We've implemented bmp_close for you. No need to modify this function
void bmp_close( unsigned char **img_data ){

  if( *img_data != NULL ){
    free( *img_data );
    *img_data = NULL;
  }
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
  unsigned int data_offset;
  unsigned char* img_data    = NULL;

  int open_return_code = bmp_open( input_bmp_filename, &img_width, &img_height, &bits_per_pixel, &padding, &data_size, &data_offset, &img_data );

  if( open_return_code ){ printf( "bmp_open failed. Returning from bmp_mask without attempting changes.\n" ); return -1; }

  unsigned int num_colors = bits_per_pixel / 8;
  unsigned char *pixel_data = img_data + data_offset;
  // YOUR CODE FOR Q2 SHOULD REPLACE EVERYTHING FROM HERE
  for (int c=x_min; c<x_max; c++) {
    for (int r=y_min; r<y_max; r++) {
      pixel_data[ r*(img_width*num_colors+padding) + c*num_colors + 2 ] = red;
      pixel_data[ r*(img_width*num_colors+padding) + c*num_colors + 1 ] = green;
      pixel_data[ r*(img_width*num_colors+padding) + c*num_colors + 0 ] = blue;
    }
  }
  // TO HERE!
  FILE *bmp = fopen(output_bmp_filename, "w");
  unsigned int row_len = bits_per_pixel / 8 * img_width + padding;
  fwrite(img_data, sizeof(char), data_offset+row_len*img_height, bmp);
  bmp_close( &img_data );

  return 0;
}

int bmp_collage( char* bmp_input1, char* bmp_input2, char* bmp_result, int x_offset, int y_offset ){

  unsigned int img_width1;
  unsigned int img_height1;
  unsigned int bits_per_pixel1;
  unsigned int data_size1;
  unsigned int padding1;
  unsigned int data_offset1;
  unsigned char* img_data1    = NULL;

  int open_return_code = bmp_open( bmp_input1, &img_width1, &img_height1, &bits_per_pixel1, &padding1, &data_size1, &data_offset1, &img_data1 );

  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input1 ); return -1; }

  unsigned int img_width2;
  unsigned int img_height2;
  unsigned int bits_per_pixel2;
  unsigned int data_size2;
  unsigned int padding2;
  unsigned int data_offset2;
  unsigned char* img_data2    = NULL;

  open_return_code = bmp_open( bmp_input2, &img_width2, &img_height2, &bits_per_pixel2, &padding2, &data_size2, &data_offset2, &img_data2 );

  assert(bits_per_pixel1 == bits_per_pixel2);
  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input2 ); return -1; }

  // YOUR CODE FOR Q3 SHOULD REPLACE EVERYTHING FROM HERE
  if (x_offset < 0 || y_offset < 0) {
    int dx = x_offset < 0 ? -x_offset : x_offset;
    int dy = y_offset < 0 ? -y_offset : y_offset;
    unsigned int width = x_offset < 0 ? MAX(img_width2-dx, img_width1+dx) : MAX(dx+img_width2, img_width1);
    // printf("img_width2-dx: %d, img_width1+dx: %d\n",img_width2-dx, img_width1+dx);
    unsigned int height = y_offset < 0 ? MAX(img_height2-dy, img_height1+dy) : MAX(dy+img_height2, img_height1);
    // printf("img_width2-dx: %d, img_width1+dx: %d\n",img_height2-dy, img_height1+dy);
    // printf("w1: %d, w2: %d, w: %d\n", img_width1, img_width2, width);
    // printf("h1: %d, h2: %d, h: %d\n", img_height1, img_height2, height);
    unsigned int bits_per_pixel = bits_per_pixel2;
    unsigned int padding = (4 - ((bits_per_pixel / 8 * width) & 7)) % 4;
    // printf("padding: %d\n", padding);
    unsigned int row_len = (bits_per_pixel / 8 * width) + padding;
    unsigned int data_offset = data_offset2;
    unsigned int data_size = data_offset + row_len * height;
    unsigned char *img_data = malloc(data_size);
    unsigned char *pixel_data = img_data + data_offset;
    unsigned char *pixel_data1 = img_data1 + data_offset1;
    unsigned char *pixel_data2 = img_data2 + data_offset2;
    unsigned int num_colors = bits_per_pixel / 8;
    memcpy(img_data, img_data2, data_offset);
    memcpy(img_data+2, &data_size, sizeof(data_size));
    memcpy(img_data+18, &width, sizeof(width));
    memcpy(img_data+22, &height, sizeof(height));
    for (unsigned int r=0; r<img_height1; r++) {
      for (unsigned int c=0; c<img_width1; c++) {
        unsigned char red = pixel_data1[ r*(img_width1*num_colors+padding1) + c*num_colors + 2 ];
        unsigned char green = pixel_data1[ r*(img_width1*num_colors+padding1) + c*num_colors + 1 ];
        unsigned char blue = pixel_data1[ r*(img_width1*num_colors+padding1) + c*num_colors + 0 ];
        unsigned int y = y_offset < 0 ? r + dy : r;
        unsigned int x = x_offset < 0 ? c + dx : c;
        pixel_data[ y*row_len + x*num_colors + 2 ] = red;
        pixel_data[ y*row_len + x*num_colors + 1 ] = green;
        pixel_data[ y*row_len + x*num_colors + 0 ] = blue;
      }
    }
    for (unsigned int r=0; r<img_height2; r++) {
      for (unsigned int c=0; c<img_width2; c++) {
        unsigned char red = pixel_data2[ r*(img_width2*num_colors+padding2) + c*num_colors + 2 ];
        unsigned char green = pixel_data2[ r*(img_width2*num_colors+padding2) + c*num_colors + 1 ];
        unsigned char blue = pixel_data2[ r*(img_width2*num_colors+padding2) + c*num_colors + 0 ];
        unsigned int y = r;
        unsigned int x = c;
        pixel_data[ y*row_len + x*num_colors + 2 ] = red;
        pixel_data[ y*row_len + x*num_colors + 1 ] = green;
        pixel_data[ y*row_len + x*num_colors + 0 ] = blue;
      }
    }
    FILE *result = fopen(bmp_result, "w");
    fwrite(img_data, sizeof(char), data_size, result);
    fclose(result);
  } else {
    int dx = x_offset;
    int dy = y_offset;
    unsigned int width = (dx + img_width2) > img_width1 ? (dx + img_width2) : img_width1;
    unsigned int height = (dy + img_height2) > img_height1 ? (dy + img_height2) : img_height1;
    unsigned int bits_per_pixel = bits_per_pixel1;
    unsigned int padding = (4 - ((bits_per_pixel / 8 * width) & 7)) % 4;
    unsigned int row_len = (bits_per_pixel / 8 * width) + padding;
    unsigned int data_offset = data_offset2;
    unsigned int data_size = data_offset + row_len * height;
    unsigned char *img_data = malloc(data_size);
    unsigned char *pixel_data = img_data + data_offset;
    unsigned char *pixel_data1 = img_data1 + data_offset1;
    unsigned char *pixel_data2 = img_data2 + data_offset2;
    unsigned int num_colors = bits_per_pixel / 8;
    memcpy(img_data, img_data2, data_offset);
    memcpy(img_data+2, &data_size, sizeof(data_size));
    memcpy(img_data+18, &width, sizeof(width));
    memcpy(img_data+22, &height, sizeof(height));
    for (unsigned int r=0; r<img_height1; r++) {
      for (unsigned int c=0; c<img_width1; c++) {
        unsigned char red = pixel_data1[ r*(img_width1*num_colors+padding1) + c*num_colors + 2 ];
        unsigned char green = pixel_data1[ r*(img_width1*num_colors+padding1) + c*num_colors + 1 ];
        unsigned char blue = pixel_data1[ r*(img_width1*num_colors+padding1) + c*num_colors + 0 ];
        unsigned int y = r;
        unsigned int x = c;
        pixel_data[ y*row_len + x*num_colors + 2 ] = red;
        pixel_data[ y*row_len + x*num_colors + 1 ] = green;
        pixel_data[ y*row_len + x*num_colors + 0 ] = blue;
      }
    }
    for (unsigned int r=0; r<img_height2; r++) {
      for (unsigned int c=0; c<img_width2; c++) {
        unsigned char red = pixel_data2[ r*(img_width2*num_colors+padding2) + c*num_colors + 2 ];
        unsigned char green = pixel_data2[ r*(img_width2*num_colors+padding2) + c*num_colors + 1 ];
        unsigned char blue = pixel_data2[ r*(img_width2*num_colors+padding2) + c*num_colors + 0 ];
        unsigned int y = r + dy;
        unsigned int x = c + dx;
        pixel_data[ y*row_len + x*num_colors + 2 ] = red;
        pixel_data[ y*row_len + x*num_colors + 1 ] = green;
        pixel_data[ y*row_len + x*num_colors + 0 ] = blue;
      }
    }
    FILE *result = fopen(bmp_result, "w");
    fwrite(img_data, sizeof(char), data_size, result);
    fclose(result);
  }
  // TO HERE!

  bmp_close( &img_data1 );
  bmp_close( &img_data2 );

  return 0;
}
