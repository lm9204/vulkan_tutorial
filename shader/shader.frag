#version 450

layout (location = 0) in vec4 fragColor;
layout (location = 1) in vec2 fragTexCoord;
layout (location = 2) in vec3 fragNormal;
layout (location = 3) in vec3 fragTangent;
layout (location = 4) flat in ivec4 fragBoneIds;
layout (location = 5) in vec4 fragWeights;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2D texSampler;

void main()
{
    // Normalize the interpolated normal
    vec3 normal = normalize(fragNormal);

    // Sample the texture using fragTexCoord
    vec3 texColor = texture(texSampler, fragTexCoord).rgb;

    // Simple diffuse lighting (example)
    vec3 lightDir = normalize(vec3(0.5, 0.5, -1.0)); // Directional light
    float diffuse = max(dot(normal, lightDir), 0.0);

    // Combine texture color, diffuse lighting, and vertex color
    vec3 finalColor = fragColor.rgb * texColor * diffuse;

    // Output the final color
    outColor = vec4(finalColor, fragColor.a);
}