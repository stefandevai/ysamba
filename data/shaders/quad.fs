#version 330 core

in DATA
{
  vec4 color;
  vec2 pos;
} f_in;

out vec4 color;

void main()
{
  color = f_in.color;
}

