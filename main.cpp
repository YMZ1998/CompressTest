#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <zlib.h>
#include <zstd.h>

using namespace std;
using Clock = chrono::high_resolution_clock;

vector<char> read_file(const string& path) {
    ifstream f(path, ios::binary | ios::ate);
    size_t size = f.tellg();
    f.seekg(0);
    vector<char> buffer(size);
    f.read(buffer.data(), size);
    return buffer;
}

void test_zlib(const vector<char>& data, int level) {
    uLongf zip_len = compressBound(data.size());
    vector<char> zip_buf(zip_len);

    auto start = Clock::now();
    int ret = compress2((Bytef*)zip_buf.data(), &zip_len,
                        (const Bytef*)data.data(), data.size(), level);
    auto end = Clock::now();

    if (ret != Z_OK) {
        cout << "[zlib] compress fail at level " << level << endl;
        return;
    }

    double time_s = chrono::duration<double>(end - start).count();
    cout << "[zlib] level=" << level
         << " time=" << time_s << "s"
         << " compressed_size=" << zip_len / 1024.0 / 1024.0 << " MB"
         << " ratio=" << double(zip_len) / data.size() << endl;
}

void test_zstd(const vector<char>& data, int level) {
    size_t zip_len = ZSTD_compressBound(data.size());
    vector<char> zip_buf(zip_len);

    auto start = Clock::now();
    size_t ret_len = ZSTD_compress(zip_buf.data(), zip_len,
                                   data.data(), data.size(), level);
    auto end = Clock::now();

    if (ZSTD_isError(ret_len)) {
        cout << "[zstd] compress fail at level " << level
             << " err=" << ZSTD_getErrorName(ret_len) << endl;
        return;
    }

    double time_s = chrono::duration<double>(end - start).count();
    cout << "[zstd] level=" << level
         << " time=" << time_s << "s"
         << " compressed_size=" << ret_len / 1024.0 / 1024.0 << " MB"
         << " ratio=" << double(ret_len) / data.size() << endl;
}

int main() {
    auto data = read_file("./data/voxel.raw");
    cout << "original size = " << data.size() / 1024.0 / 1024.0 << " MB" << endl;

    cout << "\n=== zlib test ===" << endl;
    for (int level = 3; level <= 9; ++level) {
        test_zlib(data, level);
    }

    cout << "\n=== zstd test ===" << endl;
    for (int level = 1; level <= 9; ++level) {
        test_zstd(data, level);
    }

    return 0;
}
