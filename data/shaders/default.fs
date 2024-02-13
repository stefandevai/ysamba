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

    vec4 texture_color;

    switch(tid)
    {
      case 0:
        texture_color = texture(textures[0], f_in.uv);
	break;
      case 1:
        texture_color = texture(textures[1], f_in.uv);
	break;
      case 2:
        texture_color = texture(textures[2], f_in.uv);
	break;
      case 3:
        texture_color = texture(textures[3], f_in.uv);
	break;
      case 4:
        texture_color = texture(textures[4], f_in.uv);
	break;
      case 5:
        texture_color = texture(textures[5], f_in.uv);
	break;
      case 6:
        texture_color = texture(textures[6], f_in.uv);
	break;
      case 7:
        texture_color = texture(textures[7], f_in.uv);
	break;
      default:
        texture_color = texture(textures[0], f_in.uv);
	break;
    }
    final_color = texture_color * f_in.color;
  }

  if (final_color.a < 0.0001)
  {
    discard;
  }

  color = final_color;
}
