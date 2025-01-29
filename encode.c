#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
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
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    
    

    // No failure return e_success
    return e_success;
}

// check for encoding or decoding
OperationType check_operation_type(char *argv[])
{
    //check for command line argument
    if(argv[1]==NULL)
        return e_unsupported;

    if(strcmp(argv[1],"-e")==0)
        return e_encode;

    else if(strcmp(argv[1],"-d")==0)
        return e_decode;

    else    
        return e_unsupported;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //argv -e .bmp .txt [.bmp]

    //check for .bmp extension
    if(strstr(argv[2],".bmp")==NULL) // not a bmp file
    {
        return e_failure;
    }
    encInfo->src_image_fname=argv[2];

    //check for .txt extension
    if(strstr(argv[3],".txt")==NULL) // not a txt file
    {
        return e_failure;
    }
    encInfo->secret_fname=argv[3];
    strcpy(encInfo->extn_secret_file,".txt");


    //checking whether 4 argument exists
    if(argv[4]==NULL)
    {
        printf("INFO: Output File not mentioned. Creating steged_img.bmp as default\n");
        encInfo->stego_image_fname="steged_img.bmp";
        return e_success;
    }
    //if exists validate argument
    else
    {
        if(strstr(argv[4],".bmp")==NULL) // not a bmp file
        {
            return e_failure;
        }
        encInfo->stego_image_fname=argv[4];

        return e_success;   
    }
    return e_success;   

}
Status check_capacity(EncodeInfo *encInfo)
{
    //stroring img capacity to structure
    uint img_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->image_capacity = img_size;

    uint file_size = get_file_size(encInfo->fptr_secret);//get secret string size

    if(file_size==0) // exit if secret string is empty
    {
        printf("INFO: Empty secret string\n");
        exit(0);
    }
    encInfo->size_secret_file=file_size;
    printf("INFO: Done, Not empty\n");

    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    
    if((14+file_size)*8 > img_size)
        return e_failure;

    return e_success;
    
}
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    uint size = ftell(fptr);
    fseek(fptr,0,SEEK_SET);
    return size;
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //resetting fptrs
    fseek(fptr_src_image,0,SEEK_SET); 
    fseek(fptr_dest_image,0,SEEK_SET);

    char buffer[100];
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);


    return e_success;
}
Status char_encode_to_8byte(char ch, char buff[]) 
{
    for (int i = 7; i >= 0; i--) {

        if (ch & (1 << i)) 
        {
            buff[7 - i] |= 1;  // Set LSB to 1
        } 
        else 
        {
            buff[7 - i] &= ~1; // Clear LSB to 0
        }
    }
    return e_success;
}
Status encode_8(EncodeInfo *encInfo,char ch)
{
    char byte_buff[8]={};
    fread(byte_buff,8,1,encInfo->fptr_src_image); //read  8 bytes
    char_encode_to_8byte(ch,byte_buff); // steg 1 byte to 8 byte
    fwrite(byte_buff,8,1,encInfo->fptr_stego_image);// write 8 bytes
        
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int len= strlen(MAGIC_STRING);
    for(int i=0;i<len;i++)
    {
        encode_8(encInfo,MAGIC_STRING[i]);

        // for (int i = 0; i < 8; i++) {
        //     printf("%02X ", byte_buff[i]);  
        // }
        // printf("\n");
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    
    int extn_size=strlen(encInfo->extn_secret_file);
    unsigned char *ch = (unsigned char *)&extn_size;

    //gettting in big-endian format
    for (int i = 3; i >= 0; i--) 
    {    
        encode_8(encInfo,ch[i]);   
    }

    int i=0;
    while(file_extn[i]!='\0')
    {
        encode_8(encInfo,file_extn[i]);
        i++;
    }

    return e_success;

}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{

    unsigned char *ch = (unsigned char *)&file_size;

    //gettting in big-endian format
    for (int i = 3; i >= 0; i--) 
    {    
        encode_8(encInfo,ch[i]);  
    }
    return e_success;

}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char ch=getc(encInfo->fptr_secret);
    while(ch!=EOF)
    {
        encode_8(encInfo,ch);
        // printf("%c",ch);
        ch=getc(encInfo->fptr_secret);
    }
    return e_success;
    
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        fwrite(buffer, 1, bytes, fptr_dest);
    }

    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    if(open_files(encInfo)==e_success)
    {
        //opening required file in open_files function
        printf("INFO: Opened %s\n",encInfo->src_image_fname);
        printf("INFO: Opened %s\n",encInfo->secret_fname);
        printf("INFO: Opened %s\n",encInfo->stego_image_fname);
    }
    else
    {
        exit(0);
    }

    printf("INFO: Done\n");


    printf("INFO: ## Encoding Procedure Started ##\n");

    printf("INFO: Checking for %s size\n",encInfo->secret_fname);
    if(check_capacity(encInfo)==e_success)
    {   
        printf("INFO: Done, Found OK\n");
    }
    else{
        printf("INFO: %s cannot handle the %s\n",encInfo->src_image_fname,encInfo->secret_fname);
        exit(0);

    }

    // encode starts
    // copy header to stego
    printf("INFO: Copying imager header\n");
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: error copying image header\n");
        exit(0);

    }


    //encode magic string
    printf("INFO: Encoding Magic String signature\n");
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Done\n");
        printf("INFO: error copying magic string\n");
        exit(0);
    }

    
    //encode secret file extension
    printf("INFO: Encoding %s file extension \n",encInfo->secret_fname);
    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
    {
        printf("INFO: Done\n");  
    }
    else
    {
        printf("INFO: error copying extension \n");
        exit(0);
    }
    

    //encode secret file size
    printf("INFO: Encoding %s file size \n",encInfo->secret_fname);
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
    {
        printf("INFO: Done\n");  
    }
    else
    {
        printf("INFO: error copying file size \n");
        exit(0);
    }
    
    //encode secret file data
    printf("INFO: Encoding %s file data \n",encInfo->secret_fname);
    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("INFO: Done\n");  
    }
    else
    {
        printf("INFO: error copying secret file data \n");
        exit(0);
    }
    
    //copy remaining secret file data
    printf("INFO: Copying left over data \n");
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("INFO: Done\n");  
    }
    else
    {
        printf("INFO: error copying secret file data \n");
        exit(0);
    }
    printf("INFO: ##Encoding done successfully ##\n");

}