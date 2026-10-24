#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR: Input image should be .bmp file\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    if(argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = NULL;
    }

    return e_success;
}


Status open_decode_files(DecodeInfo *decInfo)
{
    printf("INFO: Opening required files\n");

    decInfo->fptr_stego_image =fopen(decInfo->stego_image_fname, "r");

    if(decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        printf("ERROR: Unable to open file %s\n",decInfo->stego_image_fname);

        return e_failure;
    }

    printf("INFO: Opened %s\n",decInfo->stego_image_fname);

    return e_success;
}

Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    char value = 0;
    unsigned char mask = 128;

    for(int i = 0; i < 8; i++)
    {
        if(image_buffer[i] & 1)
        {
            value = value | mask;
        }

        mask = mask / 2;
    }

    *data = value;

    return e_success;
}


Status do_decoding(DecodeInfo *decInfo)
{
    int extn_size;
    char file_extn[20];
    long file_size;

    printf("INFO: ## Decoding Procedure Started ##\n");

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    printf("INFO: Decoding Magic String Signature\n");

    if(decode_magic_string(decInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    printf("INFO: Decoding Output File Extension\n");

    if(decode_secret_file_extn_size(decInfo, &extn_size) == e_failure)
    {
        return e_failure;
    }

    if(decode_secret_file_extn(file_extn, extn_size, decInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    if(decInfo->output_fname == NULL)
    {
        decInfo->output_fname = "decoded.txt";

        printf("INFO: Output File not mentioned. Creating decoded.txt as default\n");
    }

    decInfo->fptr_output = fopen(decInfo->output_fname, "w");

    if(decInfo->fptr_output == NULL)
    {
        perror("fopen");

        printf("ERROR: Unable to create file %s\n",decInfo->output_fname);

        return e_failure;
    }

    printf("INFO: Opened %s\n", decInfo->output_fname);
    printf("INFO: Done. Opened all required files\n");

    printf("INFO: Decoding File Size\n");

    if(decode_secret_file_size(decInfo, &file_size) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    printf("INFO: Decoding File Data\n");

    if(decode_secret_file_data(decInfo, file_size) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    printf("INFO: ## Decoding Done Successfully ##\n");

    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data;

    for(int i = 0; i < 2; i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);

        decode_byte_from_lsb(image_buffer, &data);

    }

    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *extn_size)
{
    char image_buffer[8];

    for(int i = 0; i < sizeof(int); i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);

        decode_byte_from_lsb(image_buffer, (char *)extn_size + i);
    }

    return e_success;
}


Status decode_secret_file_extn(char *file_extn, int extn_size, DecodeInfo *decInfo)
{
    char image_buffer[8];

    for(int i = 0; i < extn_size; i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);

        decode_byte_from_lsb(image_buffer, &file_extn[i]);
    }

    file_extn[extn_size] = '\0';

    return e_success;
}


Status decode_secret_file_size(DecodeInfo *decInfo, long *file_size)
{
    char image_buffer[8];

    for(int i = 0; i < sizeof(long); i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);

        decode_byte_from_lsb(image_buffer, (char *)file_size + i);
    }

    return e_success;
}


Status decode_secret_file_data(DecodeInfo *decInfo, long file_size)
{
    char image_buffer[8];
    char data;

    for(int i = 0; i < file_size; i++)
    {
        fread(image_buffer, 8, 1, decInfo->fptr_stego_image);

        decode_byte_from_lsb(image_buffer, &data);

        fwrite(&data, 1, 1, decInfo->fptr_output);
    }

    return e_success;
}