#include "Renderer.hpp"

Renderer::Renderer() : m_vertices(sf::Quads) {
  // Pre-render white circle onto an off-screen texture
  unsigned tex_dim = static_cast<unsigned>(m_tex_size);
  m_circle_rt.create(tex_dim, tex_dim);
  m_circle_rt.clear(sf::Color::Transparent);

  sf::CircleShape circle(m_tex_size * 0.5f);
  circle.setFillColor(sf::Color::White);
  circle.setPointCount(64); // Smooth edges
  m_circle_rt.draw(circle);
  m_circle_rt.display();
  m_circle_rt.setSmooth(true);
}

void Renderer::render(sf::RenderWindow &window,
                      const std::vector<Particle> &particles) {
  const size_t count = particles.size();
  // Resize vertex buffer to hold 4 corners per ball (allocates only when ball
  // count grows)
  m_vertices.resize(count * 4);

  for (size_t i = 0; i < count; ++i) {
    const Particle &p = particles[i];
    const float r = p.radius;
    const sf::Vector2f pos = p.current_pos;
    const size_t idx = i * 4;

    // Top-left corner
    m_vertices[idx + 0].position = {pos.x - r, pos.y - r};
    m_vertices[idx + 0].texCoords = {0.f, 0.f};
    m_vertices[idx + 0].color = p.color;

    // Top-right corner
    m_vertices[idx + 1].position = {pos.x + r, pos.y - r};
    m_vertices[idx + 1].texCoords = {m_tex_size, 0.f};
    m_vertices[idx + 1].color = p.color;

    // Bottom-right corner
    m_vertices[idx + 2].position = {pos.x + r, pos.y + r};
    m_vertices[idx + 2].texCoords = {m_tex_size, m_tex_size};
    m_vertices[idx + 2].color = p.color;

    // Bottom-left corner
    m_vertices[idx + 3].position = {pos.x - r, pos.y + r};
    m_vertices[idx + 3].texCoords = {0.f, m_tex_size};
    m_vertices[idx + 3].color = p.color;
  }

  // Send all balls to the GPU in one single draw call
  sf::RenderStates states;
  states.texture = &m_circle_rt.getTexture();
  window.draw(m_vertices, states);
}
