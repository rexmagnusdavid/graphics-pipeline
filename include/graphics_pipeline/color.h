#pragma once

namespace Color {
constexpr unsigned int BLACK = 0x000000FF;
constexpr unsigned int RED = 0xFF0000FF;
constexpr unsigned int GREEN = 0x00FF00FF;
constexpr unsigned int BLUE = 0x0000FFFF;
constexpr unsigned int WHITE = 0xFFFFFFFF;
constexpr unsigned int YELLOW = 0xFFFF00FF;
constexpr unsigned int MAGENTA = 0xFF00FFFF;
constexpr unsigned int CYAN = 0x00FFFFFF;

constexpr int RED_SHIFT = 24;
constexpr int GREEN_SHIFT = 16;
constexpr int BLUE_SHIFT = 8;

constexpr float MIN_ALPHA_CHANNEL = 0.0F;
constexpr float MAX_ALPHA_CHANNEL = 255.0F;
constexpr int NUM_ALPHA_CHANNELS = 256;
constexpr unsigned int ALPHA_CHANNEL_MASK = 0xFF;
} // namespace Color
