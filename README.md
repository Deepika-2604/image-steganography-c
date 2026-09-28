# Image Steganography in C

A C-based Image Steganography project that hides secret data inside a BMP image using the Least Significant Bit (LSB) technique.

## 📌 Project Overview

This project implements image steganography using C programming.

The project allows the user to:
- Encode secret data into a BMP image
- Decode hidden data from the stego image
- Preserve the visual appearance of the original image
- Perform encoding and decoding through a command-line interface

## 🔐 Steganography Technique

The project uses the **Least Significant Bit (LSB)** technique.

The least significant bits of the image pixel data are modified to store the secret information.

Since only the least significant bits are changed, the visual difference between the original image and the stego image is minimal.

## 🛠️ Technologies Used

- C Programming
- File Handling
- Bitwise Operations
- Structures
- Pointers
- BMP Image Processing
- LSB Steganography
- VS Code

## 📂 Project Files

| File | Description |
|------|-------------|
| `encode.c` | Encoding secret data into the image |
| `decode.c` | Extracting hidden data from the image |
| `encode.h` | Encoding declarations |
| `decode.h` | Decoding declarations |
| `common.h` | Common definitions |
| `types.h` | Type definitions |
| `test_encode.c` | Testing encoding functionality |
| `beautiful.bmp` | Original BMP image |
| `stego.bmp` | Image containing hidden data |

## ⚙️ How It Works

### Encoding

Original Image + Secret Data → LSB Encoding → Stego Image

### Decoding

Stego Image → LSB Extraction → Secret Data

## 🚀 Compilation

Compile the project using GCC:

```bash
gcc *.c

## 🎯 Key Concepts Learned

- File handling in C
- Bitwise operations
- Binary data processing
- Command-line arguments
- Structures and pointers
- BMP image processing
- LSB-based steganography

## 👩‍💻 Author

Deepika R
