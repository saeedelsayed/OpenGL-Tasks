#version 330 core
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved


in vec3 fragNormal; 
in vec3 fragWorldPos; // World position from vertex shader

out vec4 color;

uniform vec3 cameraPos;


uniform sampler2D background;
uniform sampler2D grassTexture; // Grass texture

uniform vec2 screenSize;

const vec3 dirToLight = normalize(vec3(1, 3, 1));	

//Calculates the visible surface color based on the Blinn-Phong illumination model
vec4 calculateLighting(vec4 materialColor, float specularIntensity, vec3 normalizedNormal, vec3 directionToViewer)
{
	vec4 color = materialColor;
	vec3 h = normalize(dirToLight + directionToViewer);
	color.xyz *= 0.9 * max(dot(normalizedNormal, dirToLight), 0) + 0.1;
	color.xyz += specularIntensity * pow(max(dot(h, normalizedNormal), 0), 50);
	return color;
}

vec4 getBackgroundColor()
{
	return texture(background, gl_FragCoord.xy / screenSize);
}

void main()
{
	//surface geometry
	vec3 n = fragNormal;
	vec3 dirToViewer = normalize(cameraPos - vec3(gl_FragCoord.x, 0.0, gl_FragCoord.y));

	// Sample grass texture using scaled xz world coordinates
    vec2 textureCoord = fragWorldPos.xz / 25.5; // Scale coordinates for texture tiling
    vec4 grassColor = texture(grassTexture, textureCoord);

	float specular = 0;

	

	//Calculate light
	color = calculateLighting(grassColor, specular, n, dirToViewer);

	
}