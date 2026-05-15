#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D strokeTexture;
uniform vec3 tintColor;

void main() {
    vec4 tex = texture(strokeTexture, TexCoord);
    if (tex.a < 0.1)
        discard;
    FragColor = vec4(tex.rgb * tintColor, tex.a);
}
