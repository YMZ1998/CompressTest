#include <windows.h>
#include <zlib.h>
#include <zstd.h>
#include <chrono>
#include <cstring>  // for memcmp
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string getExeDir() {
  char path[MAX_PATH];
  GetModuleFileNameA(NULL, path, MAX_PATH);
  std::string full(path);
  auto pos = full.find_last_of("\\/");
  return full.substr(0, pos);
}

std::vector<char> read_file(const std::string& path) {
  std::ifstream f(path, std::ios::binary | std::ios::ate);
  size_t size = f.tellg();
  f.seekg(0);
  std::vector<char> buffer(size);
  f.read(buffer.data(), size);
  return buffer;
}

void test_zlib(const std::vector<char>& data, int level) {
  uLongf zip_len = compressBound(data.size());
  std::vector<char> zip_buf(zip_len);

  auto start = std::chrono::high_resolution_clock::now();
  int ret = compress2(reinterpret_cast<Bytef*>(zip_buf.data()), &zip_len,
                      reinterpret_cast<const Bytef*>(data.data()), data.size(),
                      level);
  auto end = std::chrono::high_resolution_clock::now();

  if (ret != Z_OK) {
    std::cout << "[zlib] compress fail at level " << level << std::endl;
    return;
  }

  double time_s = std::chrono::duration<double>(end - start).count();
  std::cout << "[zlib] level=" << level << " time=" << time_s << "s"
            << " compressed_size=" << zip_len / 1024.0 / 1024.0 << " MB"
            << " ratio=" << double(zip_len) / data.size() << std::endl;

  // ½âÑ¹Ëõ²âÊÔ
  std::vector<char> unzip_buf(data.size());
  uLongf unzip_len = data.size();

  start = std::chrono::high_resolution_clock::now();
  ret = uncompress(reinterpret_cast<Bytef*>(unzip_buf.data()), &unzip_len,
                   reinterpret_cast<const Bytef*>(zip_buf.data()), zip_len);
  end = std::chrono::high_resolution_clock::now();

  if (ret != Z_OK || unzip_len != data.size() ||
      std::memcmp(unzip_buf.data(), data.data(), data.size()) != 0) {
    std::cout << "[zlib] decompress check FAIL at level " << level << std::endl;
    return;
  }

  time_s = std::chrono::duration<double>(end - start).count();
  std::cout << "  [zlib] decompress time=" << time_s << "s" << std::endl;
}

void test_zstd(const std::vector<char>& data, int level) {
  size_t zip_len = ZSTD_compressBound(data.size());
  std::vector<char> zip_buf(zip_len);

  auto start = std::chrono::high_resolution_clock::now();
  size_t ret_len =
      ZSTD_compress(zip_buf.data(), zip_len, data.data(), data.size(), level);
  auto end = std::chrono::high_resolution_clock::now();

  if (ZSTD_isError(ret_len)) {
    std::cout << "[zstd] compress fail at level " << level
              << " err=" << ZSTD_getErrorName(ret_len) << std::endl;
    return;
  }

  double time_s = std::chrono::duration<double>(end - start).count();
  std::cout << "[zstd] level=" << level << " time=" << time_s << "s"
            << " compressed_size=" << ret_len / 1024.0 / 1024.0 << " MB"
            << " ratio=" << double(ret_len) / data.size() << std::endl;

  // ½âÑ¹Ëõ²âÊÔ
  std::vector<char> unzip_buf(data.size());
  start = std::chrono::high_resolution_clock::now();
  size_t dec_len =
      ZSTD_decompress(unzip_buf.data(), data.size(), zip_buf.data(), ret_len);
  end = std::chrono::high_resolution_clock::now();

  if (ZSTD_isError(dec_len) || dec_len != data.size() ||
      std::memcmp(unzip_buf.data(), data.data(), data.size()) != 0) {
    std::cout << "[zstd] decompress check FAIL at level " << level << std::endl;
    return;
  }

  time_s = std::chrono::duration<double>(end - start).count();
  std::cout << "[zstd] decompress time=" << time_s << "s" << std::endl;
}

int main() {
  auto data = read_file(getExeDir() + "/data/voxel.raw");

  std::cout << "original size = " << data.size() / 1024.0 / 1024.0 << " MB"
            << std::endl;

  std::cout << "\n=== zlib test ===" << std::endl;
  for (int level = 3; level <= 9; ++level) {
    test_zlib(data, level);
  }

  std::cout << "\n=== zstd test ===" << std::endl;
  for (int level = 1; level <= 9; ++level) {
    test_zstd(data, level);
  }

  return 0;
}
