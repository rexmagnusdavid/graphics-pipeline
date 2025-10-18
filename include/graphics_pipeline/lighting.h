#pragma once

#include <vector>

#include "vector_3.h"

class Lighting {
public:
  struct LightSource {
    enum Type : std::uint8_t { POINT, DIRECTIONAL, SPOT };
    Type type;
    Vector3 position;
    Vector3 direction;
    Vector3 color;
    float spot_angle;
    bool enabled;
    LightSource();
  };

  float constant_attenuation_factor;
  float linear_attenuation_factor;
  float quadratic_attenuation_factor;
  Lighting();

  auto ComputeAttenuation(float distance) -> float;
  auto ComputeLighting(const Vector3 &point, const Vector3 &normal, const Vector3 &view_direction,
                       const std::vector<LightSource> &lights, float ambient_coefficient, float specular_exponent)
      -> Vector3;
};