float lightness(vec4 color)
{
    float cmax = max(color[0], max(color[1], color[2]));
    float cmin = min(color[0], min(color[1], color[2]));
    return (cmax + cmin) / 2;
}

vec4 gray(vec4 color)
{
    float l = lightness(color);
    return vec4(l, l, l, 1.0);
}

vec4 threshold(vec4 color, float t)
{
    float l = lightness(color);
    float i = l < t ? 0.0 : 1.0;

    return vec4(i, i, i, 1.0);
}

vec4 invert(vec4 color)
{
    return vec4(1.0 - color[0], 1.0 - color[1], 1.0 - color[2], color[3]);
}

vec4 gaussBlur(sampler2D tex, vec2 textureSize, vec2 coords, vec4 gaussKernel[81], int kernelRadius)
{
    int kernelSize = 2 * kernelRadius + 1;

    float dxtex = 1.0 / textureSize[0];
    float dytex = 1.0 / textureSize[1];

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

    for (int i = -kernelRadius; i <= kernelRadius; ++i) {
        for (int j = -kernelRadius; j <= kernelRadius; ++j) {
            vec4 pixel = texture2D(tex, coords + vec2(float(i) * dxtex, float(j) * dytex));
            vec4 gauss = gaussKernel[j + (i * kernelSize)];
            color += pixel * gauss;
        }

    }

    return vec4(color.r, color.g, color.b, 1.0);
}
