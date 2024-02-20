struct VertexInput {
  @location(0) position: vec3f,
  @location(1) color: vec3f,
}

struct VertexOutput {
  @builtin(position) position: vec4f,
  @location(0) color: vec3f,
}

struct Uniforms {
  color: vec4f,
  time: f32,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var offset = 0.3 * vec2f(cos(uniforms.time), sin(uniforms.time));
  var pos = in.position.xy + offset;

  var out: VertexOutput;
  out.position = vec4f(pos, in.position.z, 1.0);
  out.color = in.color;
  return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(in.color, 1.0) * uniforms.color;
}
