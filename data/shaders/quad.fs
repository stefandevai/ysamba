#version 330 core

in DATA
{
  vec2 pos;
  vec4 color;
} f_in;

out vec4 color;

void main()
{
  color = vec4(f_in.color.rgb, f_in.color.a);
}

