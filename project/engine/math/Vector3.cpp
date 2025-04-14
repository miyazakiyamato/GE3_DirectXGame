#include "Vector3.h"
#include <cmath>
#include <algorithm>

float Vector3::Length(){
    return Length(*this);
}

Vector3 Vector3::Normalize(){
    return Normalize(*this);
}

Vector3 Vector3::Abs(){
    return Abs(*this);
}

Vector3 Vector3::Clamp01(){
    return Clamp(*this, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
}

Vector3 Vector3::Clamp_11(){
    return Clamp(*this, { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });
}

Vector3 Vector3::Add(const Vector3& v1, const Vector3& v2) {
    Vector3 v3;
    v3.x = v1.x + v2.x;
    v3.y = v1.y + v2.y;
    v3.z = v1.z + v2.z;
    return v3;
}

Vector3 Vector3::Subtract(const Vector3& v1, const Vector3& v2) {
    Vector3 v3;
    v3.x = v1.x - v2.x;
    v3.y = v1.y - v2.y;
    v3.z = v1.z - v2.z;
    return v3;
}

Vector3 Vector3::Multiply(float scalar, const Vector3& v) {
    Vector3 v2;
    v2.x = v.x * scalar;
    v2.y = v.y * scalar;
    v2.z = v.z * scalar;
    return v2;
}
Vector3 Vector3::Divide(float scalar, const Vector3& v){
    Vector3 v2;
    v2.x = v.x / scalar;
    v2.y = v.y / scalar;
    v2.z = v.z / scalar;
    return v2;
}
float Vector3::Dot(const Vector3& v1, const Vector3& v2){
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vector3::Length(const Vector3& v){
    return std::sqrt(Dot(v, v));
}

Vector3 Vector3::Normalize(const Vector3& v){
    return (Vector3)v / Length(v);
}

Vector3 Vector3::Abs(const Vector3& v){
    return { std::fabs(v.x), std::fabs(v.y), std::fabs(v.z) };
}

Vector3 Vector3::Min(const Vector3& v, const Vector3& v2){
    return { std::min(v.x, v2.x), std::min(v.y, v2.y), std::min(v.z, v2.z) };
}

Vector3 Vector3::Max(const Vector3& v, const Vector3& v2){
    return { std::max(v.x, v2.x), std::max(v.y, v2.y), std::max(v.z, v2.z) };
}

Vector3 Vector3::Clamp(const Vector3& v, const Vector3& min, const Vector3& max){
    return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y), std::clamp(v.z, min.z, max.z) };
}

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2){
    return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

Vector3 Vector3::Project(const Vector3& v1, const Vector3& v2){
    Vector3 norm_v2 = Normalize(v2);
    return norm_v2 * Dot(v1, norm_v2);
}
Vector3 Vector3::Perpendicular(const Vector3& vector) {
    if (vector.x != 0.0f || vector.y != 0.0f) {
        return { -vector.y,vector.x,0.0f };
    }
    return { 0.0f,-vector.z,vector.y };
}

Vector3 Vector3::Reflect(const Vector3& input, const Vector3& normal) {
    return Vector3(input) - Vector3(normal) * (2.0f * Vector3::Dot(input, normal));
}

//Vector3 Vector3::ClosestPoint(const Vector3& point, const Line& line)
//{
//    return line.origin + Project(point - line.origin, line.diff);
//}
//
//Vector3 Vector3::ClosestPoint(const Vector3& point, const Ray& ray)
//{
//    Vector3 v3 = ray.origin + Project(point - ray.origin, ray.diff);
//    return Max(v3, ray.origin);
//}
//
//Vector3 Vector3::ClosestPoint(const Vector3& point, const Segment& segment)
//{
//    Vector3 v3 = segment.origin + Project(point - segment.origin, segment.diff);
//    return Clamp(v3, segment.origin, segment.origin + segment.diff);
//}

Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, float t){
    return (Vector3)v1 + ((Vector3)v2 - v1) * t;
}

Vector3 Vector3::CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const float& t){
    const float s = 0.5f; // 1/2
    float t2 = t * t;  // t の2乗
    float t3 = t2 * t; // t の3乗

    Vector3 e3 = (Vector3)p0 * -1.0f + (Vector3)p1 * 3.0f - (Vector3)p2 * 3.0f + p3;
    Vector3 e2 = (Vector3)p0 * 2.0f - (Vector3)p1 * 5.0f + (Vector3)p2 * 4.0f - p3;
    Vector3 e1 = (Vector3)p0 * -1.0f + p2;
    Vector3 e0 = (Vector3)p1 * 2.0f;

    return (e3 * t3 + e2 * t2 + e1 * t + e0) * s;
}

Vector3 Vector3::Random(std::mt19937& randomEngine,const Vector3& vMin, const Vector3& vMax){
    std::uniform_real_distribution<float> distX(vMin.x, vMax.x);
    std::uniform_real_distribution<float> distY(vMin.y, vMax.y);
    std::uniform_real_distribution<float> distZ(vMin.z, vMax.z);
    return { distX(randomEngine), distY(randomEngine), distZ(randomEngine) };
}

Vector3 Vector3::operator-(){
    return { -x, -y, -z };
}

Vector3 Vector3::operator+(const Vector3& v2){
    return { x + v2.x, y + v2.y, z + v2.z };
}

Vector3 Vector3::operator-(const Vector3& v2){
    return { x - v2.x, y - v2.y, z - v2.z };
}

Vector3 Vector3::operator*(const float& v2){
    return { x * v2, y * v2, z * v2 };
}

Vector3 Vector3::operator/(const float& v2){
    return { x / v2, y / v2, z / v2 };
}

Vector3& Vector3::operator+=(const Vector3& v2){
    x += v2.x;
    y += v2.y;
    z += v2.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& v2){
    x -= v2.x;
    y -= v2.y;
    z -= v2.z;
    return *this;
}

Vector3& Vector3::operator*=(const float& v2)
{
    x *= v2;
    y *= v2;
    z *= v2;
    return *this;
}

Vector3& Vector3::operator/=(const float& v2){
    x /= v2;
    y /= v2;
    z /= v2;
    return *this;
}