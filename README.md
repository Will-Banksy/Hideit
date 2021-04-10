# Hideit

A steganography utility

Currently supports hiding any sort of data in an image

Supported image formats: PNG, BMP, TGA, JPG

## Compilation (Linux)

First clone this repository and update the submodules:
```bash
git clone --recurse-submodules git@github.com:Will-Banksy/Hideit.git
```

Then go into the project directory and do:
```bash
mkdir build && cd build
cmake ..
make
```

The executable is called 'hideit'