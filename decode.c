#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] && strstr(argv[2], ".bmp"))
    {
        decInfo->stego_image_fname = argv[2];

        if (argv[3])
            decInfo->output_fname = argv[3];
        else
            decInfo->output_fname = "output";

        return e_success;
    }

    return e_failure;
}

Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("Stego image open failed\n");
        return e_failure;
    }

    return e_success;
}

int decode_size_from_lsb(char *image_data)
{
    int size = 0;
    for (int i = 0; i < 32; i++)
        size |= ((image_data[i] & 1) << i);
    return size;
}

char decode_byte_from_lsb(char *image_data)
{
    char ch = 0;
    for (int i = 0; i < 8; i++)
        ch |= ((image_data[i] & 1) << i);
    return ch;
}

Status decode_magic_string_size(DecodeInfo *decInfo)
{
    char arr[32];

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    fread(arr, 1, 32, decInfo->fptr_stego_image);

    decInfo->magic_string_size = decode_size_from_lsb(arr);
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char arr[8];

    for(int i = 0; i < decInfo->magic_string_size; i++)
    {
        fread(arr,1,8,decInfo->fptr_stego_image);
        decInfo->magic_string[i] = decode_byte_from_lsb(arr);
    }

    decInfo->magic_string[decInfo->magic_string_size] = '\0';

    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char arr[32];

    fread(arr, 1, 32, decInfo->fptr_stego_image);
    decInfo->extn_size = decode_size_from_lsb(arr);

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char arr[8];

    for(int i = 0; i < decInfo->extn_size; i++)
    {
        fread(arr,1,8,decInfo->fptr_stego_image);
        decInfo->extn_secret_file[i] = decode_byte_from_lsb(arr);
    }

    decInfo->extn_secret_file[decInfo->extn_size] = '\0';
    return e_success;
}


Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char arr[32];

    fread(arr, 1, 32, decInfo->fptr_stego_image);
    decInfo->secret_file_size = decode_size_from_lsb(arr);

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char arr[8];
    char ch;

    for (int i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(arr, 1, 8, decInfo->fptr_stego_image);
        ch = decode_byte_from_lsb(arr);
        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_files_decode(decInfo) != e_success)
        return e_failure;

    decode_magic_string_size(decInfo);
    decode_magic_string(decInfo);


    if (strcmp(decInfo->magic_string, MAGIC_STRING) != 0)
    {
        printf("Magic string mismatch\n");
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }

    decode_secret_file_extn_size(decInfo);
    decode_secret_file_extn(decInfo);



    strcpy(decInfo->output_file, decInfo->output_fname);
    strcat(decInfo->output_file, decInfo->extn_secret_file);

    decInfo->fptr_output = fopen(decInfo->output_file, "wb");

    if (decInfo->fptr_output == NULL)
    {
        printf("Output file open failed\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }


    decode_secret_file_size(decInfo);
    decode_secret_file_data(decInfo);

    printf("Decoding successful\n");
    printf("Output file: %s\n", decInfo->output_fname);

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    return e_success;
}