#include <stdio.h>
#include<stdlib.h>
#include<string.h>
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
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
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
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    
    printf("INFO: Opening required files\n");
    

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

    printf("INFO: Checking for %s size\n",encInfo->secret_fname);
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

    printf("INFO: Done\n");

    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    
}



Status do_encoding(EncodeInfo *encInfo)
{
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

    if(check_capacity(encInfo)==e_failure)
    {   
        printf("INFO: %s cannot handle the %s\n",encInfo->src_image_fname,encInfo->secret_fname);
        exit(0);
    }
    printf("INFO: Done, Found OK\n");

    // encode starts
    // copy header to stego
    printf("INFO: Copying imager header\n");
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)!=e_success)
    {
        printf("INFO: error copying image header\n");
        exit(0);

    }


    //encode magic string
    encode_magic_string(MAGIC_STRING,encInfo);




}