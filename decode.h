
#include<stdio.h>
#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output File Info */
    char *output_fname;
    FILE *fptr_output;

} DecodeInfo;


/* Decoding function prototypes */

OperationType check_operation_type(int argc, char *argv[]);

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status open_decode_files(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_byte_from_lsb(char *image_buffer, char *data);

Status decode_magic_string(DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *extn_size);

Status decode_secret_file_extn(char *file_extn, int extn_size, DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo, long *file_size);

Status decode_secret_file_data(DecodeInfo *decInfo, long file_size);




#endif