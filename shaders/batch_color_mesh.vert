#version 460
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outUV;

struct Vertex {
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 color;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer { 
    Vertex vertices[];
};

struct InstanceData {
    mat4 model;
};

layout(buffer_reference, std430) readonly buffer InstanceBuffer {
    InstanceData instances[];
};

layout(push_constant) uniform constants {
    mat4 viewProjection;
    VertexBuffer vertexBuffer;
    InstanceBuffer instanceBuffer;
} PushConstants;

void main() {
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
    InstanceData inst = PushConstants.instanceBuffer.instances[gl_InstanceIndex]; // <-- use instance index

    gl_Position = PushConstants.viewProjection * inst.model * vec4(v.position, 1.0);
    outColor = v.color.xyz;
    outUV = vec2(v.uv_x, v.uv_y);
}