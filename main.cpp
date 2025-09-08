#include <snappy.h>
#include <windows.h>
#include <zlib.h>
#include <zstd.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

std::string getExeDir() {
  char path[MAX_PATH];
  GetModuleFileNameA(NULL, path, MAX_PATH);
  std::string full(path);
  auto pos = full.find_last_of("\\/");
  return full.substr(0, pos);
}

std::ofstream log_file(getExeDir() + "/result.txt");  // 全局日志文件

// 同时输出到控制台和文件
void log(const std::string& msg) {
  std::cout << msg << std::endl;
  log_file << msg << std::endl;
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

  auto start = Clock::now();
  int ret = compress2((Bytef*)zip_buf.data(), &zip_len,
                      (const Bytef*)data.data(), data.size(), level);
  auto end = Clock::now();

  if (ret != Z_OK) {
    log("[zlib] compress fail at level " + std::to_string(level));
    return;
  }

  double time_s = std::chrono::duration<double>(end - start).count();
  log("[zlib] level=" + std::to_string(level) +
      " time=" + std::to_string(time_s) + "s" +
      " compressed_size=" + std::to_string(zip_len / 1024.0 / 1024.0) + " MB" +
      " ratio=" + std::to_string(double(zip_len) / data.size()));

  // 解压缩测试
  std::vector<char> unzip_buf(data.size());
  uLongf unzip_len = data.size();

  start = Clock::now();
  ret = uncompress((Bytef*)unzip_buf.data(), &unzip_len,
                   (const Bytef*)zip_buf.data(), zip_len);
  end = Clock::now();

  if (ret != Z_OK || unzip_len != data.size() ||
      memcmp(unzip_buf.data(), data.data(), data.size()) != 0) {
    log("[zlib] decompress check FAIL at level " + std::to_string(level));
    return;
  }

  time_s = std::chrono::duration<double>(end - start).count();
  log("[zlib] decompress time=" + std::to_string(time_s) + "s");
}

void test_zstd(const std::vector<char>& data, int level) {
  size_t zip_len = ZSTD_compressBound(data.size());
  std::vector<char> zip_buf(zip_len);

  auto start = Clock::now();
  size_t ret_len =
      ZSTD_compress(zip_buf.data(), zip_len, data.data(), data.size(), level);
  auto end = Clock::now();

  if (ZSTD_isError(ret_len)) {
    log("[zstd] compress fail at level " + std::to_string(level) +
        " err=" + ZSTD_getErrorName(ret_len));
    return;
  }

  double time_s = std::chrono::duration<double>(end - start).count();
  log("[zstd] level=" + std::to_string(level) +
      " time=" + std::to_string(time_s) + "s" +
      " compressed_size=" + std::to_string(ret_len / 1024.0 / 1024.0) + " MB" +
      " ratio=" + std::to_string(double(ret_len) / data.size()));

  // 解压缩测试
  std::vector<char> unzip_buf(data.size());
  start = Clock::now();
  size_t dec_len =
      ZSTD_decompress(unzip_buf.data(), data.size(), zip_buf.data(), ret_len);
  end = Clock::now();

  if (ZSTD_isError(dec_len) || dec_len != data.size() ||
      memcmp(unzip_buf.data(), data.data(), data.size()) != 0) {
    log("[zstd] decompress check FAIL at level " + std::to_string(level));
    return;
  }

  time_s = std::chrono::duration<double>(end - start).count();
  log("[zstd] decompress time=" + std::to_string(time_s) + "s");
}

void test_snappy(const std::vector<char>& data) {
  std::string compressed;
  std::string uncompressed;

  auto start = Clock::now();
  snappy::Compress(data.data(), data.size(), &compressed);
  auto end = Clock::now();

  double time_s = std::chrono::duration<double>(end - start).count();
  log("[snappy] compress time=" + std::to_string(time_s) + "s" +
      " compressed_size=" +
      std::to_string(compressed.size() / 1024.0 / 1024.0) + " MB" +
      " ratio=" + std::to_string(double(compressed.size()) / data.size()));

  // 解压缩测试
  start = Clock::now();
  snappy::Uncompress(compressed.data(), compressed.size(), &uncompressed);
  end = Clock::now();

  if (uncompressed.size() != data.size() ||
      memcmp(uncompressed.data(), data.data(), data.size()) != 0) {
    log("[snappy] decompress check FAIL");
    return;
  }

  time_s = std::chrono::duration<double>(end - start).count();
  log("[snappy] decompress time=" + std::to_string(time_s) + "s");
}

int main() {
  auto data = read_file(getExeDir() + "/data/voxel.raw");

  //data.resize(data.size() / 4);

  log("original size = " + std::to_string(data.size() / 1024.0 / 1024.0) +
      " MB");

  log("\n=== zlib test ===");
  for (int level = 1; level <= 9; ++level) {
    test_zlib(data, level);
  }

  log("\n=== zstd test ===");
  for (int level = 1; level <= 9; ++level) {
    test_zstd(data, level);
  }

  log("\n=== snappy test ===");
  test_snappy(data);

  log_file.close();  // 关闭文件
  return 0;
}
