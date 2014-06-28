#version 150

uniform mat4 model;
uniform sampler2D tex;

uniform struct Light {
	vec3 position;
	vec3 intensities;
} light;

in vec3 fragVert;
in vec2 fragTexCoord;
in vec3 fragNormal;

out vec4 finalColor;

void main() {
	vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);
	vec3 surfacePos = vec3(model * vec4(fragVert, 1));
	vec4 surfaceColor = texture(tex, fragTexCoord);
	vec3 surfaceToLight = normalize(light.position - surfacePos);

	float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
	vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.intensities;

    finalColor = vec4(diffuse, 1.0);
}