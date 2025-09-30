#include "graphics_pipeline/vector_3.h"
#include "graphics_pipeline/matrix_3x3.h"

#include <cmath>

Vector3::Vector3(float x_coordinate, float y_coordinate, float z_coordinate) {
	coordinates[0] = x_coordinate;
	coordinates[1] = y_coordinate;
	coordinates[2] = z_coordinate;
}

auto operator>>(std::istream& input, Vector3& vector) -> std::istream& {
	return input >> vector[0] >> vector[1] >> vector[2];
}

auto operator<<(std::ostream& output, Vector3 vector) -> std::ostream& {
	return output << vector[0] << " " << vector[1] << " " << vector[2];
}

auto Vector3::operator[](int index) -> float& {
	return coordinates.at(index);
}

auto Vector3::operator+(Vector3 right) -> Vector3 {
	Vector3 ret;
	Vector3& vector = (*this);

	ret[0] = vector[0] + right[0];
	ret[1] = vector[1] + right[1];
	ret[2] = vector[2] + right[2];

	return ret;
}

auto Vector3::operator-(Vector3 right) -> Vector3 {
	Vector3 ret;
	Vector3& vector = (*this);

	ret[0] = vector[0] - right[0];
	ret[1] = vector[1] - right[1];
	ret[2] = vector[2] - right[2];

	return ret;
}

auto Vector3::operator*(float right) -> Vector3 {
	Vector3 ret;
	Vector3& vector = (*this);

	ret[0] = vector[0] * right;
	ret[1] = vector[1] * right;
	ret[2] = vector[2] * right;

	return ret;
}

auto Vector3::operator/(float right) -> Vector3 {
	Vector3 ret;
	Vector3& vector = (*this);

	ret[0] = vector[0] / right;
	ret[1] = vector[1] / right;
	ret[2] = vector[2] / right;

	return ret;
}

auto Vector3::GetMagnitude() -> float {
	float ret = NAN;
	Vector3& vector = (*this);

	ret = sqrtf(vector.Dot(vector));

	return ret;
}


auto Vector3::GetNormal() -> Vector3 {
	Vector3 ret;
	Vector3& vector = (*this);

	ret = vector / vector.GetMagnitude();

	return ret;
}

auto Vector3::Cross(Vector3 with) -> Vector3 {
	Vector3 ret;
	Vector3& vector = (*this);

	ret[0] = vector[1] * with[2] - vector[2] * with[1];
	ret[1] = vector[2] * with[0] - vector[0] * with[2];
	ret[2] = vector[0] * with[1] - vector[1] * with[0];

	return ret;
}

auto Vector3::Dot(Vector3 with) -> float {
	float ret = NAN;
	Vector3& vector = (*this);

	ret = vector[0] * with[0] + vector[1] * with[1] + vector[2] * with[2];

	return ret;
}

auto Vector3::RotateAboutAxis(Vector3 origin, Vector3 direction, float angle) -> Vector3 {
    Vector3 ret;
    Vector3& vector = (*this);

    Matrix3x3 local_coordinate_system;

    Vector3 local_y_axis = direction.GetNormal();
    local_coordinate_system[1] = local_y_axis;
    Vector3 auxiliary = Vector3(0.0F, 1.0F, 0.0F);
    if (fabsf(local_y_axis[0]) < fabsf(local_y_axis[1])) {
        auxiliary = Vector3(1.0F, 0.0F, 0.0F);
    }

    Vector3 local_x_axis = (auxiliary.Cross(local_y_axis)).GetNormal();
    local_coordinate_system[0] = local_x_axis;
    Vector3 local_z_axis = local_x_axis.Cross(local_y_axis);
    local_coordinate_system[2] = local_z_axis;

    Vector3 point_in_local_space = local_coordinate_system * (vector - origin);

    Matrix3x3 y_axis_rotation_matrix;
    y_axis_rotation_matrix.RotateAboutY(angle);
    Vector3 rotated_point_in_local_space = y_axis_rotation_matrix * point_in_local_space;

    ret = local_coordinate_system.GetInverse() * rotated_point_in_local_space + origin;

    return ret;
}