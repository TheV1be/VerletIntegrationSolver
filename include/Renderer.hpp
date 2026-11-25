#pragma once
#include "Particle.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class Renderer {
public:
  Renderer();

  void render(sf::RenderWindow &window, const std::vector<Particle> &particles);

private:
  sf::RenderTexture m_circle_rt;
  sf::VertexArray m_vertices;
  float m_tex_size = 128.f;
};
