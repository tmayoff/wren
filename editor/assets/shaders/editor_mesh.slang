struct AssembledVertex {
  float3 position : POSITION;
  float3 normal : NORMAL;
  float4 color : COLOR;
};

cbuffer GLOBALS {
  float4x4 view;
  float4x4 proj;
};

cbuffer LOCALS {
  float4x4 model;
};

struct VertexStageOutput {
  float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexStageOutput vertexMain(AssembledVertex assembledVertex) {
  VertexStageOutput output;
  float3 position = assembledVertex.position;

  output.sv_position = mul(proj * view * model, float4(position, 1.0));
  return output;
}

struct Fragment {
  float4 color;
};

[shader("fragment")]
Fragment fragmentMain() {
  Fragment output;
  output.color = float4(1.0);
  return output;
}
