
#version 330 core

struct Material {
    sampler2DArray diffuseTexture;

    int shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 colour;

    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
};

uniform Material material;
uniform DirectionalLight light;
uniform vec3 viewPosition;

in vec3 normal;
in vec3 fragmentPosition;
in vec3 textureCoords;

out vec4 colour;

void main() {

    vec3 diffuseColour = vec3(texture(material.diffuseTexture, textureCoords));

    // ambient
    vec3 ambientLight = light.ambientIntensity * light.colour * diffuseColour;

    // diffuse
    vec3 norm = normalize(normal);
    // light.direction is the direction that the light is shining towards, but we want 
    // lightDir to be the vector pointing towards the light, hence the -ve
    vec3 lightDir = normalize(-light.direction);
    float diffuseCoefficient = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = light.diffuseIntensity * light.colour * diffuseCoefficient * diffuseColour;

    // specular using Blinn-Phong
    vec3 viewDir = normalize(viewPosition - fragmentPosition);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specularCoefficient = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specularLight = light.specularIntensity * light.colour * specularCoefficient * diffuseColour;

    vec3 result = ambientLight + diffuseLight + specularLight;
    colour = vec4(result, 1.0);

}
