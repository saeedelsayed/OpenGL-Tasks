#version 330 core
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

in vec2 fragTexCoord;

in vec3 fragPosition;
in vec3 fragNormal;  

out vec4 color;

uniform vec3 cameraPos;
uniform sampler2D background;
uniform sampler2D grassTexture; // Added texture uniform
uniform vec2 screenSize;

const vec3 dirToLight = normalize(vec3(1, 3, 1));	

// Calculates the visible surface color based on the Blinn-Phong illumination model
vec4 calculateLighting(vec4 materialColor, float specularIntensity, vec3 normalizedNormal, vec3 directionToViewer)
{
	vec4 color = materialColor;
	vec3 h = normalize(dirToLight + directionToViewer);
	color.xyz *= 0.9 * max(dot(normalizedNormal, dirToLight), 0) + 0.1;
	color.xyz += specularIntensity * pow(max(dot(h, normalizedNormal), 0), 50);
	return color;
}

// Retrieves background color
vec4 getBackgroundColor()
{
	return texture(background, gl_FragCoord.xy / screenSize);
}

void main()
{
	// Compute texture coordinates using world-space xz
	vec2 texCoords = fragPosition.xz / 25.5;
	vec3 grassColor = texture(grassTexture, texCoords).rgb;
	
	vec3 n = normalize(fragNormal);
	// Surface geometry
	vec3 dirToViewer = normalize(cameraPos - fragPosition);  

	// Material properties	
	vec4 materialColor = vec4(texCoords, 0.0, 1.0); // Use texture color
	float specular = 0.1;  // Adjust for shininess

	// Apply lighting with the texture
	color = vec4(grassColor, 1.0);
}
