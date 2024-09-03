uniform sampler2D texture;
uniform vec2 resolution;
uniform float radius;
uniform float sigmaColor;
uniform float sigmaSpace;

void main() {
    vec2 texCoord = gl_TexCoord[0].xy;
    vec2 pixel = 1.0 / resolution;

    vec4 centerColor = texture2D(texture, texCoord);
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;

    for (float x = -radius; x <= radius; x++) {
        for (float y = -radius; y <= radius; y++) {
            vec2 offset = vec2(x * pixel.x, y * pixel.y);
            vec4 sampleColor = texture2D(texture, texCoord + offset);

            float spatialWeight = exp(-(x * x + y * y) / (2.0 * sigmaSpace * sigmaSpace));
            float colorWeight = exp(-distance(centerColor, sampleColor) / (2.0 * sigmaColor * sigmaColor));
            float weight = spatialWeight * colorWeight;

            color += sampleColor * weight;
            totalWeight += weight;
        }
    }

    gl_FragColor = color / totalWeight;
}