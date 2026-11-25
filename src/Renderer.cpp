#include "Renderer.hpp"
#include <algorithm>
#include <cmath>

namespace {

constexpr float MAX_SPEED = 600.f; // px/s that maps to the "fastest" colour

sf::Color lerpColor(sf::Color a, sf::Color b, float t) {
  auto mix = [t](sf::Uint8 x, sf::Uint8 y) {
    return static_cast<sf::Uint8>(x + (y - x) * t);
  };
  return sf::Color(mix(a.r, b.r), mix(a.g, b.g), mix(a.b, b.b));
}

// t = 0 -> slow (blue), 0.5 -> yellow, 1 -> fast (red)
sf::Color speedColor(float t) {
  const sf::Color slow(40, 80, 255);
  const sf::Color mid(255, 220, 60);
  const sf::Color fast(255, 50, 40);
  t = std::clamp(t, 0.f, 1.f);
  return t < 0.5f ? lerpColor(slow, mid, t * 2.f)
                  : lerpColor(mid, fast, (t - 0.5f) * 2.f);
}

} // namespace

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
                      const std::vector<Particle> &particles,
                      bool color_by_speed, float sub_dt) {
  const size_t count = particles.size();
  // Resize vertex buffer to hold 4 corners per ball (allocates only when ball
  // count grows)
  m_vertices.resize(count * 4);

  for (size_t i = 0; i < count; ++i) {
    const Particle &p = particles[i];
    const float r = p.radius;
    const sf::Vector2f pos = p.current_pos;
    const size_t idx = i * 4;

    // Pick the colour: the particle's own, or one based on its speed
    sf::Color color = p.color;
    if (color_by_speed) {
      const sf::Vector2f d = p.current_pos - p.old_pos;
      const float speed = std::sqrt(d.x * d.x + d.y * d.y) / sub_dt;
      color = speedColor(speed / MAX_SPEED);
    }

    // Top-left corner
    m_vertices[idx + 0].position = {pos.x - r, pos.y - r};
    m_vertices[idx + 0].texCoords = {0.f, 0.f};
    m_vertices[idx + 0].color = color;

    // Top-right corner
    m_vertices[idx + 1].position = {pos.x + r, pos.y - r};
    m_vertices[idx + 1].texCoords = {m_tex_size, 0.f};
    m_vertices[idx + 1].color = color;

    // Bottom-right corner
    m_vertices[idx + 2].position = {pos.x + r, pos.y + r};
    m_vertices[idx + 2].texCoords = {m_tex_size, m_tex_size};
    m_vertices[idx + 2].color = color;

    // Bottom-left corner
    m_vertices[idx + 3].position = {pos.x - r, pos.y + r};
    m_vertices[idx + 3].texCoords = {0.f, m_tex_size};
    m_vertices[idx + 3].color = color;
  }

  // Send all balls to the GPU in one single draw call
  sf::RenderStates states;
  states.texture = &m_circle_rt.getTexture();
  window.draw(m_vertices, states);
}
