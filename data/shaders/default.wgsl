struct VertexInput {
  @location(0) position: vec3f,
  @location(1) color: vec3f,
  @location(2) uv: vec2f,
}

struct VertexOutput {
  @builtin(position) position: vec4f,
  @location(0) color: vec3f,
  @location(1) uv: vec2f,
}

struct Uniforms {
  projection: mat4x4f,
  view: mat4x4f,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var gradientTexture: binding_array<texture_2d<f32>, 2>;
@group(0) @binding(2) var textureSampler: sampler;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var out: VertexOutput;
  out.position = uniforms.projection * uniforms.view * vec4f(in.position, 1.0);
  out.color = in.color;
  out.uv = in.uv;
  return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
  var idx = 0;

  if (in.uv.x < 0.5) {
    idx = 1;
  }

  let color = textureSample(gradientTexture[idx], textureSampler, in.uv);

  if (color.a < 0.0001) {
    discard;
  }

  return color;
}
