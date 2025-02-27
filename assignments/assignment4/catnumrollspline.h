#pragma once

// C/C++
#include <vector>

// glm
#include "glm/vec3.hpp"

static std::vector<glm::vec3> CatnumrollSpline(std::vector<glm::vec3> points, int subdivisions) {

	// equation
	auto q = [](float t, float p0, float p1, float p2, float p3) -> float {
		return (0.5f * ((2 * p1) + (p2 - p0) * t +
			(2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
			(3 * p1 - p0 - 3 * p2 + p3) * t * t * t));
	};

	std::vector<glm::vec3> subdividedPoints;
	auto increments = (1.0f / (float)subdivisions);

	for (auto i = 0; i < (points.size() - 1); i++) {
		const auto p0 = (i == 0) ? points[i] : points[i-1];
		const auto p1 = points[i];
		const auto p2 = points[i+1];
		const auto p3 = ((i + 2) == points.size()) ? points[i+1] : points[i+2];

		for (auto j = 0; j <= subdivisions; j++) {
			auto x = q(j * increments, p0.x, p1.x, p2.x, p3.x);
			auto y = q(j * increments, p0.y, p1.y, p2.y, p3.y);
			auto z = 0.0f;

			subdividedPoints.push_back({ x, y, z });
		}
	}

	return subdividedPoints;
}
