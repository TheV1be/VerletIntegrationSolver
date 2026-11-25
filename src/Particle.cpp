#include "Particle.hpp"

Particle::Particle(sf::Vector2f pos, float r, sf::Color c)
    : current_pos(pos), old_pos(pos), acceleration(0.f, 0.f), radius(r),
      color(c) {}

void Particle::update(float dt) {
  if (pinned) {
    acceleration = {0.f, 0.f};
    old_pos = current_pos;
    return;
  }
  const sf::Vector2f displacement = current_pos - old_pos;

  const sf::Vector2f new_pos =
      current_pos + displacement + (acceleration) * (dt * dt);
  old_pos = current_pos;
  current_pos = new_pos;
  acceleration = {0.f, 0.f};
}

void Particle::accelerate(sf::Vector2f acc) { acceleration += acc; }

void Particle::setVelocity(sf::Vector2f vel, float dt) {
  old_pos = current_pos - (vel * dt);
}

sf::Vector2f Particle::getVelocity(float dt) const {
  return (current_pos - old_pos) / dt;
}
