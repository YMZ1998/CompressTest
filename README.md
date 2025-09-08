# CompressTest

This project compares **zlib** and **zstd** compression.

## Libraries
- [zlib](https://zlib.net/) — widely used data compression library (DEFLATE).
- [zstd](https://facebook.github.io/zstd/) — fast lossless compression algorithm by Facebook.

## Build
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
