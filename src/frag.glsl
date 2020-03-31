// Source: https://learnopengl.com/Lighting/Basic-Lighting
// TODO:
// See if using given functions is ok
// or if it should be done manually

#version 330 core

uniform vec3 light;
uniform vec3 viewPos;

in vec3 ourColor;
in vec3 Normal;
in vec3 FragPos;

out vec4 fragColor;

void main() {
    vec3 lightDir = normalize(light - FragPos);
    vec3 norm = normalize(Normal);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
    // Light settings
    float ambientStrength = 0.3;
    float spectralStrength = 0.5;
    float shininess = 32.0;

    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * lightColor;

    // Spectral lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 spectral = spectralStrength * spec * lightColor;

    vec3 finalColor = (ambient + diffuse + spectral) * ourColor;
    fragColor = vec4(finalColor, 1.0f);
}
