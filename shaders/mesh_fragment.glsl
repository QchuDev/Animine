#version 330 core

out vec4 FragColor;

in vec3 FragNormal;
in vec2 TexCoord;

uniform vec3 tintColor;
uniform sampler2D meshTexture;
uniform int hasTexture;
uniform vec3 lightDir; // normalized direction TO the light

void main() {
    vec3 norm = normalize(FragNormal);
    vec3 ld = normalize(lightDir);
    float diff = max(dot(norm, ld), 0.0);
    float ambient = 0.3;
    float lighting = ambient + diff * 0.7;

    vec3 baseColor;
    if (hasTexture == 1) {
        baseColor = texture(meshTexture, TexCoord).rgb;
    } else {
        baseColor = tintColor;
    }

    FragColor = vec4(baseColor * lighting, 1.0);
}
