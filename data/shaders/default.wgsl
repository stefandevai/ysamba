struct VertexInput {
  @location(0) position: vec3f,
  @location(1) uv: vec2f,
  @location(2) texture_id: f32,
  @location(3) color: vec4f,
}

struct VertexOutput {
  @builtin(position) position: vec4f,
  @location(0) uv: vec2f,
  @location(1) texture_id: f32,
  @location(2) color: vec4f,
}

struct Uniforms {
  projection: mat4x4f,
  view: mat4x4f,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var textureSampler: sampler;

@group(1) @binding(0) var gradientTexture: binding_array<texture_2d<f32>>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var out: VertexOutput;
  out.position = uniforms.projection * uniforms.view * vec4f(in.position, 1.0);
  out.color = in.color;
  out.uv = in.uv;
  out.texture_id = in.texture_id;
  return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
  var color = vec4f(1.0);

  if (in.texture_id > -0.5) {
    var idx = u32(in.texture_id);
    color = textureSample(gradientTexture[idx], textureSampler, in.uv);
  }

  if (color.a < 0.0001) {
    discard;
  }

  return color * in.color;
}
