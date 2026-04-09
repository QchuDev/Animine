uniform bool useTexture; // Lo activas desde C++
uniform vec3 flatColor;
uniform sampler2D tex;
in vec2 TexCoord;

void main() {
    if (useTexture) {
        FragColor = texture(tex, TexCoord);
    } else {
        FragColor = vec4(flatColor, 1.0);
    }
}