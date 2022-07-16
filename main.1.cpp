#include <cassert>
#include <cstdio>
#include <initializer_list>

template <int nx, int ny, int nz>
struct Block {
  static constexpr int nx_ = nx;
  static constexpr int ny_ = ny;
  static constexpr int nz_ = nz;
  static constexpr int size = nx * ny * nz;

  bool data[nx][ny][nz];

  Block(std::initializer_list<bool> l) {
    assert(l.size() == size);
    auto it = l.begin();
    for (int x = 0; x < nx; x++) {
      for (int y = 0; y < ny; y++) {
        for (int z = 0; z < nz; z++) {
          data[x][y][z] = *it++;
        }
      }
    }
  }

  bool cmp_data(const Block& other) const {
    for (int x = 0; x < nx; x++) {
      for (int y = 0; y < ny; y++) {
        for (int z = 0; z < nz; z++) {
          if (data[x][y][z] != other.data[x][y][z]) {
            return false;
          }
        }
      }
    }
    return true;
  }

  void prt() const {
    for (int i = 0; i < ny + 2; ++i) putchar('-');
    puts("");

    for (int x = nx - 1; x >= 0; --x) {
      for (int z = nz - 1; z >= 0; --z) {
        putchar('|');
        for (int y = 0; y < ny; ++y) {
          printf(data[x][y][z] ? "*" : " ");
        }
        puts("|");
      }
      for (int i = 0; i < ny + 2; ++i) putchar('-');
      puts("");
    }
  }
};

template <int i1, int i2, int i3, int i4, int i5, int i6>
bool operator==(const Block<i1, i2, i3>& lhs, const Block<i4, i5, i6>& rhs) {
  return false;
}

template <int i1, int i2, int i3>
bool operator==(const Block<i1, i2, i3>& lhs, const Block<i1, i2, i3>& rhs) {
  return lhs.cmp_data(rhs);
}

int main() {
  Block<2, 2, 2> b1({1, 1, 1, 1, 1, 1, 1, 1});
  Block<2, 2, 2> b2({1, 1, 1, 0, 0, 1, 0, 0});
  Block<2, 2, 3> b3({1, 1, 1, 0, 0, 1, 0, 0});

  // assert(b1 == b2);
  b2.prt();

  printf("%d\n", b1 == b2);
  printf("%d\n", b1 == b3);
}
