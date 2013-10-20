#version 150

in vec4 vPosition;
out vec4 color;

uniform vec4 vColor;

void
main()
{
    color = vColor;
	gl_Position = vPosition;
}