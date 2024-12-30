#version 450

layout (binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inNormal;
layout (location = 4) in vec3 inTangent;
layout (location = 5) in ivec4 inBoneIds;
layout (location = 6) in vec4 inWeights;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec2 fragTexCoord;
layout (location = 2) out vec3 fragNormal;
layout (location = 3) out vec3 fragTangent;
layout (location = 4) out ivec4 fragBoneIds;
layout (location = 5) out vec4 fragWeights;

void main() {
    // Transform vertex position
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    // Pass data to fragment shader
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal = normalize((ubo.model * vec4(inNormal, 0.0)).xyz); // Transform normal to world space
    fragTangent = normalize((ubo.model * vec4(inTangent, 0.0)).xyz); // Transform tangent to world space
    fragBoneIds = inBoneIds;
    fragWeights = inWeights;
}