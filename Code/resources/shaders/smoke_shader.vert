#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float age;
layout(location = 2) in float lifetime;
layout(location = 3) in vec4 color;
layout(location = 4) in float size;

uniform mat4 view;
uniform mat4 projection;

out float v_age;
out float v_lifetime;
out vec4 v_color;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0);
    gl_PointSize = size;
    v_age = age;
    v_lifetime = lifetime;
    v_color = color;
}