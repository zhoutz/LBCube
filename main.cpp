#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdio>
#include <initializer_list>
#include <iostream>
#include <vector>

struct Cube {
  using data_t = signed char;

  int nx, ny, nz;
  std::vector<data_t> data;

  Cube(int nx, int ny, int nz, data_t init)
      : nx(nx), ny(ny), nz(nz), data(nx * ny * nz, init) {}

  Cube(int nx, int ny, int nz, std::vector<data_t> v)
      : nx(nx), ny(ny), nz(nz), data(v) {
    data.resize(this->size());
  }

  int size() const { return nx * ny * nz; }

  data_t& operator()(int x, int y, int z) {
    return data[(x * ny + y) * nz + z];
  }

  data_t operator()(int x, int y, int z) const {
    return data[(x * ny + y) * nz + z];
  }

  bool operator==(const Cube& other) const {
    return nx == other.nx && ny == other.ny && nz == other.nz &&
           data == other.data;
  }

  bool operator<(const Cube& other) const {
    if (nx != other.nx) return nx < other.nx;
    if (ny != other.ny) return ny < other.ny;
    if (nz != other.nz) return nz < other.nz;
    return data < other.data;
  }

  template <typename F>
  void prt(F fp) const {
    for (int z = nz - 1; z >= 0; --z) {
      for (int i = 0; i < nx + 1; ++i) putchar(' ');
      for (int i = 0; i < ny + 2; ++i) putchar('-');
      puts("");

      for (int x = 0; x < nx; ++x) {
        for (int i = 0; i < nx - x; ++i) putchar(' ');
        putchar('/');
        for (int y = 0; y < ny; ++y) {
          auto c = (*this)(x, y, z);
          putchar(fp(c));
        }
        puts("/");
      }

      for (int i = 0; i < ny + 2; ++i) putchar('-');
      puts("");
    }
  }

  auto rotate_along_x() const {
    Cube b = *this;
    std::swap(b.ny, b.nz);
    for (int x = 0; x < nx; ++x) {
      for (int y = 0; y < ny; ++y) {
        for (int z = 0; z < nz; ++z) {
          b(x, nz - 1 - z, y) = (*this)(x, y, z);
        }
      }
    }
    return b;
  }

  auto rotate_along_y() const {
    Cube b = *this;
    std::swap(b.nx, b.nz);
    for (int x = 0; x < nx; ++x) {
      for (int y = 0; y < ny; ++y) {
        for (int z = 0; z < nz; ++z) {
          b(z, y, nx - 1 - x) = (*this)(x, y, z);
        }
      }
    }
    return b;
  }

  auto rotate_along_z() const {
    Cube b = *this;
    std::swap(b.nx, b.ny);
    for (int x = 0; x < nx; ++x) {
      for (int y = 0; y < ny; ++y) {
        for (int z = 0; z < nz; ++z) {
          b(ny - 1 - y, x, z) = (*this)(x, y, z);
        }
      }
    }
    return b;
  }

  auto generate_all_directions() const {
    std::vector<Cube> v;
    v.reserve(24);

    v.push_back(*this);
    for (int i = 0; i < 3; ++i) v.push_back(v.back().rotate_along_x());

    v.push_back(this->rotate_along_y());
    for (int i = 0; i < 3; ++i) v.push_back(v.back().rotate_along_x());

    v.push_back(this->rotate_along_y().rotate_along_y());
    for (int i = 0; i < 3; ++i) v.push_back(v.back().rotate_along_x());

    v.push_back(this->rotate_along_y().rotate_along_y().rotate_along_y());
    for (int i = 0; i < 3; ++i) v.push_back(v.back().rotate_along_x());

    v.push_back(this->rotate_along_z());
    for (int i = 0; i < 3; ++i) v.push_back(v.back().rotate_along_x());

    v.push_back(this->rotate_along_z().rotate_along_z().rotate_along_z());
    for (int i = 0; i < 3; ++i) v.push_back(v.back().rotate_along_x());

    // printf("%d\n", (int)v.size());
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
    // printf("%d\n", (int)v.size());

    return v;
  }
};

// 0 for empty, 1 for filled
struct Block : Cube {
  char name;

  Block(int nx, int ny, int nz, std::vector<data_t> v, char name = (char)0)
      : Cube(nx, ny, nz, v), name(name) {}
};

// -1 for empty
struct CubeMap : Cube {
  CubeMap(int nx, int ny, int nz) : Cube(nx, ny, nz, -1) {}

  bool judge_conflict(int px, int py, int pz, const Cube& b) {
    for (int x = 0; x < b.nx; ++x) {
      for (int y = 0; y < b.ny; ++y) {
        for (int z = 0; z < b.nz; ++z) {
          if (b(x, y, z) != 0 && (*this)(px + x, py + y, pz + z) != -1) {
            return true;
          }
        }
      }
    }
    return false;
  }

  void insert_block(int px, int py, int pz, const Cube& b, int bid) {
    // printf("insert_block %d %d %d %c\n", px, py, pz, b.name);
    // b.prt();

    for (int x = 0; x < b.nx; ++x) {
      for (int y = 0; y < b.ny; ++y) {
        for (int z = 0; z < b.nz; ++z) {
          if (b(x, y, z)) {
            (*this)(px + x, py + y, pz + z) = bid;
          }
        }
      }
    }
  }

  void remove_block(int block_id) {
    // printf("remove block %d\n", block_id);
    for (int i = 0; i < (int)data.size(); ++i) {
      if (data[i] == block_id) data[i] = -1;
    }
  }

  bool try_drop(int px, int py, const Cube& b, int bid) {
    for (int pz = 0; pz <= nz - b.nz; ++pz) {
      if (!judge_conflict(px, py, pz, b)) {
        insert_block(px, py, pz, b, bid);
        return true;
      }
    }
    return false;
  }

  void print(std::vector<Block> const& blocks) const {
    auto fp = [blocks](data_t c) {
      if (c == -1) return ' ';
      return blocks[c].name;
    };
    Cube::prt(fp);
  }
};

struct Solver {
  CubeMap cm;
  std::vector<Block> const& blocks;
  std::vector<CubeMap> solutions;

  Solver(int cm_nx, int cm_ny, int cm_nz, std::vector<Block> const& blocks)
      : cm(cm_nx, cm_ny, cm_nz), blocks(blocks), solutions() {}

  void find_solution() {
    std::vector<bool> used(blocks.size(), false);
    std::vector<std::vector<Cube>> blocks_all_dir;
    blocks_all_dir.reserve(blocks.size());
    for (auto& b : blocks) {
      blocks_all_dir.push_back(b.generate_all_directions());
    }

    // puts("blocks_all_dir sizes: ");
    // for (auto& v : blocks_all_dir) {
    //   printf("%d ", (int)v.size());
    // }
    // puts("");

    dfs(used, blocks_all_dir);
  }

  void dfs(std::vector<bool>& used,
           std::vector<std::vector<Cube>> const& blocks_all_dir) {
    if (std::count(used.cbegin(), used.cend(), true) == (int)blocks.size()) {
      // static int sol_cnt = 0;
      // printf("%d ", ++sol_cnt);
      // puts("Solution found!");
      // cm.print(blocks);

      solutions.emplace_back(cm);

      return;
    }

    for (int i = 0; i < (int)blocks_all_dir.size(); ++i) {
      if (used[i]) continue;
      auto& dirs = blocks_all_dir[i];
      for (int j = 0; j < (int)dirs.size(); ++j) {
        for (int px = 0; px <= cm.nx - dirs[j].nx; ++px) {
          for (int py = 0; py <= cm.ny - dirs[j].ny; ++py) {
            if (cm.try_drop(px, py, dirs[j], i)) {
              used[i] = true;
              dfs(used, blocks_all_dir);
              used[i] = false;
              cm.remove_block(i);
            }
          }
        }
      }
    }
  }

  void print_solution() {
    if (solutions.empty()) {
      puts("No solution found!");
      return;
    }
    if (solutions.size() == 1) {
      puts("Only one solution found!");
      auto const& sol = solutions[0];
      sol.print(blocks);
      return;
    }
    auto beg = solutions.begin();
    auto end = solutions.end();
    do {
      auto dirs = beg->generate_all_directions();
      end = std::partition(beg + 1, end, [dirs](CubeMap _cm) {
        return std::find(dirs.cbegin(), dirs.cend(), _cm) == dirs.cend();
      });
      ++beg;
    } while (end - beg > 1);
    solutions.erase(end, solutions.end());
    puts("Multiple solutions found!");
    for (auto& sol : solutions) {
      sol.print(blocks);
    }
  }
};

int main() {
  Block b1(2, 2, 2, {1, 1, 1, 1, 1, 1, 1, 0}, 'a');
  Block b2(1, 1, 1, {1}, 'b');

  std::vector<Block> blocks({b1, b2});

  Solver s(2, 2, 2, blocks);

  s.find_solution();
  s.print_solution();

  return 0;
}
