#pragma once

namespace Color {
constexpr unsigned int BLACK = 0xFF000000;
constexpr unsigned int RED = 0xFFFF0000;
constexpr unsigned int GREEN = 0xFF00FF00;
constexpr unsigned int BLUE = 0xFF0000FF;
constexpr unsigned int WHITE = 0xFFFFFFFF;

constexpr float MIN_ALPHA_CHANNEL = 0.0F;
constexpr float MAX_ALPHA_CHANNEL = 255.0F;
constexpr int NUM_ALPHA_CHANNELS = 256;
} // namespace Color
