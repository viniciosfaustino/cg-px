#version 330 core
struct Material
{
  vec4 ambient; //Oa da equacao
  vec4 diffuse;
  vec4 spot;
  float shine;
};

struct Light
{
  int type;
  int fl;
  float gammaL;
  int decayExponent;
  int openingAngle;
  vec3 lightPosition;
  vec4 lightColor;
  vec3 direction;

};

uniform Material material;
uniform Light lights[10];
uniform int numLights;




uniform mat4 transform;
uniform mat3 normalMatrix;
uniform mat4 vpMatrix = mat4(1);
uniform vec4 color;
uniform vec4 ambientLight = vec4(0.2, 0.2, 0.2, 1); 
uniform vec3 lightPosition;
uniform vec4 lightColor = vec4(1);
uniform int flatMode;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec4 vertexColor;

struct pinto
{
 int fodase;
};

void main()
{
  vec4 P = transform * position;
  vec3 L = normalize(lightPosition - vec3(P));
  vec3 N = normalize(normalMatrix * normal);
  vec4 A = ambientLight * float(1 - flatMode); //Ia

  gl_Position = vpMatrix * P;
  vertexColor = A + color * lightColor * max(dot(N, L), float(flatMode));
  vec4 OaIa;
  for(int i = 0; i < 4; i++)
  {
    OaIa[i] = material.ambient[i]*A[i];
  }

  vec4 OdId;
}
