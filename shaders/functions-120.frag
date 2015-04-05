const int GaussianKernelRadius = -1 // WILL BE GENERATED
const vec4 GaussianKernel[1] = (vec4(-1.0)) // WILL BE GENERATED
const float pi = -1.0; // WILL BE GENERATED

const mat3 SobelMaskX = mat3(-1.0, 0.0, 1.0,
                             -2.0, 0.0, 2.0,
                             -1.0, 0.0, 1.0);
const mat3 SobelMaskY = mat3(-1.0, -2.0, -1.0,
                              0.0,  0.0,  0.0,
                              1.0,  2.0,  1.0);

float lightness(vec4 color)
{
    float cmax = max(color.r, max(color.g, color.b));
    float cmin = min(color.r, min(color.g, color.b));
    return (cmax + cmin) / 2;
}

vec4 gray(vec4 color)
{
    float l = lightness(color);
    return vec4(l, l, l, color.a);
}

vec4 threshold(vec4 color, float t)
{
    float l = lightness(color);
    float i = l < t ? 0.0 : 1.0;

    return vec4(i, i, i, color.a);
}

vec4 invert(vec4 color)
{
    return vec4(vec3(1.0) - color.rgb, color.a);
}

vec4 gaussBlur(sampler2D tex,
               vec2 textureSize,
               vec2 coords)
{
    int kernelSize = 2 * GaussianKernelRadius + 1;

    float dxtex = 1.0 / textureSize[0];
    float dytex = 1.0 / textureSize[1];

    vec4 color = vec4(0.0);
    vec4 sumCoef = vec4(0.0);

    for (int i = -GaussianKernelRadius; i <= GaussianKernelRadius; ++i) {
        for (int j = -GaussianKernelRadius; j <= GaussianKernelRadius; ++j) {
            vec4 pixel = texture2D(tex, coords + vec2(float(i) * dxtex, float(j) * dytex));
            vec4 coef = GaussianKernel[j + (i * kernelSize)];

            sumCoef += coef;
            color += pixel * coef;
        }

    }

    color /= sumCoef;
    return vec4(color.rgb, 1.0);
}

vec4[2] calcGradient(sampler2D tex,
                         vec2 textureSize,
                         vec2 coords,
                         bool blur)
{
    float dxtex = 1.0 / textureSize[0];
    float dytex = 1.0 / textureSize[1];

    vec4 dx = vec4(0.0); // X Directional Gradient
    vec4 dy = vec4(0.0); // Y Directional Gradient

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            vec2 offset = vec2(float(i) * dxtex, float(j) * dytex);
            vec4 pixel;
            if (blur)
                pixel = gaussBlur(tex, textureSize, coords + offset);
            else
                pixel = texture2D(tex, coords + vec2(float(i) * dxtex, float(j) * dytex));
            float gx = SobelMaskX[i+1][j+1];
            float gy = SobelMaskY[i+1][j+1];
            dx += pixel * vec4(gx);
            dy += pixel * vec4(gy);
        }
    }

    return vec4[2](dx, dy);
}

vec4 sobel(sampler2D tex,
           vec2 textureSize,
           vec2 coords,
           bool blur)
{
    vec4 gradient[2] = calcGradient(tex, textureSize, coords, blur);
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
    float dx = lightness(gradient[0]);
    float dy = lightness(gradient[1]);

    float x = abs(atan(dy, dx));
    if (x > pi)
        x -= pi;

    const float angleStep = 45.0 / 2.0;

    // North East and South West
    if (x >= radians(45.0 - angleStep) && x < radians(45.0 + angleStep))
        return vec2(1.0, -1.0);

    // North and South
    if (x >= radians(90.0 - angleStep) && x < radians(90.0 + angleStep))
        return vec2(0.0, 1.0);

    // North West and South East
    if (x >= radians(135.0 - angleStep) && x < radians(135.0 + angleStep))
        return (vec2(1.0, 1.0));

    // West and East
    return vec2(1.0, 0.0);
}

vec4 canny(sampler2D tex,
           vec2 textureSize,
           vec2 coords)
{
    vec4 gradient[2] = calcGradient(tex, textureSize, coords, true);
    float strength = gradientStrength(gradient);
    const float threshold = 0.2;

    if (strength < threshold)
        return vec4(0.0, 0.0, 0.0, 1.0);

    vec2 direction = gradientDirection(gradient);
    float dxtex = 1.0 / textureSize[0];
    float dytex = 1.0 / textureSize[1];

    vec2 offset = vec2(float(direction[0]) * dxtex, float(direction[1]) * dytex);

    vec4 forwardGradient[2] = calcGradient(tex, textureSize, coords + offset, true);
    vec4 backwardGradient[2] = calcGradient(tex, textureSize, coords - offset, true);

    float forwardStrength = gradientStrength(forwardGradient);
    float backwardStrength = gradientStrength(backwardGradient);

    // Non-max suppression
    if (forwardStrength > strength || backwardStrength > strength)
        return vec4(0.0, 0.0, 0.0, 1.0);

    return vec4(1.0);
}
