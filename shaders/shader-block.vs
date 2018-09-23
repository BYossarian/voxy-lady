
#version 330 core

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};
uniform mat4 model;
uniform mat3 normalMatrix;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexCoords;

out vec3 normal;
out vec3 fragmentPosition;
out vec3 textureCoords;

void main() {

    vec4 worldPosition = model * vec4(aPos, 1.0);

    normal = normalMatrix * aNormal;
    fragmentPosition = (worldPosition).xyz;
    textureCoords = aTexCoords;

    gl_Position = projection * view * worldPosition;
    
}
