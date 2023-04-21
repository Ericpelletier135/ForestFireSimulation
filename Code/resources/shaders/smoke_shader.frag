#version 330 core

in float age;
in float lifetime;
in vec4 color;
in float size;

out vec4 FragColor;

void main() {
    // Calculate a simple alpha based on the particle's age
    float alpha = clamp((1.0 - age / lifetime), 0.0, 1.0);
    FragColor = vec4(color.rgb, color.a * alpha);
}
