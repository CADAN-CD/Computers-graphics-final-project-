#pragma once
#include <glm/glm.hpp>
#include <vector>

class BezierPath
{
public:
	// Control points
	//public for imGui
	glm::vec3 P0, P1, P2, P3;
	//animate is for automatic movement
	bool  animate = false;
	//
	bool  pingPong = true;  
	//speed 
	float speed = 0.15f;    
	float t = 0.0f;

	BezierPath(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

	// Goes forward if t moves on
	void update(float deltaTime);

	// Position of the curve for the actual point
	glm::vec3 getPosition() const;

	// Posición for t [0-1]
	glm::vec3 evaluate(float t) const;

	// Direction of the movement
	glm::vec3 getTangent() const;

	// points to draw the curve
	std::vector<glm::vec3> sample(int segments = 50) const;

private:
	bool forward = true;
};