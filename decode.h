#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include "types.h"

#define MAX_FILE_SUFFIX 5

typedef struct _DecodeInfo
{
    /*stego image info*/
    char *stego_image_fname;
    FILE *fptr_stego_image;
    uint image_capacity;



    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];





}DecodeInfo;

Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *dncInfo);

Status_d do_decoding(DecodeInfo *dncInfo);

Status_d open_steg(DecodeInfo *dncInfo);

Status_d open_secret(DecodeInfo *dncInfo);

Status_d decode_file_extn(DecodeInfo *dncInfo);

Status_d decode_file_size(DecodeInfo *dncInfo);

char decode_8(DecodeInfo *dncInfo);

char byte8_decode_to_char(char buff[]);

#endif