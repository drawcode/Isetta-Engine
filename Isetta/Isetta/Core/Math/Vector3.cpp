/*
 * Copyright (c) 2018 Isetta
 */
#include "Core/Math/Vector3.h"

#include <cfloat>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Vector2.h"
#include "Vector3Int.h"

namespace Isetta::Math {

const Vector3 Vector3::zero = Vector3();
const Vector3 Vector3::one = Vector3(1.f);
const Vector3 Vector3::up = Vector3(0.f, 1.f, 0.f);
const Vector3 Vector3::right = Vector3(1.f, 0.f, 0.f);
const Vector3 Vector3::forward = Vector3(0.f, 0.f, 1.f);
const Vector3 Vector3::down = Vector3(0.f, -1.f, 0.f);
const Vector3 Vector3::left = Vector3(-1.f, 0.f, 0.f);
const Vector3 Vector3::back = Vector3(0.f, 0.f, -1.f);

Vector3::Vector3(const Vector3Int& inIntVector)
    : x{static_cast<float>(inIntVector.x)},
      y{static_cast<float>(inIntVector.y)},
      z{static_cast<float>(inIntVector.z)} {}

Vector3::Vector3(const Vector2& inVector, float inZ)
    : x{inVector.x}, y{inVector.y}, z{inZ} {}

float Vector3::operator[](int i) const {
  switch (i) {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    default:
      throw std::out_of_range{"Vector3::[] => Index access out of range."};
  }
}

float Vector3::Magnitude() const { return sqrtf(SqrMagnitude()); }
float Vector3::SqrMagnitude() const { return x * x + y * y + z * z; }
Vector3 Vector3::Normalized() const {
  float length{Magnitude()};
  return Vector3(x / length, y / length, z / length);
}
void Vector3::Normalize() noexcept {
  float length{Magnitude()};
  x /= length;
  y /= length;
  z /= length;
}

std::string Vector3::ToString() const {
  std::ostringstream oss;
  oss << "(" << x << ", " << y << ", " << z << ")";
  return oss.str();
}

bool Vector3::Equals(const Vector3& lhs, const Vector3& rhs) {
  return abs(lhs.x - rhs.x) < FLT_EPSILON && abs(lhs.y - rhs.y) < FLT_EPSILON &&
         abs(lhs.z - rhs.z) < FLT_EPSILON;
}
float Vector3::Dot(const Vector3& lhs, const Vector3& rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
Vector3 Vector3::Cross(const Vector3& lhs, const Vector3& rhs) {
  return Vector3(lhs.y * rhs.z - lhs.z * rhs.y, -lhs.x * rhs.z + lhs.z * rhs.x,
                 lhs.x * rhs.y - lhs.y * rhs.x);
}
Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float time) {
  return start * (1.f - time) + end * time;
}
float Vector3::Distance(const Vector3& start, const Vector3& end) {
  return (start - end).Magnitude();
}
Vector3 Vector3::Project(const Vector3& inVector, const Vector3& onNormal) {
  return onNormal.Normalized() * Dot(inVector, onNormal);
}
Vector3 Vector3::Reflect(const Vector3& inVector, const Vector3& inNormal) {
  Vector3 normal{inNormal.Normalized()};
  return inVector - normal * Dot(inVector, normal) * 2.f;
}
Vector3 Vector3::Scale(const Vector3& inVector, const Vector3& scalar) {
  return Vector3(inVector.x * scalar.x, inVector.y * scalar.y,
                 inVector.z * scalar.z);
}
Vector3 Vector3::Slerp(const Vector3& start, const Vector3& end, float time) {
  float dot = Dot(start, end);
  dot = dot < -1.f ? -1.f : (dot > 1.f ? 1.f : dot);
  float theta = acosf(dot) * time;
  Vector3 relativeVector = end - start * dot;
  return start * cosf(theta) + relativeVector * sinf(theta);
}
Vector3 Vector3::FromString(const std::string& str) {
  Vector3 vec;
  char c;
  std::istringstream is(str);
  (is >> std::skipws) >> c >> vec.x >> c >> vec.y >> c >> vec.z;

  return vec;
}
}  // namespace Isetta::Math
