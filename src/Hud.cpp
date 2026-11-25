#include "Hud.hpp"
#include <stdexcept>

Hud::Hud(const std::string &font_path) {
  if (!m_font.loadFromFile(font_path)) {
    throw std::runtime_error("Failed to load font: " + font_path);
  }

  m_stats_text.setFont(m_font);
  m_stats_text.setCharacterSize(16);
  m_stats_text.setFillColor(sf::Color(235, 240, 245));

  m_background.setFillColor(sf::Color(15, 17, 22, 190));
  m_background.setOutlineThickness(1.f);
  m_background.setOutlineColor(sf::Color(60, 70, 85));
}

void Hud::update(float frame_time_seconds, size_t ball_count) {
  m_elapsed_time += frame_time_seconds;
  m_frame_counter++;

  // Prevent flickering numbers
  if (m_elapsed_time >= 0.25f) {
    m_current_fps = static_cast<int>(m_frame_counter / m_elapsed_time);
    m_elapsed_time = 0.f;
    m_frame_counter = 0;
  }

  m_stats_text.setString("FPS:   " + std::to_string(m_current_fps) + "\n" +
                         "Balls: " + std::to_string(ball_count));

  sf::FloatRect bounds = m_stats_text.getLocalBounds();
  m_background.setSize({bounds.width + 20.f, bounds.height + 16.f});
}

void Hud::draw(sf::RenderWindow &window) {
  constexpr float MARGIN = 10.f;
  const float win_width = static_cast<float>(window.getSize().x);
  const float box_width = m_background.getSize().x;

  // Anchor box and text to the top-right corner
  m_background.setPosition(win_width - box_width - MARGIN, MARGIN);
  m_stats_text.setPosition(win_width - box_width - MARGIN + 8.f, MARGIN + 4.f);

  window.draw(m_background);
  window.draw(m_stats_text);
}
