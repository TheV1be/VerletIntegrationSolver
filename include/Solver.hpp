#pragma once
#include "Constraint.hpp"
#include "Particle.hpp"
#include "SpatialGrid.hpp"
#include <cstdint>
#include <vector>

class Solver {
public:
  Solver(sf::Vector2f bounds, float cell_size = 16.f, uint32_t sub_steps = 8);

  Particle &addParticle(sf::Vector2f pos, float radius = 8.f,
                        sf::Color color = sf::Color::Cyan);
  Constraint &addConstraint(uint32_t idx1, uint32_t idx2, float length);
  void addRope(sf::Vector2f start, int count, float spacing, float radius,
               sf::Color color);
  void addBridge(sf::Vector2f start, int count, float spacing, float radius,
                 sf::Color color);
  void addCloth(sf::Vector2f top_left, int w, int h, float spacing,
                float radius, sf::Color color);
  void clear() {
    m_particles.clear();
    m_constraints.clear();
  }
  void update(float dt);

  void setForceField(bool active, sf::Vector2f pos = {0.f, 0.f},
                     float radius = 500.f, float strength = 10000.f);

  const std::vector<Particle> &getParticles() const { return m_particles; }
  const std::vector<Constraint> &getConstraints() const {
    return m_constraints;
  }
  void setGravity(sf::Vector2f gravity) { m_gravity = gravity; }
  void setBounds(sf::Vector2f bounds);
  bool isSpotFree(sf::Vector2f pos, float radius) const;

private:
  void applyGravity();
  void applyForceField();
  void updatePositions(float dt);
  void applyWindowBounds();
  void solveCollisions();
  void resolvePair(uint32_t idx1, uint32_t idx2);
  void solveConstraints();

  std::vector<Particle> m_particles;
  std::vector<Constraint> m_constraints;
  SpatialGrid m_grid;
  sf::Vector2f m_bounds;
  sf::Vector2f m_gravity{0.f, 1000.f};
  uint32_t m_sub_steps;
  float m_restitution = 0.8f;

  bool m_field_active = false;
  sf::Vector2f m_field_pos{0.f, 0.f};
  float m_field_radius = 500.f;
  float m_field_strength = 10000.f;
};
