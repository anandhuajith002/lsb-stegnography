#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "types.h"
#include "decode.h"
#include "common.h"

Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *dncInfo)
{
    //argv -d .bmp [.txt]

    if(strstr(argv[2],".bmp")==NULL)
    {
        return d_failure;
    }
    dncInfo->stego_image_fname=argv[2];


    //if exists validate argument
    if(argv[3]!=NULL)
    {
        if(strstr(argv[3],".txt")==NULL) // not a bmp file
        {
            return e_failure;
        }
        dncInfo->secret_fname=argv[3];

        return d_success;   
    }
    return d_success; 
}

//opening steg imgage
Status_d open_steg(DecodeInfo *dncInfo)
{
    //stego img
    dncInfo->fptr_stego_image = fopen(dncInfo->stego_image_fname,"rb");
    //errror handling
    if (dncInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", dncInfo->stego_image_fname);
    	return d_failure;
    }
    return d_success;


}

//cheching if magic string is present
Status_d decode_magic_string(const char *magic_string, DecodeInfo *dncInfo)
{
    fseek(dncInfo->fptr_stego_image,54,SEEK_SET);
    int len= strlen(MAGIC_STRING);
    char ch;
    for(int i=0;i<len;i++)
    {
        ch=decode_8(dncInfo);
        if(ch!=MAGIC_STRING[i])
        {
            return d_failure;
        }

        // for (int i = 0; i < 8; i++) {
        //     print_sleep("%02X ", byte_buff[i]);  
        // }
        // print_sleep("\n");
    }
    return d_success;
    

}


//reding 8 byte and storing in a buffer then decoding
char decode_8(DecodeInfo *dncInfo)
{
    char byte_buff[8]={};
    fread(byte_buff,8,1,dncInfo->fptr_stego_image);
    char ch=byte8_decode_to_char(byte_buff);
    // for (int i = 0; i < 8; i++) {
    //         printf("%02X ", byte_buff[i]);  
    //     }
    return ch;
}

//decoding the 8 byte insude buff to a characher
char byte8_decode_to_char(char buff[])
{
    char ch=0;
    for(int i=7 ; i>=0 ; i--)
    {
        if(buff[7-i] & 1)
        {
            ch=ch | 1<<i;
        }
    }
    return ch;

}

//opening destination file
Status_d open_secret(DecodeInfo *dncInfo)
{
    if(dncInfo->secret_fname == NULL || dncInfo->secret_fname[0] == '\0')
    {
        dncInfo->secret_fname="decode.txt";
        print_sleep("INFO: Output file not mentioned, creating decode.txt as default\n");
    }

    dncInfo->fptr_secret = fopen(dncInfo->secret_fname,"wb");
    //errror handling
    if (dncInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", dncInfo->secret_fname);
    	return d_failure;
    }
    return d_success;

}


Status_d decode_file_extn(DecodeInfo *dncInfo)
{
    //decoding 4 byte size
    char ext_size[4];
    for(int i=0;i<4;i++)
    {
        ext_size[i]=decode_8(dncInfo);
    }

    //decoding extension
    uint size=(int)ext_size[3];
    for(int i=0;i<size;i++)
    {
        dncInfo->extn_secret_file[i]=decode_8(dncInfo);
    }
    dncInfo->extn_secret_file[size]='\0';

    return d_success;
}

Status_d do_decoding(DecodeInfo *dncInfo)
{
    print_sleep("INFO: ## Decoding Procedure Started ##\n");
    
    print_sleep("INFO: Opening required files\n");

    //opening steged image
    if(open_steg(dncInfo)==d_success)
    {
        print_sleep("INFO: Opened %s\n",dncInfo->stego_image_fname);
    }
    else
    {
        exit(0);
    }

    //check if magic string is present
    printf("INFO: Decoding Magic String signature\n");
    if(decode_magic_string(MAGIC_STRING,dncInfo)==d_success)
    {
        print_sleep("INFO: Done\n");

    }
    else
    {
        print_sleep("INFO: Magic string not present, Image is not Stegged\n");
        exit(0);
    }


    //opening if decode.txt
    if(open_secret(dncInfo)==d_success)
    {
        print_sleep("INFO: Opened %s\n",dncInfo->secret_fname);
        print_sleep("INFO: Done, Opened all required files\n");
    }
    else
    {
        exit(0);
    }

    print_sleep("INFO: Decoding file extension from %s\n",dncInfo->stego_image_fname);
    if(decode_file_extn(dncInfo)==d_success)
    {
        print_sleep("INFO: Done\n");
    }
    else
    {
        print_sleep("INFO: error decoding file extension\n");

    }
    



    


}