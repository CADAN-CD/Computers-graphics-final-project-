
#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 TexCoords;

// Material
uniform sampler2D diffuse0;
uniform bool hasTexture;
uniform vec4 baseColor;

// Point Light
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;

uniform float ambientStrength;
uniform float diffuseStrength;

// Attenuation
uniform float constant;
uniform float linear;
uniform float quadratic;

void main()
{
    //Get the color material
    vec4 tex = hasTexture
        ? texture(diffuse0, TexCoords) * baseColor
        : baseColor;

    if (tex.a < 0.5)
        discard;

    if (tex.r > 0.95 && tex.g > 0.95 && tex.b > 0.95)
        discard;

    // Normalize the surface normal
    vec3 norm = normalize(Normal);

    // Calculate the direction of the light -point light pos is the origin, and crntpos is the fragment that we are going to light
    
    vec3 lightDirection = normalize(pointLightPos - crntPos);

    // calculate the diffuse ilumination 
    float diff = max(dot(norm, lightDirection), 0.0);

    //calculate the distance for the attenuation, how much decreases the light intensity bc of the distance :p
    float distance = length(pointLightPos - crntPos);

    float attenuation = 1.0 / (
        constant +
        linear * distance +
        quadratic * distance * distance
    );

    //components of light
        //base light
    vec3 ambient = ambientStrength * pointLightColor;

        //depends on the position of the source 
    vec3 diffuse = diffuseStrength * diff * pointLightColor;

    // combine the ilumination 
    vec3 result = ambient + attenuation * diffuse;

    //apply it to the material :)
    FragColor = vec4(tex.rgb * result, 1.0);
}
