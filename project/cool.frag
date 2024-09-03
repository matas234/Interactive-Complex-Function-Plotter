uniform sampler2D texture;
uniform vec2 resolution;
uniform float radius;

void main() {
    vec2 texCoord = gl_TexCoord[0].xy;
    vec2 pixel = 1.0 / resolution;

    float kernel[9];
    int index = 0;

    for (float x = -1.0; x <= 1.0; x++) {
        for (float y = -1.0; y <= 1.0; y++) {
            kernel[index] = texture2D(texture, texCoord + vec2(x * pixel.x, y * pixel.y)).r;
            index++;
        }
    }

    // Simple bubble sort to find the median value
    for (int i = 0; i < 8; i++) {
        for (int j = i + 1; j < 9; j++) {
            if (kernel[i] > kernel[j]) {
                float temp = kernel[i];
                kernel[i] = kernel[j];
                kernel[j] = temp;
            }
        }
    }

    gl_FragColor = vec4(vec3(kernel[4]), 1.0);
}