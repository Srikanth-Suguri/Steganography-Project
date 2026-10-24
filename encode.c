#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>



 Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2], ".bmp")==NULL)
    {
        printf("ERROR: Source image should be .bmp file\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];
    if(strstr(argv[3],".txt")==NULL)
    {
        printf("ERROR: Secret file should be .txt file\n");
        return e_failure;
    }
    encInfo->secret_fname = argv[3];
    if(argv[4] != NULL)
    {
        if(strstr(argv[4], ".bmp") == NULL)
        {
            printf("ERROR: Output image should be .bmp file\n");
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    return e_success;
}


/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	fprintf(stderr, "ERROR: Unable to open file %s\n",encInfo->src_image_fname);
    	return e_failure;
    }

    printf("INFO: Opened %s\n", encInfo->src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	fprintf(stderr, "ERROR: Unable to open file %s\n",encInfo->secret_fname);
    	return e_failure;
    }

    printf("INFO: Opened %s\n", encInfo->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	fprintf(stderr, "ERROR: Unable to open file %s\n",encInfo->stego_image_fname);
    	return e_failure;
    }

    printf("INFO: Opened %s\n", encInfo->stego_image_fname);
    printf("INFO: Done\n");

    return e_success;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */


uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    rewind(fptr_image);

    return width * height * 3;
}




//get the secret file size

uint get_file_size(FILE *fptr)
{
    uint size;

    fseek(fptr, 0, SEEK_END);
    size = ftell(fptr);
    rewind(fptr);

    return size;
}

//check_capacity

Status check_capacity(EncodeInfo *encInfo)
{
    uint src_size;
    uint secret_size;

    src_size = encInfo->image_capacity;
    secret_size = encInfo->size_secret_file;

    if(src_size >= (16 + (sizeof(int) * 8) +
                    (4 * 8) + (sizeof(long) * 8) +
                    secret_size * 8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

//copy the bmp header

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char copy_header[54];

    fread(copy_header, 54, 1, fptr_src_image);
    fwrite(copy_header, 54, 1, fptr_dest_image);

    return e_success;
}


//encode magic string


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char image_buffer[8];

    for(int i = 0; magic_string[i] != '\0'; i++)
    {
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(magic_string[i], image_buffer);

        fwrite(image_buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

//extension size of sexret file
int extn_size_of_secret_file(const char *file_extn)
{
    int size;

    size = strlen(file_extn);

    return size;
}


//encode the secret file extension
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char image_buffer[8];

    for(int i = 0; file_extn[i] != '\0'; i++)
    {
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(file_extn[i], image_buffer);

        fwrite(image_buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

//encode_secret_file_size

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb((char *)&file_size, sizeof(long),
                       encInfo->fptr_src_image,
                       encInfo->fptr_stego_image);

    return e_success;
}


Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char image_buffer[8];

    while(fread(&ch, 1, 1, encInfo->fptr_secret))
    {
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);

        encode_byte_to_lsb(ch, image_buffer);

        fwrite(image_buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

//encode size to lsb

Status encode_size_to_lsb(char *data, int size,
                          FILE *fptr_src_image,
                          FILE *fptr_stego_image)
{
    char image_buffer[8];

    for(int i = 0; i < size; i++)
    {
        fread(image_buffer, 8, 1, fptr_src_image);

        encode_byte_to_lsb(data[i], image_buffer);

        fwrite(image_buffer, 8, 1, fptr_stego_image);
    }

    return e_success;
}



Status encode_byte_to_lsb(char data, char *image_buffer)
{
    unsigned char mask = 128;

    for(int i = 0; i < 8; i++)
    {
        image_buffer[i] = image_buffer[i] & 0xFE;

        if(data & mask)
        {
            image_buffer[i] = image_buffer[i] | 1;
        }

        mask = mask / 2;
    }

    return e_success;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while(fread(&ch, 1, 1, fptr_src))
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }

    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    char *file_extn;
    int extn_size;

    printf("INFO: ## Encoding Procedure Started ##\n");

    printf("INFO: Checking for %s size\n", encInfo->secret_fname);

    encInfo->image_capacity =get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file =get_file_size(encInfo->fptr_secret);

    if(encInfo->size_secret_file == 0)
    {
        printf("ERROR: Secret file is empty\n");
        return e_failure;
    }
    else
    {
        printf("INFO: Done. Not Empty\n");
    }

    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);

    if(check_capacity(encInfo) == e_failure)
    {
        printf("ERROR: \"%s\" doesn't have the capacity to encode \"%s\"\n",encInfo->src_image_fname,encInfo->secret_fname);
        return e_failure;
    }

    printf("INFO: Done. Found OK\n");

    if(strcmp(encInfo->stego_image_fname, "stego.bmp") == 0)
    {
        printf("INFO: Output File not mentioned. Creating stego.bmp as default\n");
    }

    printf("INFO: Copying Image Header\n");

    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    printf("INFO: Encoding Magic String Signature\n");

    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    file_extn = strrchr(encInfo->secret_fname, '.');

    extn_size = extn_size_of_secret_file(file_extn);

    printf("INFO: Encoding %s File Extension\n",encInfo->secret_fname);

    encode_size_to_lsb((char *)&extn_size, sizeof(int),encInfo->fptr_src_image,encInfo->fptr_stego_image);

    if(encode_secret_file_extn(file_extn, encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    printf("INFO: Encoding %s File Size\n",encInfo->secret_fname);

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    printf("INFO: Encoding %s File Data\n",encInfo->secret_fname);

    if(encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    printf("INFO: Copying Left Over Data\n");

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Done\n");

    printf("INFO: ## Encoding Done Successfully ##\n");

    return e_success;
}