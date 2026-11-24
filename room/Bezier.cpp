#include "Bezier.h"

BezierPath::BezierPath(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
	: P0(p0), P1(p1), P2(p2), P3(p3) {
}

glm::vec3 BezierPath::evaluate(float t) const
{
	//u is the complement
	float u = 1.0f - t;
	return u * u * u * P0+ 3.0f * u * u * t * P1+ 3.0f * u * t * t * P2+ t * t * t * P3;
}

glm::vec3 BezierPath::getPosition() const
{
	return evaluate(t);
}

//Directionb of the curve -derivate  
glm::vec3 BezierPath::getTangent() const
{
	float u = 1.0f - t;
	glm::vec3 d = 3.0f * u * u * (P1 - P0)
		+ 6.0f * u * t * (P2 - P1)
		+ 3.0f * t * t * (P3 - P2);
	return glm::normalize(d);
}

void BezierPath::update(float deltaTime)
{
	if (!animate) return;

	float step = speed * deltaTime;

	if (pingPong) {
		t += forward ? step : -step;
		if (t >= 1.0f) { t = 1.0f; forward = false; }
		if (t <= 0.0f) { t = 0.0f; forward = true; }
	}
	else {
		t += step;
		if (t > 1.0f) t -= 1.0f;
	}
}

std::vector<glm::vec3> BezierPath::sample(int segments) const
{
	std::vector<glm::vec3> points;
	points.reserve(segments + 1);
	for (int i = 0; i <= segments; i++)
		points.push_back(evaluate((float)i / segments));
	return points;
}