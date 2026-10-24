#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

OperationType check_operation_type(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Invalid Input,Please provide Valid Input\n");
        return e_unsupported;
    }

    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }

    if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }

    return e_unsupported;
}

int main(int argc, char *argv[])
{
    OperationType operation;

    operation = check_operation_type(argc, argv);

    if(operation == e_encode)
    {
        EncodeInfo encInfo;

        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            if(open_files(&encInfo) == e_success)
            {
                if(do_encoding(&encInfo) == e_success)
                {
                    printf("Encoding completed successfully\n");
                }
                else
                {
                    printf("Encoding failed\n");
                }
            }
            else
            {
                printf("Failed to open files\n");
            }
        }
        else
        {
            printf("Invalid encoding arguments\n");
        }
    }
    else if(operation == e_decode)
{
    DecodeInfo decInfo;

    if(read_and_validate_decode_args(argv, &decInfo) == e_success)
    {
        if(open_decode_files(&decInfo) == e_success)
        {
            if(do_decoding(&decInfo) == e_success)
            {
                printf("Decoding completed successfully\n");
            }
            else
            {
                printf("Decoding failed\n");
            }
        }
        else
        {
            printf("Failed to open files\n");
        }
    }
    else
    {
        printf("Invalid decoding arguments\n");
    }
}
    else
    {
        printf("Unsupported operation\n");
    }

    return 0;
}