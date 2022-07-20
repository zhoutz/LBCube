#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdio>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <vector>

struct Cube {
  using elem_t = signed char;
  using data_t = std::vector<elem_t>;

  int nx, ny, nz;
  data_t data;

  Cube() { assert(false); }

  Cube(int nx, int ny, int nz, elem_t init)
      : nx(nx), ny(ny), nz(nz), data(nx * ny * nz, init) {}

  Cube(int nx, int ny, int nz, data_t v) : nx(nx), ny(ny), nz(nz), data(v) {
    data.reserve(this->size());
    data.resize(this->size());
  }

  int size() const { return nx * ny * nz; }

  elem_t& operator()(int x, int y, int z) {
    assert(x >= 0 && x < nx && y >= 0 && y < ny && z >= 0 && z < nz);
    return data[(x * ny + y) * nz + z];
  }

  elem_t operator()(int x, int y, int z) const {
    assert(x >= 0 && x < nx && y >= 0 && y < ny && z >= 0 && z < nz);
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

  void prt(std::function<char(elem_t)> fp) const {
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

  int count_if(std::function<bool(elem_t)> f) const {
    return std::count_if(data.begin(), data.end(), f);
  }

  bool connected_num_eq_one_bfs(std::function<bool(elem_t)> f) const {
    elem_t cnt = 0;
    Cube vis(nx, ny, nz, 0);
    std::queue<std::tuple<int, int, int>> q;
    for (int x = 0; x < nx; ++x) {
      for (int y = 0; y < ny; ++y) {
        for (int z = 0; z < nz; ++z) {
          if (vis(x, y, z)) continue;
          if (f((*this)(x, y, z))) {
            cnt++;
            if (cnt > 1) return false;
            vis(x, y, z) = 1;
            q.push({x, y, z});
            do {
              auto [x, y, z] = q.front();
              q.pop();
              if (x > 0 && f((*this)(x - 1, y, z)) && !vis(x - 1, y, z)) {
                vis(x - 1, y, z) = 1;
                q.push({x - 1, y, z});
              }
              if (x < nx - 1 && f((*this)(x + 1, y, z)) && !vis(x + 1, y, z)) {
                vis(x + 1, y, z) = 1;
                q.push({x + 1, y, z});
              }
              if (y > 0 && f((*this)(x, y - 1, z)) && !vis(x, y - 1, z)) {
                vis(x, y - 1, z) = 1;
                q.push({x, y - 1, z});
              }
              if (y < ny - 1 && f((*this)(x, y + 1, z)) && !vis(x, y + 1, z)) {
                vis(x, y + 1, z) = 1;
                q.push({x, y + 1, z});
              }
              if (z > 0 && f((*this)(x, y, z - 1)) && !vis(x, y, z - 1)) {
                vis(x, y, z - 1) = 1;
                q.push({x, y, z - 1});
              }
              if (z < nz - 1 && f((*this)(x, y, z + 1)) && !vis(x, y, z + 1)) {
                vis(x, y, z + 1) = 1;
                q.push({x, y, z + 1});
              }
            } while (!q.empty());
          }
        }
      }
    }
    return cnt == 1;
  }

  bool connected_num_eq_one_dfs(std::function<bool(elem_t)> f) const {
    elem_t cnt = 0;
    Cube vis(nx, ny, nz, 0);
    for (int x = 0; x < nx; ++x) {
      for (int y = 0; y < ny; ++y) {
        for (int z = 0; z < nz; ++z) {
          if (vis(x, y, z)) continue;
          if (f((*this)(x, y, z))) {
            ++cnt;
            if (cnt > 1) return false;
            vis(x, y, z) = 1;
            dfs_helper(x, y, z, vis, f);
          }
        }
      }
    }
    return cnt == 1;
  }

  void dfs_helper(int x, int y, int z, Cube& vis,
                  std::function<bool(elem_t)> f) const {
    if (x > 0 && f((*this)(x - 1, y, z)) && !vis(x - 1, y, z)) {
      vis(x - 1, y, z) = 1;
      dfs_helper(x - 1, y, z, vis, f);
    }
    if (x < nx - 1 && f((*this)(x + 1, y, z)) && !vis(x + 1, y, z)) {
      vis(x + 1, y, z) = 1;
      dfs_helper(x + 1, y, z, vis, f);
    }
    if (y > 0 && f((*this)(x, y - 1, z)) && !vis(x, y - 1, z)) {
      vis(x, y - 1, z) = 1;
      dfs_helper(x, y - 1, z, vis, f);
    }
    if (y < ny - 1 && f((*this)(x, y + 1, z)) && !vis(x, y + 1, z)) {
      vis(x, y + 1, z) = 1;
      dfs_helper(x, y + 1, z, vis, f);
    }
    if (z > 0 && f((*this)(x, y, z - 1)) && !vis(x, y, z - 1)) {
      vis(x, y, z - 1) = 1;
      dfs_helper(x, y, z - 1, vis, f);
    }
    if (z < nz - 1 && f((*this)(x, y, z + 1)) && !vis(x, y, z + 1)) {
      vis(x, y, z + 1) = 1;
      dfs_helper(x, y, z + 1, vis, f);
    }
  }
};

// 0 for empty, 1 for filled
struct Block : Cube {
  char name;

  Block(int nx, int ny, int nz, Cube::data_t v, char name = (char)0)
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
          if (b(x, y, z) != 0) {
            (*this)(px + x, py + y, pz + z) = bid;
          }
        }
      }
    }
  }

  void remove_block(int px, int py, int pz, const Cube& b, int /* bid */) {
    // printf("remove block %d\n", block_id);

    for (int x = 0; x < b.nx; ++x) {
      for (int y = 0; y < b.ny; ++y) {
        for (int z = 0; z < b.nz; ++z) {
          if (b(x, y, z) != 0) {
            (*this)(px + x, py + y, pz + z) = -1;
          }
        }
      }
    }
  }

  int try_drop(int px, int py, const Cube& b, int bid) {
    for (int pz = 0; pz <= nz - b.nz; ++pz) {
      if (!judge_conflict(px, py, pz, b)) {
        insert_block(px, py, pz, b, bid);
        return pz;
      }
    }
    return -1;
  }

  void print(std::vector<Block> const& blocks) const {
    auto fp = [blocks](elem_t c) {
      if (c == -1) return ' ';
      return blocks[c].name;
    };
    Cube::prt(fp);
  }
};

struct Solver {
  CubeMap cm;
  std::vector<Block> const& blocks;
  std::vector<std::vector<Cube>> blocks_all_dir;
  std::vector<CubeMap> solutions;
  std::set<Cube::data_t> solutions_all_dir;
  std::set<Cube::data_t> vis;

#define RECORD_HISTORY 1

#if RECORD_HISTORY
  struct Step {
    int px, py, pz;
    Cube const& c;

    int x_min() const { return px; }
    int x_max() const { return px + c.nx - 1; }
    int y_min() const { return py; }
    int y_max() const { return py + c.ny - 1; }

    bool is_overlap(const Step& s) const {
      return s.x_min() <= x_max() + 1 && s.x_max() + 1 >= x_min() &&
             s.y_min() <= y_max() + 1 && s.y_max() + 1 >= y_min();
    }
  };
  std::vector<Step> steps;
#endif

  Solver(int cm_nx, int cm_ny, int cm_nz, std::vector<Block> const& blocks)
      : cm(cm_nx, cm_ny, cm_nz), blocks(blocks) {
    int sum = std::accumulate(
        blocks.begin(), blocks.end(), 0, [](int a, Block const& b) {
          return a + b.count_if([](Cube::elem_t c) { return c != 0; });
        });
    if (sum > cm_nx * cm_ny * cm_nz) {
      printf("too many blocks\n");
    } else if (sum < cm_nx * cm_ny * cm_nz) {
      printf("too few blocks\n");
    } else {
      printf("appropriate blocks\n");
    }

    // generate all possible blocks
    blocks_all_dir.reserve(blocks.size());
    for (auto& b : blocks) {
      blocks_all_dir.push_back(b.generate_all_directions());
    }

    blocks_all_dir[0].resize(1);

    puts("blocks_all_dir sizes: ");
    for (auto& v : blocks_all_dir) {
      printf("%d ", (int)v.size());
    }
    putchar('\n');
  }

  void find_solution() {
    std::vector<bool> used(blocks.size(), false);
    dfs(used, 0);
  }

  void dfs(std::vector<bool>& used, int used_count) {
    // find a solution
    if (used_count == (int)blocks.size()) {
      this->save_solution(cm);
      return;
    }

    // if empty cell is not connected, pruning
    if (!cm.connected_num_eq_one_dfs([](Cube::elem_t c) { return c == -1; })) {
      return;
    }

    if (vis.count(cm.data) == 1) {
      return;
    } else if (used_count <= 5) {
      vis.insert(cm.data);
    }

    for (int i = 0; i < (int)blocks_all_dir.size(); ++i) {
      if (used[i]) continue;
      auto& dirs = blocks_all_dir[i];
      for (auto const& dir : dirs) {
        for (int px = 0; px <= cm.nx - dir.nx; ++px) {
          for (int py = 0; py <= cm.ny - dir.ny; ++py) {
#if RECORD_HISTORY
            if (steps.size() >= 1 &&
                !steps.back().is_overlap({px, py, 0, dir})) {
              continue;
            }
#endif
            int pz = cm.try_drop(px, py, dir, i);
            if (pz != -1) {
#if RECORD_HISTORY
              steps.push_back({px, py, pz, dir});
#endif
              used[i] = true;
              dfs(used, used_count + 1);
              if (solutions.size() == 480) return;
              used[i] = false;
              cm.remove_block(px, py, pz, dir, i);
#if RECORD_HISTORY
              steps.pop_back();
#endif
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
    for (int i = 0; i < (int)solutions.size(); ++i) {
      printf("Solution %d:\n", i + 1);
      solutions[i].print(blocks);
    }
  }

  void save_solution(CubeMap const& _cm) {
    if (solutions_all_dir.count(_cm.data) == 0) {
      static int sol_cnt = 0;
      printf("\r%d Solution found!", ++sol_cnt);
      fflush(stdout);
      // putchar('\n');

      solutions.push_back(_cm);
      auto _cm_dirs = _cm.generate_all_directions();
      for (auto& _cm_dir : _cm_dirs) {
        solutions_all_dir.insert(std::move(_cm_dir.data));
      }
    }
  }
};

int main() {
  Block b1(2, 2, 1, {1, 1, 1, 0}, 'V');
  Block b2(2, 3, 1, {1, 1, 0, 0, 1, 1}, 'Z');
  Block b3(2, 2, 2, {0, 0, 1, 0, 1, 1, 1, 0}, 'B');
  Block b4(2, 3, 1, {1, 1, 1, 0, 1, 0}, 'T');
  Block b5(2, 2, 2, {1, 1, 1, 0, 1, 0, 0, 0}, 'P');
  Block b6(2, 2, 2, {0, 1, 0, 0, 1, 1, 1, 0}, 'A');
  Block b7(2, 3, 1, {1, 1, 1, 1, 0, 0}, 'L');

  std::vector<Block> blocks({b7, b5, b1, b2, b3, b4, b6});

  Solver s(3, 3, 3, blocks);

  s.find_solution();

  s.print_solution();

  return 0;
}
