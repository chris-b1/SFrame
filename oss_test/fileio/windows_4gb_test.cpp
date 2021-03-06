#include <iostream>
#include <logger/assertions.hpp>
#include <fileio/general_fstream.hpp>
using namespace graphlab;

int main() {
  size_t length = 5 * 1024LL * 1024LL * 1024LL;
  char* data = new char[length];

  {
    graphlab::general_ofstream f("large_temp");
    // write some stuff at the 2GB and 4GB boundary so we can check reads
    for (size_t i = 0; i < 256; ++i) {
      data[i + 2LL * 1024 * 1024 * 1024] = 1;
      data[i + 4LL * 1024 * 1024 * 1024] = 2;
    }
    size_t written = f->write(data, length);
    std::cout << written << " ," << f->good() << std::endl;
    f->close();
  }

  {
    graphlab::general_ifstream f("large_temp");
    // try *really really big reads*
    // clear data
    memset(data, 0, length);
    f.read(data, length);
    ASSERT_EQ(f.gcount(), length);
    for (size_t i = 0; i < 256; ++i) {
      ASSERT_EQ(data[i + 2LL * 1024 * 1024 * 1024], 1);
      ASSERT_EQ(data[i + 4LL * 1024 * 1024 * 1024], 2);
    }
    
    // test seeks past 4GB boundary
    char c[256];
    f.seekg(2LL*1024*1024*1024, std::ios_base::beg);
    f.read(c, 256);
    ASSERT_EQ(f.gcount(), 256);
    for (size_t i = 0 ;i < 256; ++i) {
      ASSERT_EQ(c[i], 1);
    }

    f.seekg(4LL*1024*1024*1024, std::ios_base::beg);
    f.read(c, 256);
    ASSERT_EQ(f.gcount(), 256);
    for (size_t i = 0 ;i < 256; ++i) {
      ASSERT_EQ(c[i], 2);
    }

    // test read at the end
    f.seekg(4LL*1024*1024*1024 + 1024, std::ios_base::beg);
    f.read(data, 1024LL*1024*1024);
    ASSERT_EQ(f.gcount(), 1024LL*1024*1024 - 1024);
    ASSERT_TRUE(f.eof());
  }
}
