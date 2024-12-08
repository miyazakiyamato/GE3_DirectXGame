#include "Collision.h"
#include <algorithm>

bool Collision::IsCollision(const Sphere& sphere1, const Sphere& sphere2) {
	Sphere s1 = sphere1;
	Sphere s2 = sphere2;

	float distance = Vector3::Length(s2.center - s1.center);
	return (distance <= (s1.radius + s2.radius) ? true : false);
}
bool Collision::IsCollision(const Sphere& sphere, const Plane& plane) {
	float k = Vector3::Dot(plane.normal, sphere.center) - plane.distance;
	k = k < 0 ? -k : k;
	if (k <= sphere.radius) {
		return true;
	}
	return false;
}
bool Collision::IsCollision(const Line& line, const Plane& plane) {
	float dot = Vector3::Dot(plane.normal, line.diff);
	if (dot == 0.0f) {
		return false;
	}
	float t = (plane.distance - Vector3::Dot(line.origin, plane.normal)) / dot;
	if (t >= -1.0f && t <= 2.0f) {
		return true;
	}
	return false;
}
bool Collision::IsCollision(const Ray& ray, const Plane& plane) {
	float dot = Vector3::Dot(plane.normal, ray.diff);
	if (dot == 0.0f) {
		return false;
	}
	float t = (plane.distance - Vector3::Dot(ray.origin, plane.normal)) / dot;
	if (t >= 0.0f && t <= 2.0f) {
		return true;
	}
	return false;
}
bool Collision::IsCollision(const Segment& segment, const Plane& plane) {
	float dot = Vector3::Dot(plane.normal, segment.diff);
	if (dot == 0.0f) {
		return false;
	}
	float t = (plane.distance - Vector3::Dot(segment.origin, plane.normal)) / dot;
	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}
	return false;
}
bool Collision::IsCollision(const Triangle& triangle, const Segment& segment) {
	Triangle t1 = triangle;
	Segment s1 = segment;
	// ベクトルv1,v2を求める
	Vector3 v01 = t1.Vertices[1] - t1.Vertices[0];
	Vector3 v12 = t1.Vertices[2] - t1.Vertices[1];
	Plane plane;
	// 法線nを算出
	plane.normal = Vector3::Normalize(Vector3::Cross(v01, v12));
	// 距離を求める
	plane.distance = Vector3::Dot(t1.Vertices[0], plane.normal);

	if (IsCollision(s1, plane)) {
		float dot = Vector3::Dot(plane.normal, s1.diff);
		float t = (plane.distance - Vector3::Dot(s1.origin, plane.normal)) / dot;
		Vector3 segmentP = s1.origin + (s1.diff * t);

		Vector3 cross01 = Vector3::Cross(t1.Vertices[1] - t1.Vertices[0], segmentP - t1.Vertices[1]);
		Vector3 cross12 = Vector3::Cross(t1.Vertices[2] - t1.Vertices[1], segmentP - t1.Vertices[2]);
		Vector3 cross20 = Vector3::Cross(t1.Vertices[0] - t1.Vertices[2], segmentP - t1.Vertices[0]);

		if (Vector3::Dot(cross01, plane.normal) >= 0.0f && Vector3::Dot(cross12, plane.normal) >= 0.0f && Vector3::Dot(cross20, plane.normal) >= 0.0f) {
			return true;
		}
	}
	return false;
}
bool Collision::IsCollision(const AABB& aabb, const Vector3 point)
{
	// 点が AABB の範囲内にあるかをチェック
	return (point.x >= aabb.min.x && point.x <= aabb.max.x &&
		point.y >= aabb.min.y && point.y <= aabb.max.y &&
		point.z >= aabb.min.z && point.z <= aabb.max.z);
}
bool Collision::IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x && aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y && aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z) {
		return true;
	}
	return false;
}
bool Collision::IsCollision(const AABB& aabb, const Sphere& sphere) {
	Vector3 closestPoint = Vector3::Clamp(sphere.center, aabb.min, aabb.max);
	float distance = Vector3::Length(closestPoint - sphere.center);
	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}
bool Collision::IsCollision(const AABB& aabb, const Segment& segment) {
	Vector3 tMin = {(aabb.min.x - segment.origin.x) / segment.diff.x, (aabb.min.y - segment.origin.y) / segment.diff.y, (aabb.min.z - segment.origin.z) / segment.diff.z};
	Vector3 tMax = {(aabb.max.x - segment.origin.x) / segment.diff.x, (aabb.max.y - segment.origin.y) / segment.diff.y, (aabb.max.z - segment.origin.z) / segment.diff.z};
	Vector3 tNear = Vector3::Min(tMin, tMax);
	Vector3 tFar = Vector3::Max(tMin, tMax);
	float tmin = fmaxf(fmaxf(tNear.x, tNear.y), tNear.z);
	float tmax = fminf(fminf(tFar.x, tFar.y), tFar.z);
	if (tmin <= tmax) {
		if ((tmin >= 0 && tmin <= 1) || (tmax >= 0 && tmax <= 1) || tmin < 0 && tmax > 1) {
			return true;
		}
	}
	return false;
}
bool Collision::IsCollision(const AABB& aabb, const Ray& ray) {
	Vector3 tMin = {(aabb.min.x - ray.origin.x) / ray.diff.x, (aabb.min.y - ray.origin.y) / ray.diff.y, (aabb.min.z - ray.origin.z) / ray.diff.z};
	Vector3 tMax = {(aabb.max.x - ray.origin.x) / ray.diff.x, (aabb.max.y - ray.origin.y) / ray.diff.y, (aabb.max.z - ray.origin.z) / ray.diff.z};
	Vector3 tNear = Vector3::Min(tMin, tMax);
	Vector3 tFar = Vector3::Max(tMin, tMax);
	float tmin = fmaxf(fmaxf(tNear.x, tNear.y), tNear.z);
	float tmax = fminf(fminf(tFar.x, tFar.y), tFar.z);
	if (tmin <= tmax) {
		if ((tmin >= 0) || (tmax >= 0)) {
			return true;
		}
	}
	return false;
}
bool Collision::IsCollision(const AABB& aabb, const Line& line) {
	Vector3 tMin = {(aabb.min.x - line.origin.x) / line.diff.x, (aabb.min.y - line.origin.y) / line.diff.y, (aabb.min.z - line.origin.z) / line.diff.z};
	Vector3 tMax = {(aabb.max.x - line.origin.x) / line.diff.x, (aabb.max.y - line.origin.y) / line.diff.y, (aabb.max.z - line.origin.z) / line.diff.z};
	Vector3 tNear = Vector3::Min(tMin, tMax);
	Vector3 tFar = Vector3::Max(tMin, tMax);
	float tmin = fmaxf(fmaxf(tNear.x, tNear.y), tNear.z);
	float tmax = fminf(fminf(tFar.x, tFar.y), tFar.z);
	if (tmin <= tmax) {
		return true;
	}
	return false;
}
bool Collision::IsCollision(const OBB& obb, const Sphere& sphere) {
	OBB obb1 = obb;
	Sphere sphere1 = sphere;

	Matrix4x4 obbWorldMatrix{obb1.orientations[0].x, obb1.orientations[1].x, obb1.orientations[2].x, 0, obb1.orientations[0].y, obb1.orientations[1].y, obb1.orientations[2].y, 0,
	                         obb1.orientations[0].z, obb1.orientations[1].z, obb1.orientations[2].z, 0, obb1.center.x,          obb1.center.y,          obb1.center.z,          1};
	Vector3 centerInObbLocalSpace = Matrix4x4::Transform(sphere1.center, Matrix4x4::Inverse(obbWorldMatrix));
	AABB aabbOBBLocal{.min = -obb1.size, .max = obb1.size};
	Sphere sphereOBBLocal{centerInObbLocalSpace, sphere1.radius};
	if (IsCollision(aabbOBBLocal, sphereOBBLocal)) {
		return true;
	}
	return false;
}
bool Collision::IsCollision(const Segment& segment, const OBB& obb) {
	Segment segment1 = segment;
	OBB obb1 = obb;

	Matrix4x4 obbWorldMatrix{obb1.orientations[0].x, obb1.orientations[1].x, obb1.orientations[2].x, 0, obb1.orientations[0].y, obb1.orientations[1].y, obb1.orientations[2].y, 0,
	                         obb1.orientations[0].z, obb1.orientations[1].z, obb1.orientations[2].z, 0, obb1.center.x,          obb1.center.y,          obb1.center.z,          1};
	Vector3 localOrigin = Matrix4x4::Transform(segment1.origin, Matrix4x4::Inverse(obbWorldMatrix));
	Vector3 localEnd = Matrix4x4::Transform(segment1.origin + segment1.diff, Matrix4x4::Inverse(obbWorldMatrix));

	AABB localAABB{.min = -obb1.size, .max = obb1.size};

	Segment localSegment;

	localSegment.origin = localOrigin;
	localSegment.diff = localEnd - localOrigin;

	if (IsCollision(localAABB, localSegment)) {
		return true;
	}
	return false;
}
bool Collision::IsCollision(const Line& line, const OBB& obb) {
	Line line1 = line;
	OBB obb1 = obb;

	Matrix4x4 obbWorldMatrix{obb1.orientations[0].x, obb1.orientations[1].x, obb1.orientations[2].x, 0, obb1.orientations[0].y, obb1.orientations[1].y, obb1.orientations[2].y, 0,
	                         obb1.orientations[0].z, obb1.orientations[1].z, obb1.orientations[2].z, 0, obb1.center.x,          obb1.center.y,          obb1.center.z,          1};
	Vector3 localOrigin = Matrix4x4::Transform(line1.origin, Matrix4x4::Inverse(obbWorldMatrix));
	Vector3 localEnd = Matrix4x4::Transform(line1.origin + line1.diff, Matrix4x4::Inverse(obbWorldMatrix));

	AABB localAABB{.min = -obb1.size, .max = obb1.size};

	Line localLine;

	localLine.origin = localOrigin;
	localLine.diff = localEnd - localOrigin;

	return IsCollision(localAABB, localLine);
}
