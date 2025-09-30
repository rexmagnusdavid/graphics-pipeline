#pragma once

namespace Color {
constexpr unsigned int BLACK = 0x000000FF;
constexpr unsigned int RED = 0xFF0000FF;
constexpr unsigned int GREEN = 0x00FF00FF;
constexpr unsigned int BLUE = 0x0000FFFF;
constexpr unsigned int WHITE = 0xFFFFFFFF;

constexpr float MIN_ALPHA_CHANNEL = 0.0F;
constexpr float MAX_ALPHA_CHANNEL = 255.0F;
constexpr int NUM_ALPHA_CHANNELS = 256;
} // namespace Color
