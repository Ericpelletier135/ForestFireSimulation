#version 330 core
out vec4 FragColor;
uniform vec3 voxelColor;

void main()
{
    FragColor = vec4(voxelColor, 1.0);
}