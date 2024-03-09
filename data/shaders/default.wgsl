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
@group(0) @binding(1) var texture_sampler: sampler;

@group(1) @binding(0) var textures: binding_array<texture_2d<f32>>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var out: VertexOutput;
  out.position = uniforms.projection * uniforms.view * vec4f(in.position, 1.0);
  out.color = in.color.abgr;
  out.uv = in.uv;
  out.texture_id = in.texture_id;
  return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
  var color = vec4f(1.0);
  let idx = i32(in.texture_id);

  switch(idx)
  {
    case 0: {
      color = textureSample(textures[0], texture_sampler, in.uv) * in.color;
    }
    case 1: {
      color = textureSample(textures[1], texture_sampler, in.uv) * in.color;
    }
    case 2: {
      color = textureSample(textures[2], texture_sampler, in.uv) * in.color;
    }
    case 3: {
      color = textureSample(textures[3], texture_sampler, in.uv) * in.color;
    }
    case 4: {
      color = textureSample(textures[4], texture_sampler, in.uv) * in.color;
    }
    case 5: {
      color = textureSample(textures[5], texture_sampler, in.uv) * in.color;
    }
    case 6: {
      color = textureSample(textures[6], texture_sampler, in.uv) * in.color;
    }
    case 7: {
      color = textureSample(textures[7], texture_sampler, in.uv) * in.color;
    }
    default {
      color = in.color;
    }
  }

  if (color.a < 0.1) {
    discard;
  }

  return color;
}
