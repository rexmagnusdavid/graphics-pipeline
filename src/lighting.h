#include "graphics_pipeline/lighting.h"

#include <algorithm>
#include <cmath>

Lighting::LightSource::LightSource() : type(POINT), enabled(true) {
  constexpr float DEFAULT_LIGHT_X = 0.0F;
  constexpr float DEFAULT_LIGHT_Y = 50.0F;
  constexpr float DEFAULT_LIGHT_Z = 0.0F;
  constexpr float DEFAULT_DIRECTION_X = 0.0F;
  constexpr float DEFAULT_DIRECTION_Y = -1.0F;
  constexpr float DEFAULT_DIRECTION_Z = 0.0F;
  constexpr float DEFAULT_COLOR_R = 1.0F;
  constexpr float DEFAULT_COLOR_G = 1.0F;
  constexpr float DEFAULT_COLOR_B = 1.0F;
  constexpr float DEFAULT_SPOT_ANGLE = 0.5;

  position = Vector3(DEFAULT_LIGHT_X, DEFAULT_LIGHT_Y, DEFAULT_LIGHT_Z);
  direction = Vector3(DEFAULT_DIRECTION_X, DEFAULT_DIRECTION_Y, DEFAULT_DIRECTION_Z);
  color = Vector3(DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B);
  spot_angle = DEFAULT_SPOT_ANGLE;
}

Lighting::Lighting() {
  constexpr float DEFAULT_CONSTANT_ATTENUATION = 1.0F;
  constexpr float DEFAULT_LINEAR_ATTENUATION = 0.01F;
  constexpr float DEFAULT_QUADRATIC_ATTENUATION = 0.001F;

  constant_attenuation_factor = DEFAULT_CONSTANT_ATTENUATION;
  linear_attenuation_factor = DEFAULT_LINEAR_ATTENUATION;
  quadratic_attenuation_factor = DEFAULT_QUADRATIC_ATTENUATION;
}

auto Lighting::ComputeLighting(const Vector3 &point, const Vector3 &normal, const Vector3 &view_direction,
                               const std::vector<LightSource> &lights, float ambient_coefficient,
                               float specular_exponent) -> Vector3 {
  Vector3 result_color(ambient_coefficient, ambient_coefficient, ambient_coefficient);

  constexpr float DIRECTION_NEGATION = -1.0F;
  constexpr float MIN_LIGHT_CONTRIBUTION = 0.0F;
  constexpr float MIN_COLOR_VALUE = 0.0F;
  constexpr float MAX_COLOR_VALUE = 1.0F;
  constexpr float SPOTLIGHT_FALLOFF_EXPONENT = 2.0F;

  for (const auto &light : lights) {
    if (!light.enabled) {
      continue;
    }

    Vector3 light_direction;
    float attenuation = 1.0F;

    if (light.type == LightSource::DIRECTIONAL) {
      Vector3 light_dir_copy = light.direction;
      light_direction = light_dir_copy * DIRECTION_NEGATION;
      light_direction = light_direction.GetNormal();
    } else if (light.type == LightSource::POINT) {
      Vector3 light_pos_copy = light.position;
      Vector3 point_copy = point;
      light_direction = light_pos_copy - point_copy;
      float distance = light_direction.GetMagnitude();
      light_direction = light_direction.GetNormal();
      attenuation = ComputeAttenuation(distance);
    } else if (light.type == LightSource::SPOT) {
      Vector3 light_pos_copy = light.position;
      Vector3 point_copy = point;
      light_direction = light_pos_copy - point_copy;
      float distance = light_direction.GetMagnitude();
      light_direction = light_direction.GetNormal();
      attenuation = ComputeAttenuation(distance);

      Vector3 spotlight_dir_copy = light.direction;
      Vector3 spotlight_direction = spotlight_dir_copy.GetNormal();
      Vector3 light_dir_copy = light_direction;
      Vector3 negated_light_direction = light_dir_copy * DIRECTION_NEGATION;
      float spotlight_factor = negated_light_direction.Dot(spotlight_direction);
      float cosine_angle = cosf(light.spot_angle);

      if (spotlight_factor < cosine_angle) {
        continue;
      }

      attenuation *= powf(spotlight_factor, SPOTLIGHT_FALLOFF_EXPONENT);
    }

    Vector3 normal_copy = normal;
    Vector3 light_dir_copy = light_direction;
    float diffuse = std::max(MIN_LIGHT_CONTRIBUTION, normal_copy.Dot(light_dir_copy));
    Vector3 light_color_copy = light.color;
    Vector3 diffuse_color = light_color_copy * diffuse;

    Vector3 light_dir_copy2 = light_direction;
    Vector3 view_dir_copy = view_direction;
    Vector3 halfway_vector = light_dir_copy2 + view_dir_copy;
    halfway_vector = halfway_vector.GetNormal();
    Vector3 normal_copy2 = normal;
    float specular = powf(std::max(MIN_LIGHT_CONTRIBUTION, normal_copy2.Dot(halfway_vector)), specular_exponent);
    Vector3 light_color_copy2 = light.color;
    Vector3 specular_color = light_color_copy2 * specular;

    Vector3 combined_color = diffuse_color + specular_color;
    Vector3 attenuated_color = combined_color * attenuation;
    result_color = result_color + attenuated_color;
  }

  result_color[0] = std::clamp(result_color[0], MIN_COLOR_VALUE, MAX_COLOR_VALUE);
  result_color[1] = std::clamp(result_color[1], MIN_COLOR_VALUE, MAX_COLOR_VALUE);
  result_color[2] = std::clamp(result_color[2], MIN_COLOR_VALUE, MAX_COLOR_VALUE);

  return result_color;
}