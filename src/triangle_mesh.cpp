#include "graphics_pipeline/triangle_mesh.h"

#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <numbers>

#include "graphics_pipeline/vector_3.h"

void TriangleMesh::LoadBinary(char *file_name) {
  std::ifstream input(file_name, std::ios::binary);
  if (input.fail()) {
    std::cerr << "INFO: cannot open file: " << file_name << '\n';
    return;
  }

  int vertices_count;
  input.read(reinterpret_cast<char *>(&vertices_count), sizeof(int));

  char has_data;
  input.read(&has_data, 1);
  if (has_data != 'y') {
    std::cerr << "INTERNAL ERROR: there should always be vertex xyz data" << '\n';
    return;
  }

  vertices.clear();
  vertices.resize(vertices_count);

  input.read(&has_data, 1);
  colors.clear();
  if (has_data == 'y') {
    colors.resize(vertices_count);
  }

  input.read(&has_data, 1);
  normals.clear();
  if (has_data == 'y') {
    normals.resize(vertices_count);
  }

  input.read(&has_data, 1);
  std::vector<float> texture_coordinates;
  if (has_data == 'y') {
    texture_coordinates.resize(static_cast<size_t>(vertices_count) * 2);
  }

  input.read(reinterpret_cast<char *>(vertices.data()),
             static_cast<std::streamsize>(static_cast<size_t>(vertices_count) * 3 * sizeof(float)));

  if (!colors.empty()) {
    input.read(reinterpret_cast<char *>(colors.data()),
               static_cast<std::streamsize>(static_cast<size_t>(vertices_count) * 3 * sizeof(float)));
  }

  if (!normals.empty()) {
    input.read(reinterpret_cast<char *>(normals.data()),
               static_cast<std::streamsize>(static_cast<size_t>(vertices_count) * 3 * sizeof(float)));
  }

  if (!texture_coordinates.empty()) {
    input.read(reinterpret_cast<char *>(texture_coordinates.data()),
               static_cast<std::streamsize>(static_cast<size_t>(vertices_count) * 2 * sizeof(float)));
  }

  int triangles_count;
  input.read(reinterpret_cast<char *>(&triangles_count), sizeof(int));
  triangles.clear();
  triangles.resize(static_cast<size_t>(triangles_count) * 3);
  input.read(reinterpret_cast<char *>(triangles.data()),
             static_cast<std::streamsize>(static_cast<size_t>(triangles_count) * 3 * sizeof(unsigned int)));

  input.close();

  std::cerr << "INFO: loaded " << vertices_count << " verts, " << triangles_count << " tris from" << '\n';
  std::cerr << "      " << file_name << '\n';
  std::cerr << "      xyz " << ((!colors.empty()) ? "rgb " : "") << ((!normals.empty()) ? "nxnynz " : "")
            << ((!texture_coordinates.empty()) ? "tcstct " : "") << '\n';
}

void TriangleMesh::SaveBinary(char *file_name) {
  std::ofstream output(file_name, std::ios::binary);
  if (output.fail()) {
    std::cerr << "INFO: cannot open file: " << file_name << '\n';
    return;
  }

  int vertices_count = static_cast<int>(vertices.size());
  output.write(reinterpret_cast<const char *>(&vertices_count), sizeof(int));

  char has_data = 'y';
  output.write(&has_data, 1);

  has_data = (!colors.empty() && colors.size() == vertices.size()) ? 'y' : 'n';
  output.write(&has_data, 1);

  has_data = (!normals.empty() && normals.size() == vertices.size()) ? 'y' : 'n';
  output.write(&has_data, 1);

  has_data = 'n';
  output.write(&has_data, 1);

  output.write(reinterpret_cast<const char *>(vertices.data()),
               static_cast<std::streamsize>(static_cast<size_t>(vertices_count) * 3 * sizeof(float)));

  if (!colors.empty() && colors.size() == vertices.size()) {
    output.write(reinterpret_cast<const char *>(colors.data()),
                 static_cast<std::streamsize>(static_cast<size_t>(vertices_count) * 3 * sizeof(float)));
  }

  if (!normals.empty() && normals.size() == vertices.size()) {
    output.write(reinterpret_cast<const char *>(normals.data()),
                 static_cast<std::streamsize>(static_cast<size_t>(vertices_count) * 3 * sizeof(float)));
  }

  int triangles_count = static_cast<int>(triangles.size()) / 3;
  output.write(reinterpret_cast<const char *>(&triangles_count), sizeof(int));
  output.write(reinterpret_cast<const char *>(triangles.data()),
               static_cast<std::streamsize>(static_cast<size_t>(triangles_count) * 3 * sizeof(unsigned int)));

  output.close();

  std::cerr << "INFO: saved " << vertices_count << " verts, " << triangles_count << " tris to" << '\n';
  std::cerr << "      " << file_name << '\n';
}

auto TriangleMesh::GetPosition() -> Vector3 {
  Vector3 ret;

  for (auto &vertex : vertices) {
    ret = ret + vertex;
  }

  ret = ret / (float)vertices.size();

  return ret;
}

void TriangleMesh::SetPosition(Vector3 new_position) { Translate(GetPosition() - new_position); }

void TriangleMesh::Scale(float factor) {
  Vector3 position = GetPosition();
  for (auto &vertex : vertices) {
    vertex = position + (vertex - position) * factor;
  }
}

void TriangleMesh::Translate(Vector3 vector) {
  for (auto &vertex : vertices) {
    vertex = vertex + vector;
  }
}

void TriangleMesh::RotateAboutAxis(Vector3 origin, Vector3 direction, float angle) {
  for (auto &vertex : vertices) {
    vertex = vertex.RotateAboutAxis(origin, direction, angle);
  }
}

void TriangleMesh::LightDirection(Vector3 light_direction, float ambient_coefficient) {
  for (int i = 0; i < vertices.size(); i++) {
    colors[i] = Vector3(1.0F, 0.0F, 0.0F);
    colors[i] = colors[i].Light(normals[i], light_direction, ambient_coefficient);
  }
}

void TriangleMesh::LightPoint(Vector3 light_point, float ambient_coefficient) {
  for (int i = 0; i < vertices.size(); i++) {
    colors[i] = Vector3(1.0F, 0.0F, 0.0F);
    Vector3 light_direction = (light_point - vertices[i]).GetNormal();
    colors[i] = colors[i].Light(normals[i], light_direction, ambient_coefficient);
  }
}

auto TriangleMesh::AxisAlignedBox(Vector3 min_corner, Vector3 max_corner, unsigned int color) -> TriangleMesh {
  TriangleMesh mesh;

  constexpr int NUM_VERTICES = 8;
  constexpr int NUM_TRIANGLES = 12;
  constexpr int NUM_INDICES = NUM_TRIANGLES * 3;

  // Vertex positions: 8 corners of the box
  const std::array<Vector3, NUM_VERTICES> vertex_positions = {
      Vector3(min_corner[0], min_corner[1], min_corner[2]), // 0: min xyz
      Vector3(max_corner[0], min_corner[1], min_corner[2]), // 1: max x, min yz
      Vector3(max_corner[0], max_corner[1], min_corner[2]), // 2: max xy, min z
      Vector3(min_corner[0], max_corner[1], min_corner[2]), // 3: min x, max y, min z
      Vector3(min_corner[0], min_corner[1], max_corner[2]), // 4: min xy, max z
      Vector3(max_corner[0], min_corner[1], max_corner[2]), // 5: max xz, min y
      Vector3(max_corner[0], max_corner[1], max_corner[2]), // 6: max xyz
      Vector3(min_corner[0], max_corner[1], max_corner[2])  // 7: min x, max yz
  };

  // Triangle indices (2 triangles per face, 6 faces)
  constexpr std::array<unsigned int, NUM_INDICES> triangle_indices = {
      0, 1, 2, 0, 2, 3, // Front face (z = min)
      5, 4, 7, 5, 7, 6, // Back face (z = max)
      4, 0, 3, 4, 3, 7, // Left face (x = min)
      1, 5, 6, 1, 6, 2, // Right face (x = max)
      4, 5, 1, 4, 1, 0, // Bottom face (y = min)
      3, 2, 6, 3, 6, 7  // Top face (y = max)
  };

  // Normal directions for each vertex (normalized corner directions)
  constexpr std::array<std::array<float, 3>, NUM_VERTICES> normal_data = {{{-1.0F, -1.0F, -1.0F},
                                                                           {1.0F, -1.0F, -1.0F},
                                                                           {1.0F, 1.0F, -1.0F},
                                                                           {-1.0F, 1.0F, -1.0F},
                                                                           {-1.0F, -1.0F, 1.0F},
                                                                           {1.0F, -1.0F, 1.0F},
                                                                           {1.0F, 1.0F, 1.0F},
                                                                           {-1.0F, 1.0F, 1.0F}}};

  mesh.vertices.resize(NUM_VERTICES);
  mesh.normals.resize(NUM_VERTICES);
  mesh.colors.resize(NUM_VERTICES);

  Vector3 vertex_color;
  vertex_color.SetColor(color);

  for (int i = 0; i < NUM_VERTICES; i++) {
    mesh.vertices[i] = vertex_positions[i];
    mesh.normals[i] = Vector3(normal_data[i][0], normal_data[i][1], normal_data[i][2]).GetNormal();
    mesh.colors[i] = vertex_color;
  }

  mesh.triangles.assign(triangle_indices.begin(), triangle_indices.end());

  return mesh;
}

auto TriangleMesh::Sphere(Vector3 position, float radius, int subdivisions, unsigned int color) -> TriangleMesh {
  TriangleMesh mesh;

  constexpr float GOLDEN_RATIO = std::numbers::phi_v<float>;
  constexpr float ICOSAHEDRON_SCALE = 0.525731112119133606F;
  constexpr int ICOSAHEDRON_VERTICES = 12;
  constexpr int ICOSAHEDRON_TRIANGLES = 20;
  constexpr int ICOSAHEDRON_INDICES = ICOSAHEDRON_TRIANGLES * 3;

  // Icosahedron vertex positions (before scaling)
  constexpr std::array<std::array<float, 3>, ICOSAHEDRON_VERTICES> icosahedron_data = {{{-1.0F, GOLDEN_RATIO, 0.0F},
                                                                                        {1.0F, GOLDEN_RATIO, 0.0F},
                                                                                        {-1.0F, -GOLDEN_RATIO, 0.0F},
                                                                                        {1.0F, -GOLDEN_RATIO, 0.0F},
                                                                                        {0.0F, -1.0F, GOLDEN_RATIO},
                                                                                        {0.0F, 1.0F, GOLDEN_RATIO},
                                                                                        {0.0F, -1.0F, -GOLDEN_RATIO},
                                                                                        {0.0F, 1.0F, -GOLDEN_RATIO},
                                                                                        {GOLDEN_RATIO, 0.0F, -1.0F},
                                                                                        {GOLDEN_RATIO, 0.0F, 1.0F},
                                                                                        {-GOLDEN_RATIO, 0.0F, -1.0F},
                                                                                        {-GOLDEN_RATIO, 0.0F, 1.0F}}};

  // Icosahedron triangle indices
  constexpr std::array<unsigned int, ICOSAHEDRON_INDICES> icosahedron_triangle_indices = {
      0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11, 1, 5, 9, 5, 11, 4,  11, 10, 2,  10, 7, 6, 7, 1, 8,
      3, 9,  4, 3, 4, 2, 3, 2, 6, 3, 6, 8,  3, 8,  9,  4, 9, 5, 2, 4,  11, 6,  2,  10, 8,  6, 7, 9, 8, 1};

  mesh.vertices.resize(ICOSAHEDRON_VERTICES);
  for (int i = 0; i < ICOSAHEDRON_VERTICES; i++) {
    Vector3 vertex(icosahedron_data[i][0], icosahedron_data[i][1], icosahedron_data[i][2]);
    mesh.vertices[i] = vertex.GetNormal() * (radius * ICOSAHEDRON_SCALE);
  }

  mesh.triangles.assign(icosahedron_triangle_indices.begin(), icosahedron_triangle_indices.end());

  // Subdivide
  for (int subdivision = 0; subdivision < subdivisions; subdivision++) {
    std::vector<unsigned int> new_triangles;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> midpoint_cache;

    auto get_midpoint = [&](unsigned int index_a, unsigned int index_b) -> unsigned int {
      std::pair<unsigned int, unsigned int> key =
          (index_a < index_b) ? std::make_pair(index_a, index_b) : std::make_pair(index_b, index_a);

      auto iterator = midpoint_cache.find(key);
      if (iterator != midpoint_cache.end()) {
        return iterator->second;
      }

      Vector3 vertex_a = mesh.vertices[index_a];
      Vector3 vertex_b = mesh.vertices[index_b];
      constexpr float HALF = 0.5F;
      Vector3 midpoint = ((vertex_a + vertex_b) * HALF).GetNormal() * radius;

      auto new_index = static_cast<unsigned int>(mesh.vertices.size());
      mesh.vertices.push_back(midpoint);
      midpoint_cache[key] = new_index;

      return new_index;
    };

    constexpr int VERTICES_PER_TRIANGLE = 3;
    constexpr int SUBDIVIDED_TRIANGLES = 4;
    constexpr int INDICES_PER_SUBDIVIDED_TRIANGLE = SUBDIVIDED_TRIANGLES * VERTICES_PER_TRIANGLE;

    for (size_t i = 0; i < mesh.triangles.size(); i += VERTICES_PER_TRIANGLE) {
      unsigned int vertex_0 = mesh.triangles[i];
      unsigned int vertex_1 = mesh.triangles[i + 1];
      unsigned int vertex_2 = mesh.triangles[i + 2];

      unsigned int mid_01 = get_midpoint(vertex_0, vertex_1);
      unsigned int mid_12 = get_midpoint(vertex_1, vertex_2);
      unsigned int mid_20 = get_midpoint(vertex_2, vertex_0);

      const std::array<unsigned int, INDICES_PER_SUBDIVIDED_TRIANGLE> subdivided_indices = {
          vertex_0, mid_01, mid_20, vertex_1, mid_12, mid_01, vertex_2, mid_20, mid_12, mid_01, mid_12, mid_20};

      new_triangles.insert(new_triangles.end(), subdivided_indices.begin(), subdivided_indices.end());
    }

    mesh.triangles = new_triangles;
  }

  // Translate to position and set normals
  mesh.normals.resize(mesh.vertices.size());
  for (size_t i = 0; i < mesh.vertices.size(); i++) {
    mesh.normals[i] = mesh.vertices[i].GetNormal();
    mesh.vertices[i] = mesh.vertices[i] + position;
  }

  // Set colors
  mesh.colors.resize(mesh.vertices.size());
  Vector3 vertex_color;
  vertex_color.SetColor(color);
  for (size_t i = 0; i < mesh.vertices.size(); i++) {
    mesh.colors[i] = vertex_color;
  }

  return mesh;
}

auto TriangleMesh::Cylinder(Vector3 position, float radius, float height, int subdivisions, unsigned int color)
    -> TriangleMesh {
  TriangleMesh mesh;

  constexpr float TWO_PI = 6.283185307179586F;
  constexpr float HALF = 0.5F;
  constexpr int CENTERS_COUNT = 2;
  constexpr int TRIANGLES_PER_SIDE_QUAD = 2;
  constexpr int VERTICES_PER_TRIANGLE = 3;

  float angle_step = TWO_PI / static_cast<float>(subdivisions);
  float half_height = height * HALF;

  int vertices_per_circle = subdivisions;
  int total_vertices = (vertices_per_circle * 2) + CENTERS_COUNT;

  mesh.vertices.resize(total_vertices);
  mesh.normals.resize(total_vertices);
  mesh.colors.resize(total_vertices);

  Vector3 vertex_color;
  vertex_color.SetColor(color);

  // Bottom and top circle vertices
  for (int i = 0; i < subdivisions; i++) {
    float angle = static_cast<float>(i) * angle_step;
    float x_coord = radius * cosf(angle);
    float z_coord = radius * sinf(angle);

    // Bottom circle
    mesh.vertices[i] = position + Vector3(x_coord, -half_height, z_coord);
    mesh.normals[i] = Vector3(x_coord, 0.0F, z_coord).GetNormal();
    mesh.colors[i] = vertex_color;

    // Top circle
    mesh.vertices[subdivisions + i] = position + Vector3(x_coord, half_height, z_coord);
    mesh.normals[subdivisions + i] = Vector3(x_coord, 0.0F, z_coord).GetNormal();
    mesh.colors[subdivisions + i] = vertex_color;
  }

  // Center vertices
  int bottom_center_index = subdivisions * 2;
  int top_center_index = (subdivisions * 2) + 1;

  mesh.vertices[bottom_center_index] = position + Vector3(0.0F, -half_height, 0.0F);
  mesh.normals[bottom_center_index] = Vector3(0.0F, -1.0F, 0.0F);
  mesh.colors[bottom_center_index] = vertex_color;

  mesh.vertices[top_center_index] = position + Vector3(0.0F, half_height, 0.0F);
  mesh.normals[top_center_index] = Vector3(0.0F, 1.0F, 0.0F);
  mesh.colors[top_center_index] = vertex_color;

  // Create triangles
  int triangles_count = subdivisions * (TRIANGLES_PER_SIDE_QUAD + 2);
  mesh.triangles.resize(static_cast<long>(triangles_count) * VERTICES_PER_TRIANGLE);

  int triangle_index = 0;

  // Side triangles (2 triangles per subdivision to form quads)
  for (int i = 0; i < subdivisions; i++) {
    int next = (i + 1) % subdivisions;
    int bottom_current = i;
    int bottom_next = next;
    int top_current = subdivisions + i;
    int top_next = subdivisions + next;

    const std::array<unsigned int, 6> quad_indices = {
        static_cast<unsigned int>(bottom_current), static_cast<unsigned int>(bottom_next),
        static_cast<unsigned int>(top_current),    static_cast<unsigned int>(bottom_next),
        static_cast<unsigned int>(top_next),       static_cast<unsigned int>(top_current)};

    for (unsigned int index : quad_indices) {
      mesh.triangles[triangle_index++] = index;
    }
  }

  // Bottom cap triangles
  for (int i = 0; i < subdivisions; i++) {
    int next = (i + 1) % subdivisions;
    const std::array<unsigned int, VERTICES_PER_TRIANGLE> cap_indices = {
        static_cast<unsigned int>(bottom_center_index), static_cast<unsigned int>(next), static_cast<unsigned int>(i)};

    for (unsigned int index : cap_indices) {
      mesh.triangles[triangle_index++] = index;
    }
  }

  // Top cap triangles
  for (int i = 0; i < subdivisions; i++) {
    int next = (i + 1) % subdivisions;
    const std::array<unsigned int, VERTICES_PER_TRIANGLE> cap_indices = {
        static_cast<unsigned int>(top_center_index), static_cast<unsigned int>(subdivisions + i),
        static_cast<unsigned int>(subdivisions + next)};

    for (unsigned int index : cap_indices) {
      mesh.triangles[triangle_index++] = index;
    }
  }

  return mesh;
}