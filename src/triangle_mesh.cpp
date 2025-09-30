#include "graphics_pipeline/triangle_mesh.h"

auto TriangleMesh::GetPosition() -> Vector3 {
  Vector3 ret;

  for (auto vertex : vertices) {
    ret = ret + vertex;
  }

  return ret / (float)vertices.size();
}

void TriangleMesh::SetPosition(Vector3 new_position) {
  Translate(GetPosition() - new_position);
}

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

void TriangleMesh::RotateAboutAxis(Vector3 origin, Vector3 direction,
                                   float angle) {
  for (auto &vertex : vertices) {
    vertex = vertex.RotateAboutAxis(origin, direction, angle);
  }
}

void TriangleMesh::DrawPoints(PlanarPinholeCamera *camera,
                              Framebuffer *framebuffer, unsigned int color,
                              int size) {
  for (auto vertex : vertices) {
    framebuffer->Draw3DPoint(camera, vertex, size, color);
  }
}

void TriangleMesh::DrawWireFrame(PlanarPinholeCamera *camera,
                                 Framebuffer *framebuffer, unsigned int color) {
  for (size_t i = 0; i < triangles.size(); i += 3) {
    unsigned int index_0 = triangles[i + 0];
    unsigned int index_1 = triangles[i + 1];
    unsigned int index_2 = triangles[i + 2];

    std::array<Vector3, 3> current_triangle_vertices = {
        vertices[index_0], vertices[index_1], vertices[index_2]};

    bool has_vertex_colors = !colors.empty();
    std::array<Vector3, 3> current_vertex_colors;
    if (has_vertex_colors) {
      current_vertex_colors = {colors[index_0], colors[index_1],
                               colors[index_2]};
    }

    for (int edge_index = 0; edge_index < 3; ++edge_index) {
      const Vector3 &start_point = current_triangle_vertices[edge_index];
      const Vector3 &end_point =
          current_triangle_vertices[(edge_index + 1) % 3];

      if (has_vertex_colors) {
        const Vector3 &start_color = current_vertex_colors[edge_index];
        const Vector3 &end_color = current_vertex_colors[(edge_index + 1) % 3];
        framebuffer->Draw3DSegment(camera, start_point, end_point, start_color,
                                   end_color);
        continue;
      }

      framebuffer->Draw3DSegment(camera, start_point, end_point, color);
    }
  }
}