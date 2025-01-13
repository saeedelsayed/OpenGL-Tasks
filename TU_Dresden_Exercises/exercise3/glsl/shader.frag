#version 330 core
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

uniform vec2 c;
uniform float m;
uniform bool bloomEnabled;
uniform sampler2D screenTexture;
uniform vec2 screenSize;

in vec4 fragment_color;
in vec4 fragment_position;

out vec4 color;

void main(void)
{
    // Julia fractal calculation
    vec2 z = fragment_position.xy * m;
    int i_max = 200;
    int i;

    for (i = 1; i <= i_max; i++) {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;
        if (x * x + y * y > 4.0) {
            break;
        }
        z.x = x;
        z.y = y;
    }

    float alpha = 0.0;
    if (i < i_max) {
        alpha = float(i) / float(i_max);
    }

    vec4 baseColor = vec4(alpha, alpha, alpha, alpha) * 10.0 * fragment_color;

    // Bloom effect
    if (bloomEnabled) {
        vec3 bloomColor = vec3(0.0);
        float threshold = 1.0; // Adjust threshold for glow sensitivity
        int blurRadius = 2;    // Radius for the bloom effect
        vec2 texelSize = 1.0 / screenSize;

        for (int x = -blurRadius; x <= blurRadius; x++) {
            for (int y = -blurRadius; y <= blurRadius; y++) {
                vec2 offset = vec2(x, y) * texelSize;
                vec3 sampleColor = texture(screenTexture, gl_FragCoord.xy / screenSize + offset).rgb;

                if (dot(sampleColor, vec3(0.333)) > threshold) {
                    bloomColor += sampleColor;
                }
            }
        }

        baseColor.rgb += bloomColor * 0.25; // Adjust bloom intensity
    }

    color = baseColor;
}
