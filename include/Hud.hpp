#pragma once
#include <SFML/Graphics.hpp>
#include <cstddef>
#include <string>

class Hud {
public:
  Hud(const std::string &font_path);

  void update(float frame_time_seconds, size_t ball_count);
  void draw(sf::RenderWindow &window);

private:
  sf::Font m_font;
  sf::Text m_stats_text;
  sf::RectangleShape m_background;

  float m_elapsed_time = 0.f;
  int m_frame_counter = 0;
  int m_current_fps = 60;
};
