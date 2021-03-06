# Hideit

A steganography utility

Currently supports hiding any sort of data in an image

Supported image formats: PNG, BMP, TGA

## Compilation (Linux)

First clone this repository and update the submodules:
```bash
git clone --recurse-submodules git@github.com:Will-Banksy/Hideit.git
```

Then go into the project directory and do:
```bash
mkdir build && cd build
cmake -DCLI_PARSER_STATIC=ON -DCOMPLIB_STATIC=ON -DILIB_STATIC=ON -DENCLIB_STATIC=ON -DCMAKE_BUILD_TYPE=Release ..
make
```

(If you don't want to compile the libraries as static, then don't specify those cmake options, and you'll just have `cmake -DCMAKE_BUILD_TYPE=Release ..`)

The executable is called 'hideit'