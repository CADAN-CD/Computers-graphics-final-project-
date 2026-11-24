#version 330 core
out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 TexCoords;

uniform sampler2D diffuse0;
uniform bool hasTexture;
uniform vec4 baseColor;

void main()
{
    vec4 tex = hasTexture ? texture(diffuse0, TexCoords) * baseColor : baseColor;
    if (tex.a < 0.5) discard;
    if (tex.r > 0.95 && tex.g > 0.95 && tex.b > 0.95) discard;

    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    float light = 0.4 + 0.6 * diff;

    FragColor = vec4(tex.rgb * light, 1.0);
}