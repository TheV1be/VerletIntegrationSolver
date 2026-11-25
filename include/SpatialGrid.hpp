#pragma once
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstdint>
#include <vector>

class SpatialGrid {
public:
  struct Cell {
    uint8_t count = 0;
    std::array<uint32_t, 8> objects{};

    void add(uint32_t id) {
      if (count < 8) {
        objects[count++] = id;
      }
    }

    void clear() { count = 0; }
  };

  SpatialGrid(sf::Vector2f bounds, float cell_size = 16.f);

  void resize(sf::Vector2f bounds);
  void clear();
  void addAtom(uint32_t id, sf::Vector2f pos);

  int getCols() const { return m_cols; }
  int getRows() const { return m_rows; }
  float getCellSize() const { return m_cell_size; }
  const Cell &getCell(int col, int row) const {
    return m_cells[row * m_cols + col];
  }

private:
  float m_cell_size;
  int m_cols = 0;
  int m_rows = 0;
  std::vector<Cell> m_cells;
};
