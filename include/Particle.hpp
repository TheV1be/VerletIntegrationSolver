#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

struct Particle {
  sf::Vector2f current_pos;
  sf::Vector2f old_pos;
  sf::Vector2f acceleration;
  float radius;
  sf::Color color;

  Particle(sf::Vector2f pos, float radius = 12.f,
           sf::Color color = sf::Color::Cyan);

  void update(float dt);
  void accelerate(sf::Vector2f acc);
  void setVelocity(sf::Vector2f vel, float dt);
  sf::Vector2f getVelocity(float dt) const;
};
