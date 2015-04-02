#version 330

uniform sampler2D texture;
uniform float thresholdValue;
uniform int animProgress;

in vec2 varyingTextureCoordinate;

out vec4 fragColor;

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

void main(void)
{
    fragColor = texture2D(texture, varyingTextureCoordinate);
    float progress = clamp(animProgress / 100.0, 0.0, 1.0);
    if (varyingTextureCoordinate[1] > (1.0 - progress)) {
        //fragColor = threshold(color, thresholdValue);
        fragColor = gray(fragColor);
        fragColor = invert(fragColor);
    }
}
