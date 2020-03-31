#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 camera;

out vec3 ourColor;
out vec3 Normal;
out vec3 FragPos;

void main() {
    gl_Position = projection * camera * model * vec4(aPos, 1.0);
    ourColor = aColor;

    // Source: https://learnopengl.com/Lighting/Basic-Lighting
    FragPos = vec3(model * vec4(aPos, 1.0));
    vec4 newNorm = inverse(transpose(model)) * vec4(aNormal, 1.0);
    Normal = vec3(newNorm.x, newNorm.y, newNorm.z);
}
