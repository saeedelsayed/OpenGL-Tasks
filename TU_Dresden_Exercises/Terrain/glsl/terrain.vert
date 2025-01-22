#version 330 core
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

in vec4 position;

out vec3 fragNormal;
out vec3 fragWorldPos; // Pass world position to the fragment shaderout
out vec3 fragTangent;
out vec3 fragBitangent;

uniform mat4 mvp;

//Returns the height of the procedural terrain at a given xz position
float getTerrainHeight(vec2 p);



void main()
{
	vec4 worldPosition = position;
	worldPosition.y = getTerrainHeight(position.xz); // update height 
	
	// Calculate normals using finite differences
    float delta = 1.0;
    vec2 dx = vec2(delta, 0);
    vec2 dz = vec2(0, delta);

    float heightL = getTerrainHeight(worldPosition.xz - dx); // Height to the left
    float heightR = getTerrainHeight(worldPosition.xz + dx); // Height to the right
    float heightD = getTerrainHeight(worldPosition.xz - dz); // Height below
    float heightU = getTerrainHeight(worldPosition.xz + dz); // Height above

    vec3 normal = normalize(vec3(heightL - heightR, 2.0, heightD - heightU));
    fragNormal = normal;

	fragTangent = normalize(vec3(1.0, (heightR - heightL)/delta, 0.0)); // Tangent in x direction
	fragBitangent = normalize(vec3(0.0, (heightU - heightD)/delta, 1.0)); // Bitangent in z direction

	fragWorldPos = worldPosition.xyz; // Pass world position

	gl_Position = mvp * worldPosition;
}

//source: https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 c)
{
	return 2 * fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453) - 1;
}

float perlinNoise(vec2 p )
{
	vec2 ij = floor(p);
	vec2 xy = p - ij;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(3.1415926 * xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

//based on https://www.seedofandromeda.com/blogs/58-procedural-heightmap-terrain-generation
float getTerrainHeight(vec2 p)
{
	float total = 0.0;
	float maxAmplitude = 0.0;
	float amplitude = 1.0;
	float frequency = 0.02;
	for (int i = 0; i < 11; i++) 
	{
		total +=  ((1.0 - abs(perlinNoise(p * frequency))) * 2.0 - 1.0) * amplitude;
		frequency *= 2.0;
		maxAmplitude += amplitude;
		amplitude *= 0.45;
	}
	return 15 * total / maxAmplitude;
}