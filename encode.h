#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    // uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    // char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(int argc,char *argv[]); //1

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo); //2

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);   //3 all ready code there

/* check capacity */
Status check_capacity(EncodeInfo *encInfo); //6

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);//4

/* Get file size */
uint get_file_size(FILE *fptr); //5

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image); //7

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);  //8

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo); //10

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo); //11

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);//12

/* Encode function, which does the real encoding */
Status encode_size_to_lsb(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);//int

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer); //char

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

// create an extra function extn_size_of_secret_file //9

int extn_size_of_secret_file(const char *file_extn); //9
#endif
