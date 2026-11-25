#pragma once
#include "Particle.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class Renderer {
public:
  Renderer();

  void render(sf::RenderWindow &window, const std::vector<Particle> &particles,
              bool color_by_speed = false, float sub_dt = 1.f / 480.f);

private:
  sf::RenderTexture m_circle_rt;
  sf::VertexArray m_vertices;
  float m_tex_size = 128.f;
};
