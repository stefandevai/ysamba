#version 330 core

in DATA
{
  vec2 uv;
  float tid;
  vec4 color;
  vec2 pos;
} f_in;

out vec4 color;

uniform sampler2D textures[11];

void main()
{
  vec4 final_color = f_in.color;

  if (f_in.tid > -0.05)
  {
    int tid = int(f_in.tid + 0.5);
    vec4 texture_color = texture(textures[tid], f_in.uv);
    final_color = texture_color * f_in.color;
  }

  /* if (final_color.a < 0.0001) */
  /* { */
  /*   discard; */
  /* } */

  color = final_color;
}
