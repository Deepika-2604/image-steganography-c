#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include <math.h>

OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Invalid command line\n");
        printf("./a.out -e beautiful.bmp secret.txt\n");
        printf("./a.out -e beautiful.bmp secret.txt sample.bmp\n");
        printf("./a.out -d steno.bmp\n");
        return 1;
    }

    if (check_operation_type(argv) == e_encode)
    {
        // encodeing
        EncodeInfo encInfo;
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            do_encoding(&encInfo);
        }
        else
        {
            printf("Invalid command line for Encoding\n");
        
            exit(0);
        }
    }
    else if (check_operation_type(argv) == e_decode)
    {
        DecodeInfo decInfo;
        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            do_decoding(&decInfo);
        }
    }
    else
    {
        printf("Invalid command line\n");
        
    }
    
    return 0;
}
