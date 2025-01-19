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

struct CoarseVertex {
  float4 color;
  float3 normal;
};

struct VertexStageOutput {
  CoarseVertex coarseVertex : CoarseVertex;
  float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexStageOutput vertexMain(AssembledVertex assembledVertex) {
  VertexStageOutput output;
  float3 position = assembledVertex.position;
  float3 normal = assembledVertex.normal;
  float4 colour = assembledVertex.color;

  output.sv_position = mul(proj * view * model, float4(position, 1.0));
  output.sv_position.w = 1.0;
  // output.coarseVertex.color = colour;
  // output.coarseVertex.normal = normal;
  return output;
}

struct Fragment {
  float4 color;
};

[shader("fragment")]
Fragment fragmentMain(CoarseVertex coarseVertex: CoarseVertex) : SV_Target {
  float4 color = coarseVertex.color;

  Fragment output;
  output.color = color;
  return output;
}
