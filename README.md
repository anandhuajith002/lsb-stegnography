# LSB Image Steganography Tool

## Overview
A command-line tool for hiding secret messages within BMP images using Least Significant Bit (LSB) steganography technique.

## Features
- Embed secret text files into BMP images
- Extract hidden messages from steganographic images
- Supports magic string detection
- Validates image capacity before embedding
- Simple command-line interface

## Usage

### Encoding
```bash
./lsb_steg -e <source.bmp> <secret.txt> [output.bmp]
```

### Decoding
```bash
./lsb_steg -d <steged_image.bmp> [output.txt]
```

## Requirements
- C compiler (GCC recommended)
- BMP image files
- Text files to hide

## Build Instructions
```bash
gcc -o lsb_steg test_encode.c encode.c decode.c -I.
```

## How It Works
- Modifies least significant bits of image pixels
- Embeds file extension, size, and data
- Uses a magic string for verification
- Preserves image visual integrity

## Technical Details
### Encoding Process
1. Check image capacity
2. Copy BMP header
3. Embed magic string
4. Encode file extension
5. Encode file size
6. Encode file data

### Decoding Process
1. Open steganographic image
2. Verify magic string
3. Extract file extension
4. Extract file size
5. Extract file data

## Security Note
Not cryptographically secure. Use for educational purposes only.

## Example
```bash
# Encode a secret message
./lsb_steg -e image.bmp secret.txt steged_image.bmp

# Decode the hidden message
./lsb_steg -d steged_image.bmp decoded_secret.txt
```

## License
Open-source project

## Contributions
Contributions are welcome! Please submit pull requests or open issues.
