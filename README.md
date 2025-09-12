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
```

```
original size = 512.000000 MB

=== zlib test ===
[zlib] level=1 time=3.380969s compressed_size=178.297985 MB ratio=0.348238
[zlib] decompress time=0.893609s
[zlib] level=2 time=3.565404s compressed_size=177.224670 MB ratio=0.346142
[zlib] decompress time=0.893550s
[zlib] level=3 time=4.811300s compressed_size=175.484807 MB ratio=0.342744
[zlib] decompress time=0.849174s
[zlib] level=4 time=5.218141s compressed_size=175.569417 MB ratio=0.342909
[zlib] decompress time=0.941077s
[zlib] level=5 time=7.383613s compressed_size=174.718477 MB ratio=0.341247
[zlib] decompress time=1.267094s
[zlib] level=6 time=14.062809s compressed_size=173.740620 MB ratio=0.339337
[zlib] decompress time=0.905032s
[zlib] level=7 time=9.720036s compressed_size=173.718716 MB ratio=0.339294
[zlib] decompress time=0.893704s
[zlib] level=8 time=10.945577s compressed_size=173.384421 MB ratio=0.338641
[zlib] decompress time=0.885885s
[zlib] level=9 time=14.328108s compressed_size=173.337916 MB ratio=0.338551
[zlib] decompress time=0.907112s

=== zstd test ===
[zstd] level=1 time=0.356324s compressed_size=177.548021 MB ratio=0.346773
[zstd] decompress time=0.168472s
[zstd] level=2 time=0.480748s compressed_size=176.258966 MB ratio=0.344256
[zstd] decompress time=0.187347s
[zstd] level=3 time=1.078193s compressed_size=163.380512 MB ratio=0.319103
[zstd] decompress time=0.269788s
[zstd] level=4 time=1.455819s compressed_size=158.863719 MB ratio=0.310281
[zstd] decompress time=0.284954s
[zstd] level=5 time=1.920928s compressed_size=156.628962 MB ratio=0.305916
[zstd] decompress time=0.294994s
[zstd] level=6 time=2.238227s compressed_size=156.392899 MB ratio=0.305455
[zstd] decompress time=0.281022s
[zstd] level=7 time=2.525145s compressed_size=155.127247 MB ratio=0.302983
[zstd] decompress time=0.291397s
[zstd] level=8 time=3.243779s compressed_size=155.026007 MB ratio=0.302785
[zstd] decompress time=0.290527s
[zstd] level=9 time=3.689270s compressed_size=154.946235 MB ratio=0.302629
[zstd] decompress time=0.315582s

=== snappy test ===
[snappy] compress time=0.338882s compressed_size=256.582170 MB ratio=0.501137
[snappy] decompress time=0.164270s
```
