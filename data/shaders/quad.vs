#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

out DATA
{
  vec2 pos;
  vec4 color;
} v_out;

uniform mat4 mvp;

void main()
{
  gl_Position = mvp * vec4(position, 1.0);
  v_out.color = color.wzyx;
  v_out.pos = position.xy;
}

