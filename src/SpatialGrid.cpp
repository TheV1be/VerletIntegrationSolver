#include "SpatialGrid.hpp"
#include <algorithm>
#include <cmath>

SpatialGrid::SpatialGrid(sf::Vector2f bounds, float cell_size)
    : m_cell_size(cell_size) {
  resize(bounds);
}

void SpatialGrid::resize(sf::Vector2f bounds) {
  m_cols = std::max(1, static_cast<int>(std::ceil(bounds.x / m_cell_size)));
  m_rows = std::max(1, static_cast<int>(std::ceil(bounds.y / m_cell_size)));
  m_cells.resize(static_cast<size_t>(m_cols * m_rows));
}

void SpatialGrid::clear() {
  for (auto &cell : m_cells) {
    cell.clear();
  }
}

void SpatialGrid::addAtom(uint32_t id, sf::Vector2f pos) {
  const int col =
      std::clamp(static_cast<int>(pos.x / m_cell_size), 0, m_cols - 1);
  const int row =
      std::clamp(static_cast<int>(pos.y / m_cell_size), 0, m_rows - 1);
  m_cells[row * m_cols + col].add(id);
}
