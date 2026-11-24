#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D diffuse0;
uniform bool hasTexture;
uniform vec4 baseColor;
uniform vec3 pointLightColor;
void main()
{
    vec4 tex = hasTexture ? texture(diffuse0, TexCoords) * baseColor : baseColor;
    FragColor = vec4(tex.rgb * pointLightColor * 1.5, 1.0);
}