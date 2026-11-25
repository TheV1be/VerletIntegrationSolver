#include "Hud.hpp"
#include "Renderer.hpp"
#include "Solver.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>

int main() {
  constexpr float WIDTH = 800.f;
  constexpr float HEIGHT = 600.f;

  sf::RenderWindow window(sf::VideoMode(static_cast<unsigned>(WIDTH),
                                        static_cast<unsigned>(HEIGHT)),
                          "Verlet Physics Solver");

  window.setVerticalSyncEnabled(true);
  // window.setFramerateLimit(144);

  constexpr float GRAVITY = 1000.f;
  constexpr float BALL_RADIUS = 7.f;
  float color_phase = 0.f;

  constexpr uint32_t SUB_STEPS = 8;
  Solver solver({WIDTH, HEIGHT}, BALL_RADIUS * 2.f, SUB_STEPS);
  Renderer renderer;
  Hud hud("assets/font.ttf");

  // Time/frame config
  sf::Clock frame_clock;
  constexpr float FIXED_DT = 1.f / 60.f;
  constexpr float SUB_DT = FIXED_DT / static_cast<float>(SUB_STEPS);
  float time_accumulator = 0.f;

  // Mouse spawning controls
  float mouse_spawn_timer = 0.f;
  constexpr float MOUSE_SPAWN_INTERVAL = 0.03f;

  // Stacked top-left benchmark streams (Space: pause/resume, M: +1 stream, N:
  // -1 stream)
  bool stream_active = false;
  int stream_count = 1;
  int max_stream_count = 40;
  float stream_timer = 0.f;
  constexpr float STREAM_INTERVAL =
      std::max(0.01f, (BALL_RADIUS / 2.f) * 0.01f);
  constexpr float STREAM_VERTICAL_GAP = (2.f * BALL_RADIUS) + 1.f;

  // Attraction field configuration
  constexpr float FIELD_RADIUS = 500.f;
  constexpr float FIELD_FORCE = 10000.f;
  int field_sign = 1;

  bool color_by_speed = false;

  while (window.isOpen()) {
    // Measure real elapsed time since last frame
    float raw_frame_time = frame_clock.restart().asSeconds();

    // Clamp frame time to 0.25s max to prevent the spiralling if the
    // window stalls
    float clamped_frame_time = std::min(raw_frame_time, 0.25f);

    time_accumulator += clamped_frame_time;
    mouse_spawn_timer += clamped_frame_time;
    stream_timer += clamped_frame_time;

    // Handle all events
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R) {
          solver.clear();
        }
        if (event.key.code == sf::Keyboard::Space) {
          stream_active = !stream_active;
        }
        if (event.key.code == sf::Keyboard::M) {
          stream_count = std::min(max_stream_count, stream_count + 1);
        }
        if (event.key.code == sf::Keyboard::N) {
          stream_count = std::max(1, stream_count - 1);
        }
        if (event.key.code == sf::Keyboard::W) {
          sf::Vector2f gravity{0.f, -GRAVITY};
          solver.setGravity(gravity);
        }
        if (event.key.code == sf::Keyboard::A) {
          sf::Vector2f gravity{-GRAVITY, 0.f};
          solver.setGravity(gravity);
        }
        if (event.key.code == sf::Keyboard::S) {
          sf::Vector2f gravity{0.f, GRAVITY};
          solver.setGravity(gravity);
        }
        if (event.key.code == sf::Keyboard::D) {
          sf::Vector2f gravity{GRAVITY, 0.f};
          solver.setGravity(gravity);
        }
        if (event.key.code == sf::Keyboard::I) {
          field_sign *= -1;
        }
        if (event.key.code == sf::Keyboard::C) {
          color_by_speed = !color_by_speed;
        }
        if (event.key.code == sf::Keyboard::Num1) {
          const sf::Vector2f mouse =
              window.mapPixelToCoords(sf::Mouse::getPosition(window));
          solver.addRope(mouse, 25, BALL_RADIUS * 2.f, BALL_RADIUS,
                         sf::Color::White);
        }
        if (event.key.code == sf::Keyboard::Num2) {
          const sf::Vector2f mouse =
              window.mapPixelToCoords(sf::Mouse::getPosition(window));
          solver.addBridge(mouse, 25, BALL_RADIUS * 2.f, BALL_RADIUS,
                           sf::Color::White);
        }
        if (event.key.code == sf::Keyboard::Num3) {
          const sf::Vector2f mouse =
              window.mapPixelToCoords(sf::Mouse::getPosition(window));
          solver.addCloth(mouse, 10, 10, BALL_RADIUS * 2.f + 4.f, BALL_RADIUS,
                          sf::Color::White);
        }
      }

      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0.f, 0.f,
                                  static_cast<float>(event.size.width),
                                  static_cast<float>(event.size.height));
        window.setView(sf::View(visibleArea));
        solver.setBounds({visibleArea.width, visibleArea.height});
      }
    }

    // Space: Top-left benchmark streams
    if (stream_active && stream_timer >= STREAM_INTERVAL) {
      stream_timer = 0.f;
      color_phase += 0.08f;

      const sf::Vector2f LAUNCH_VEL{1000.f, 0.f};

      for (int i = 0; i < stream_count; ++i) {
        float phase = color_phase;
        sf::Color rainbow(
            static_cast<sf::Uint8>(128 + 127 * std::sin(phase)),
            static_cast<sf::Uint8>(128 + 127 * std::sin(phase + 2.09f)),
            static_cast<sf::Uint8>(128 + 127 * std::sin(phase + 4.18f)));

        sf::Vector2f spawn_pos{10.f, 10.f + i * STREAM_VERTICAL_GAP};
        Particle &p = solver.addParticle(spawn_pos, BALL_RADIUS, rainbow);
        p.setVelocity(LAUNCH_VEL, SUB_DT);
      }
    }

    // Left-click: manual spawn stream
    sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
    sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

    if (window.hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
        mouse_spawn_timer >= MOUSE_SPAWN_INTERVAL) {
      mouse_spawn_timer = 0.f;
      color_phase += 0.12f;

      if (world_pos.x >= 0.f &&
          world_pos.x <= static_cast<float>(window.getSize().x) &&
          world_pos.y >= 0.f &&
          world_pos.y <= static_cast<float>(window.getSize().y) &&
          solver.isSpotFree(world_pos, BALL_RADIUS)) {

        sf::Color stream_color(
            static_cast<sf::Uint8>(128 + 127 * std::sin(color_phase)),
            static_cast<sf::Uint8>(128 + 127 * std::sin(color_phase + 2.09f)),
            static_cast<sf::Uint8>(128 + 127 * std::sin(color_phase + 4.18f)));
        solver.addParticle(world_pos, BALL_RADIUS, stream_color);
      }
    }

    // Right-click: invisible attraction force field
    bool right_held =
        window.hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Right);
    solver.setForceField(right_held, world_pos, FIELD_RADIUS,
                         FIELD_FORCE * float(field_sign));

    // Fixed-Timestep Accumulator with a max catch-up cap (prevents CPU
    // snowballing)
    constexpr int MAX_STEPS_PER_FRAME = 2;
    int steps = 0;
    while (time_accumulator >= FIXED_DT && steps < MAX_STEPS_PER_FRAME) {
      solver.update(FIXED_DT);
      time_accumulator -= FIXED_DT;
      steps++;
    }
    // Discard leftover debt if the CPU hit the cap so it doesn't stay
    // permanently behind
    if (steps == MAX_STEPS_PER_FRAME && time_accumulator >= FIXED_DT) {
      time_accumulator = 0.f;
    }

    hud.update(raw_frame_time, solver.getParticles().size());

    // Render visuals as fast (or slow) as the GPU/display runs
    window.clear(sf::Color(20, 22, 28));
    renderer.render(window, solver.getParticles(), color_by_speed, SUB_DT);
    hud.draw(window);
    window.display();
  }

  return 0;
}
