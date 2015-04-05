const mat3 GX = mat3(-1.0, 0.0, 1.0,
                     -2.0, 0.0, 2.0,
                     -1.0, 0.0, 1.0);
const mat3 GY = mat3(-1.0, -2.0, -1.0,
                      0.0,  0.0,  0.0,
                      1.0,  2.0,  1.0);

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

vec4 gaussBlur(sampler2D tex,
               vec2 textureSize,
               vec2 coords,
               vec4 gaussKernel[81],
               int kernelRadius)
{
    int kernelSize = 2 * kernelRadius + 1;

    float dxtex = 1.0 / textureSize[0];
    float dytex = 1.0 / textureSize[1];

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 sumCoef = vec4(0.0, 0.0, 0.0, 0.0);

    for (int i = -kernelRadius; i <= kernelRadius; ++i) {
        for (int j = -kernelRadius; j <= kernelRadius; ++j) {
            vec4 pixel = texture2D(tex, coords + vec2(float(i) * dxtex, float(j) * dytex));
            vec4 coef = gaussKernel[j + (i * kernelSize)];

            sumCoef += coef;
            color += pixel * coef;
        }

    }

    color /= sumCoef;
    return vec4(color.r, color.g, color.b, 1.0);
}

vec4[2] filteredGradient(sampler2D tex,
                         vec2 textureSize,
                         vec2 coords,
                         vec4 gaussKernel[81],
                         int kernelRadius)
{
    float dxtex = 1.0 / textureSize[0];
    float dytex = 1.0 / textureSize[1];

    vec4 dx = vec4(0.0); // X Directional Gradient
    vec4 dy = vec4(0.0); // Y Directional Gradient

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            vec2 offset = vec2(float(i) * dxtex, float(j) * dytex);
            vec4 filteredPixel = gaussBlur(tex, textureSize, coords + offset, gaussKernel, kernelRadius);
            float gx = GX[i+1][j+1];
            float gy = GY[i+1][j+1];
            dx += filteredPixel * vec4(gx);
            dy += filteredPixel * vec4(gy);
        }
    }

    vec4 gradient[2];
    gradient[0] = dx;
    gradient[1] = dy;
    return gradient;
}

vec4 sobel(sampler2D tex,
           vec2 textureSize,
           vec2 coords,
           vec4 gaussKernel[81],
           int kernelRadius)
{
    vec4 gradient[2] = filteredGradient(tex, textureSize, coords, gaussKernel, kernelRadius);
    vec4 dx = gradient[0];
    vec4 dy = gradient[1];

    vec4 result;
    for(int i = 0; i < 4; ++i) {
        result[i] = length(vec2(dx[i], dy[i]));
    }

    return vec4(result.rgb, 1.0);
}

float gradientStrength(vec4 gradient[2])
{
   float dx = lightness(gradient[0]);
   float dy = lightness(gradient[1]);

   return length(vec2(dx, dy));
}

vec2 gradientDirection(vec4 gradient[2]) {
    const float atan0   =  0.414213;
    const float atan45  =  2.414213;
    const float atan90  = -2.414213;
    const float atan135 = -0.414213;

    float dx = lightness(gradient[0]);
    float dy = lightness(gradient[1]);

    float x = atan(dy, dx);

    if (x < atan0 && x > atan135)
        return vec2(1.0, 0.0);

    if (x < atan90 && x > atan45)
        return vec2(0.0, 1.0);

    if (x > atan135 && x < atan90)
        return vec2(-1.0, 1.0);

    return vec2(1.0, 1.0);
}

vec4 canny(sampler2D tex,
           vec2 textureSize,
           vec2 coords,
           vec4 gaussKernel[81],
           int kernelRadius)
{
    vec4 gradient[2] = filteredGradient(tex, textureSize, coords, gaussKernel, kernelRadius);
    float strength = gradientStrength(gradient);
    const float threshold = 0.2;

    if (strength < threshold)
        return vec4(0.0, 0.0, 0.0, 1.0);

    vec2 direction = gradientDirection(gradient);
    float dxtex = 1.0 / textureSize[0];
    float dytex = 1.0 / textureSize[1];

    vec2 offset = vec2(float(direction[0]) * dxtex, float(direction[1]) * dytex);

    vec4 forwardGradient[2] = filteredGradient(tex, textureSize, coords + offset, gaussKernel, kernelRadius);
    vec4 backwardGradient[2] = filteredGradient(tex, textureSize, coords - offset, gaussKernel, kernelRadius);

    float forwardStrength = gradientStrength(forwardGradient);
    float backwardStrength = gradientStrength(backwardGradient);

    if (forwardStrength > strength || backwardStrength > strength)
        return vec4(0.0, 0.0, 0.0, 1.0);

    return vec4(1.0);
}
