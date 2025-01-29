# LSB Image Steganography

## Introduction
LSB (Least Significant Bit) Image Steganography is a technique for hiding secret information within digital images by modifying the least significant bits of pixel values. This project implements a command-line tool to embed and extract hidden messages in `.bmp` images.

## Features
- Accepts a `.bmp` image file as a carrier.
- Hides a message from a text file within the image.
- Checks whether the message fits in the image before embedding.
- Provides an option to embed a magic string for detection.
- Supports message extraction from steganographic images.
- Fully command-line based, with all options passed as arguments.

## Requirements
- **Programming Knowledge:**
  - File I/O operations
  - Encryption & decryption concepts
  - Pointers and structures in C (if applicable)

## Installation
Clone this repository:
```sh
git clone https://github.com/yourusername/lsb-steganography.git
cd lsb-steganography
