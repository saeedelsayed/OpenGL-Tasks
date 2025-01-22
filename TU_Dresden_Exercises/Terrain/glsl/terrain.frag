#version 330 core
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved


in vec3 fragNormal;    // Normal from vertex shader
in vec3 fragWorldPos;  // World position from vertex shader
in vec3 fragTangent;   // Tangent from vertex shader
in vec3 fragBitangent; // Bitangent from vertex shader

out vec4 color;

uniform vec3 cameraPos;


uniform sampler2D background;
uniform sampler2D grassTexture; // Grass texture
uniform sampler2D rockTexture; // Rock texture
uniform sampler2D roadColorTexture; // Road color texture
uniform sampler2D alphaMap; // Alpha map
uniform sampler2D roadSpecularMap; // Specular map for road
uniform sampler2D roadNormalMap; // Normal map for road

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
	vec3 dirToViewer = normalize(cameraPos - fragWorldPos);

	// Sample grass texture using scaled xz world coordinates
    vec2 textureCoord = fragWorldPos.xz / 25.5; // Scale coordinates for grass and rock tiling
	vec2 alphaCoord = fragWorldPos.xz / 255.0; // Scale coordinates for alpha map

    vec4 grassColor = texture(grassTexture, textureCoord);
	vec4 rockColor = texture(rockTexture, textureCoord);

    // Calculate blend factor based on the x-component of the normal
    float blendFactor = clamp(0.5 + 0.5 * n.x, 0.0, 1.0); // Scale n.x from [-1, 1] to [0, 1]
	vec4 blendedColor = mix(grassColor, rockColor, blendFactor);

	// Sample alpha map and road texture
	float alpha = texture(alphaMap, alphaCoord).r; // Red channel holds alpha map value
	vec4 roadColor = texture(roadColorTexture, textureCoord);
	
	// Sample specular map for road
	float roadSpecular = texture(roadSpecularMap, textureCoord).r;

	// Sample normal map for road
	vec3 normalMapValue = texture(roadNormalMap, textureCoord).rgb * 2.0 - 1.0; // Transform [0,1] -> [-1,1]
	normalMapValue.y = -normalMapValue.y; // Invert Y-component for correct handedness

	// Transform normal from tangent space to world space
	mat3 TBN = mat3(normalize(fragTangent), normalize(fragBitangent), normalize(fragNormal));
	vec3 worldSpaceNormal = normalize(TBN * normalMapValue);	

	// Blend road texture with blended terrain using the alpha map
	vec4 finalColor = mix(blendedColor, roadColor, alpha);

	

	//Calculate light
	color = calculateLighting(finalColor, roadSpecular, worldSpaceNormal, dirToViewer);

	
}