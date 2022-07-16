#include <cassert>
#include <cstdio>
#include <initializer_list>
#include <iostream>
#include <vector>

struct Block {
  int nx, ny, nz;
  std::vector<bool> data;

  Block(int nx, int ny, int nz, std::initializer_list<bool> l)
      : nx(nx), ny(ny), nz(nz), data(l) {
    assert(l.size() == this->size());
  }

  bool operator()(int x, int y, int z) const {
    return data[x + nx * (y + ny * z)];
  }

  int size() const { return nx * ny * nz; }

  bool operator==(const Block& other) const {
    return nx == other.nx && ny == other.ny && nz == other.nz &&
           data == other.data;
  }

  void prt() const {
    for (int z = nz - 1; z >= 0; --z) {
      for (int i = 0; i < nx + 1; ++i) putchar(' ');
      for (int i = 0; i < ny + 2; ++i) putchar('-');
      puts("");

      for (int x = 0; x < nx; ++x) {
        for (int i = 0; i < nx - x; ++i) putchar(' ');
        putchar('/');
        for (int y = 0; y < ny; ++y) {
          printf((*this)(x, y, z) ? "*" : " ");
        }
        puts("/");
      }

      for (int i = 0; i < ny + 2; ++i) putchar('-');
      puts("");
    }
  }
};

int main() {
  Block b1(2, 2, 2, {1, 1, 1, 1, 1, 1, 1, 1});
  Block b2(2, 2, 2, {1, 1, 1, 0, 0, 1, 0, 0});
  Block b3(2, 2, 2, {1, 1, 1, 0, 0, 1, 0, 0});
  Block b4(3, 4, 2,
           {
               1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1,
               1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1,
           });

  // assert(b1 == b2);
  b4.prt();

  printf("%d\n", b1 == b2);
  printf("%d\n", b1 == b3);
}
