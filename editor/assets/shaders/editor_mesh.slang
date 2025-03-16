struct AssembledVertex {
  float3 position : POSITION;
  float3 normal : NORMAL;
  float4 color : COLOR;
};

[[vk::binding(0, 0)]]
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
  float3 pos;
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

  output.sv_position = mul(float4(position, 1.0), view);
  output.coarseVertex.color = colour;
  output.coarseVertex.normal = mul(view * proj, float4(normal, 1.0)).xyz;
  output.coarseVertex.pos = output.sv_position.xyz;
  return output;
}

struct Fragment {
  float4 color;
};

[shader("fragment")]
Fragment fragmentMain(CoarseVertex coarseVertex: CoarseVertex) : SV_Target {
  const var lightColor = float4(1.0, 1.0, 1.0, 1.0);
  const var lightPos = float3(5.0, 5.0, 5.0);

  float3 norm = normalize(coarseVertex.normal);
  var lightDir = normalize(lightPos - coarseVertex.pos);

  float diff = max(dot(norm, lightDir), 0.0);
  var diffuse = diff * lightColor;

  float ambientStrength = 0.3;
  float4 ambient = ambientStrength * lightColor;

  float4 color = (ambient + diffuse) * coarseVertex.color;

  Fragment output;
  output.color = color;
  return output;
}
