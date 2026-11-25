#include "Solver.hpp"
#include <algorithm>
#include <cmath>

Solver::Solver(sf::Vector2f bounds, float cell_size, uint32_t sub_steps)
    : m_grid(bounds, cell_size), m_bounds(bounds), m_sub_steps(sub_steps) {}

Particle &Solver::addParticle(sf::Vector2f pos, float radius, sf::Color color) {
  return m_particles.emplace_back(pos, radius, color);
}

void Solver::setBounds(sf::Vector2f bounds) {
  m_bounds = bounds;
  m_grid.resize(bounds);
}

void Solver::setForceField(bool active, sf::Vector2f pos, float radius,
                           float strength) {
  m_field_active = active;
  m_field_pos = pos;
  m_field_radius = radius;
  m_field_strength = strength;
}

void Solver::update(float dt) {
  const float sub_dt = dt / static_cast<float>(m_sub_steps);
  for (uint32_t step = 0; step < m_sub_steps; ++step) {
    applyGravity();
    applyForceField();
    updatePositions(sub_dt);
    solveCollisions();
    applyWindowBounds();
  }
}

void Solver::applyGravity() {
  for (auto &p : m_particles) {
    p.accelerate(m_gravity);
  }
}

void Solver::applyForceField() {
  if (!m_field_active) {
    return;
  }

  const float radius_sq = m_field_radius * m_field_radius;

  for (auto &p : m_particles) {
    const sf::Vector2f to_target = m_field_pos - p.current_pos;
    const float dist_sq = to_target.x * to_target.x + to_target.y * to_target.y;

    if (dist_sq < radius_sq && dist_sq > 25.f) {
      const float dist = std::sqrt(dist_sq);
      const sf::Vector2f dir = to_target / dist;
      const float falloff = 1.f - (dist / m_field_radius);
      p.accelerate(dir * (m_field_strength * falloff));
    }
  }
}

void Solver::updatePositions(float dt) {
  for (auto &p : m_particles) {
    p.update(dt);
    sf::Vector2f d = p.current_pos - p.old_pos;
    const float len_sq = d.x * d.x + d.y * d.y;
    const float max_d = p.radius; // never move more than 1 radius per substep
    if (len_sq > max_d * max_d) {
      p.old_pos = p.current_pos - d * (max_d / std::sqrt(len_sq));
    }
  }
}

void Solver::applyWindowBounds() {
  auto clampAxis = [&](float &cur, float &old, float lo, float hi) {
    if (cur < lo) {
      const float v = cur - old; // <0 = moving into wall
      cur = lo;
      old = cur - (v < 0.f ? -v * m_restitution : v);
    } else if (cur > hi) {
      const float v = cur - old; // >0 = moving into wall
      cur = hi;
      old = cur - (v > 0.f ? -v * m_restitution : v);
    }
  };

  for (auto &p : m_particles) {
    clampAxis(p.current_pos.x, p.old_pos.x, p.radius, m_bounds.x - p.radius);
    clampAxis(p.current_pos.y, p.old_pos.y, p.radius, m_bounds.y - p.radius);
  }
}

void Solver::resolvePair(uint32_t idx1, uint32_t idx2) {
  Particle &p1 = m_particles[idx1];
  Particle &p2 = m_particles[idx2];

  const sf::Vector2f axis = p1.current_pos - p2.current_pos;
  const float dist_sq = axis.x * axis.x + axis.y * axis.y;
  const float min_dist = p1.radius + p2.radius;

  // Prevent division by 0 and separate overlapping balls
  if (dist_sq < min_dist * min_dist && dist_sq > 0.0001f) {
    const float dist = std::sqrt(dist_sq);
    const sf::Vector2f normal = axis / dist;
    constexpr float RESPONSE = 0.75f;
    const float overlap = 0.5f * RESPONSE * (min_dist - dist);

    p1.current_pos += normal * overlap;
    p2.current_pos -= normal * overlap;
  }
}

void Solver::solveCollisions() {
  // Clear and populate grid
  m_grid.clear();
  const uint32_t count = static_cast<uint32_t>(m_particles.size());
  for (uint32_t i = 0; i < count; ++i) {
    m_grid.addAtom(i, m_particles[i].current_pos);
  }

  const int cols = m_grid.getCols();
  const int rows = m_grid.getRows();
  const float cell_size = m_grid.getCellSize();

  // Iterate over balls
  for (uint32_t i = 0; i < count; ++i) {
    const sf::Vector2f pos = m_particles[i].current_pos;
    const int col =
        std::clamp(static_cast<int>(pos.x / cell_size), 0, cols - 1);
    const int row =
        std::clamp(static_cast<int>(pos.y / cell_size), 0, rows - 1);

    const int min_c = std::max(0, col - 1);
    const int max_c = std::min(cols - 1, col + 1);
    const int min_r = std::max(0, row - 1);
    const int max_r = std::min(rows - 1, row + 1);

    // Solve pairs with surrounding cells
    for (int nr = min_r; nr <= max_r; ++nr) {
      for (int nc = min_c; nc <= max_c; ++nc) {
        const auto &cell = m_grid.getCell(nc, nr);
        for (uint8_t k = 0; k < cell.count; ++k) {
          const uint32_t other_idx = cell.objects[k];
          if (i < other_idx) {
            resolvePair(i, other_idx);
          }
        }
      }
    }
  }
}
