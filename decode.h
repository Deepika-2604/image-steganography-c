#ifndef DECODE_H
#define DECODE_H

#include "types.h"// Contains user defined types
#include <string.h>
#include<stdlib.h>
#include <stdio.h> 


#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


/* Structure to store decoding information */
typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    char *output_fname;
    char output_file[100];
    FILE *fptr_output;

    /* Decoded data */
    int magic_string_size;
    char magic_string[50];

    int extn_size;
    char extn_secret_file[10];

    int secret_file_size;

} DecodeInfo;

/* Decoding function prototypes */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status open_files_decode(DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);

/* Core decoding functions */
int decode_size_from_lsb(char *image_data);
char decode_byte_from_lsb(char *image_buffer);
Status decode_magic_string_size(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif