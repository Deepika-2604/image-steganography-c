#include <stdio.h>
#include <string.h> /* REQUIRED */
#include "encode.h"
#include "types.h"
#include "common.h"

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, fptr_image);
    fread(&height, sizeof(int), 1, fptr_image);

    return width * height * 3;
}

/* Open files */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
        return e_failure;

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    if (encInfo->fptr_secret == NULL)
        return e_failure;

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
        return e_failure;

    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strstr(argv[2], ".bmp"))
    {
        encInfo->src_image_fname = argv[2];

        if (strstr(argv[3], ".txt"))
        {
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file, strstr(argv[3], "."));

            encInfo->stego_image_fname = argv[4] ? argv[4] : "stego.bmp";
            return e_success;
        }
    }
    return e_failure;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    //printf("Enter the magic string : ");
    //scanf("%s", 
    
    strcpy(encInfo->magic_string,MAGIC_STRING); 

    fseek(encInfo->fptr_secret, 0, SEEK_END);
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);
    fseek(encInfo->fptr_secret, 0, SEEK_SET);

    int size_of_info =
        4 + strlen(encInfo->magic_string) +
        4 + strlen(encInfo->extn_secret_file) +
        4 + encInfo->size_secret_file;

    if (encInfo->image_capacity > size_of_info * 8)
        return e_success;

    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(arr, 54, 1, fptr_src_image);
    fwrite(arr, 54, 1, fptr_dest_image);
    return e_success;
}

Status encode_size_to_lsb(int size, char *image_data)
{
    for (int i = 0; i < 32; i++)
    {
        image_data[i] = (image_data[i] & ~1) | ((size >> i) & 1);
    }
    return e_success;
}

/* REQUIRED helper */
Status encode_byte_to_lsb(char data, char *image_data)
{
    for (int i = 0; i < 8; i++)
    {
        image_data[i] = (image_data[i] & ~1) | ((data >> i) & 1);
    }
}

Status encode_data_to_image(char *data, int size,
                            FILE *fptr_src_image,
                            FILE *fptr_stego_image)
{
    char arr[8];

    for (int i = 0; i < size; i++)
    {
        fread(arr, 8, 1, fptr_src_image);
        encode_byte_to_lsb(data[i], arr);
        fwrite(arr, 8, 1, fptr_stego_image);
    }
    return e_success;
}

Status encode_magic_string_size(EncodeInfo *encInfo)
{
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(strlen(encInfo->magic_string), arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_magic_string(EncodeInfo *encInfo)
{
    return encode_data_to_image(encInfo->magic_string,
                                strlen(encInfo->magic_string),
                                encInfo->fptr_src_image,
                                encInfo->fptr_stego_image);
}

Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(strlen(encInfo->extn_secret_file), arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    return encode_data_to_image(encInfo->extn_secret_file,
                                strlen(encInfo->extn_secret_file),
                                encInfo->fptr_src_image,
                                encInfo->fptr_stego_image);
}

Status encode_secret_file_size(EncodeInfo *encInfo)
{
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(encInfo->size_secret_file, arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    while (fread(&ch, 1, 1, encInfo->fptr_secret))
    {
        encode_data_to_image(&ch, 1,
                             encInfo->fptr_src_image,
                             encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    char ch;
    while (fread(&ch, 1, 1, encInfo->fptr_src_image))
        fwrite(&ch, 1, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("File opened successfully...\n");
    }
    else
    {
        printf("File open error !\n");
        return e_failure;
    }

    if (check_capacity(encInfo) == e_success)
    {
        printf("Capacity enough to store data\n");
    }
    else
    {
        printf("Capacity not enough to store data !\n");
        return e_failure;
    }

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("BMP header copied successfully...\n");
    }
    else
    {
        printf("Failed to copy BMP header !\n");
        return e_failure;
    }

    if (encode_magic_string_size(encInfo) == e_success)
    {
        printf("Magic string size encoded successfully...\n");
    }
    else
    {
        printf("Failed to encode magic string size !\n");
        return e_failure;
    }

    if (encode_magic_string(encInfo) == e_success)
    {
        printf("Magic string encoded successfully...\n");
    }
    else
    {
        printf("Failed to encode magic string !\n");
        return e_failure;
    }

    encode_secret_file_extn_size(encInfo);

    if (encode_secret_file_extn(encInfo) == e_success)
    {
        printf("Secret file extension encoded successfully...\n");
    }
    else
    {
        printf("Failed to encode secret file extension !\n");
        return e_failure;
    }

    encode_secret_file_size(encInfo);

    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret file data encoded successfully...\n");
    }
    else
    {
        printf("Failed to encode secret file data !\n");
        return e_failure;
    }

    if (copy_remaining_img_data(encInfo) == e_success)
    {
        printf("Remaining image data copied successfully...\n");
    }
    else
    {
        printf("Failed to copy remaining image data !\n");
        return e_failure;
    }

    printf("Stego image created successfully...\n");
    return e_success;
}
