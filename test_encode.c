#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char *argv[])
{
    EncodeInfo enc_info;
    DecodeInfo dnc_info;

    if(check_operation_type(argv)==e_encode)
    {
        if(argc>=4 && read_and_validate_encode_args(argv,&enc_info)==e_success)
        {
            do_encoding(&enc_info);
        }
        else
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]");
            return 1;
        }
    }

    if(check_operation_type(argv)==e_decode)
    {
        if(argc>=3 && read_and_validate_decode_args(argv,&dnc_info)==d_success)
        {
            do_decoding(&dnc_info);

        }
        else
        {
            printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp file> [output file]");
            return 1;
        }
    }
    if(check_operation_type(argv)==e_unsupported)
    {
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]");
        printf("\n./lsb_steg: Decoding: ./lsb_steg -d <.bmp file> [output file]");
        return 1;
    }
    



}
