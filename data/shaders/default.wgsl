struct VertexInput {
  @location(0) position: vec3f,
  @location(1) color: vec3f,
}

struct VertexOutput {
  @builtin(position) position: vec4f,
  @location(0) color: vec3f,
}

@group(0) @binding(0) var<uniform> u_time: f32;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var offset = 0.3 * vec2f(cos(u_time), sin(u_time));
  var pos = in.position.xy + offset;

  var out: VertexOutput;
  out.position = vec4f(pos, in.position.z, 1.0);
  out.color = in.color;
  return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(in.color, 1.0);
}
