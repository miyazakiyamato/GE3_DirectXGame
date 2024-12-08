#pragma once
#include<iostream>
#include "Vector3.h"
#include "Matrix4x4.h"

struct Line {
	Vector3 origin;
	Vector3 diff;
};
struct Ray {
	Vector3 origin;
	Vector3 diff;
};
struct Segment {
	Vector3 origin;
	Vector3 diff;
};
struct Triangle {
	Vector3 Vertices[3];
};
struct Sphere {
	Vector3 center;
	float radius;
};
struct Plane {
	Vector3 normal;
	float distance;
};
struct AABB {
	Vector3 min;
	Vector3 max;
};
struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};
class Collision {
public:
	static bool IsCollision(const Sphere& s1, const Sphere& s2);
	static bool IsCollision(const Sphere& sphere, const Plane& plane);
	static bool IsCollision(const Line& line, const Plane& plane);
	static bool IsCollision(const Ray& ray, const Plane& plane);
	static bool IsCollision(const Segment& segment, const Plane& plane);
	static bool IsCollision(const Triangle& triangle, const Segment& segment);
	static bool IsCollision(const AABB& aabb, const Vector3 point);
	static bool IsCollision(const AABB& aabb1, const AABB& aabb2);
	static bool IsCollision(const AABB& aabb, const Sphere& sphere);
	static bool IsCollision(const AABB& aabb, const Segment& segment);
	static bool IsCollision(const AABB& aabb, const Ray& ray);
	static bool IsCollision(const AABB& aabb, const Line& line);
	static bool IsCollision(const OBB& obb, const Sphere& sphere);
	static bool IsCollision(const Segment& segment, const OBB& obb);
	static bool IsCollision(const Line& line, const OBB& obb);
};
