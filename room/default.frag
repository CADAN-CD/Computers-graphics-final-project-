#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 color;

void main()
{
	vec3 n = normalize(Normal);
	vec3 L = normalize(vec3(0.5, 1.0, 0.8));   // dirección de donde llega la luz
	float diffuse = max(dot(n, L), 0.0);
	FragColor = vec4(color * (0.25 + 0.75 * diffuse), 1.0);
}