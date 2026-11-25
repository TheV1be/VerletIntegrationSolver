#pragma once
#include "Particle.hpp"
#include "SpatialGrid.hpp"
#include <cstdint>
#include <vector>

class Solver {
public:
  Solver(sf::Vector2f bounds, float cell_size = 16.f, uint32_t sub_steps = 8);

  Particle &addParticle(sf::Vector2f pos, float radius = 8.f,
                        sf::Color color = sf::Color::Cyan);
  void clear() { m_particles.clear(); }
  void update(float dt);

  void setForceField(bool active, sf::Vector2f pos = {0.f, 0.f},
                     float radius = 500.f, float strength = 10000.f);

  const std::vector<Particle> &getParticles() const { return m_particles; }
  void setGravity(sf::Vector2f gravity) { m_gravity = gravity; }
  void setBounds(sf::Vector2f bounds);

private:
  void applyGravity();
  void applyForceField();
  void updatePositions(float dt);
  void applyWindowBounds();
  void solveCollisions();
  void resolvePair(uint32_t idx1, uint32_t idx2);

  std::vector<Particle> m_particles;
  SpatialGrid m_grid;
  sf::Vector2f m_bounds;
  sf::Vector2f m_gravity{0.f, 1000.f};
  uint32_t m_sub_steps;
  float m_restitution = 0.85f;

  bool m_field_active = false;
  sf::Vector2f m_field_pos{0.f, 0.f};
  float m_field_radius = 500.f;
  float m_field_strength = 10000.f;
};
