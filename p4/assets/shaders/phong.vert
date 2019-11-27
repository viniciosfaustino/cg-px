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
  vec3 lightPosition;
  vec4 lightColor;
  vec3 direction;

};

uniform Material material; //ok
uniform Light lights[10]; //TODO DENTRO DO P3
uniform int numLights; //ok mas tem que trocar o valor


uniform mat4 transform; //ok
uniform mat3 normalMatrix; //ok
uniform mat4 vpMatrix = mat4(1); //ok
uniform vec3 camPos; //ok
uniform vec4 ambientLight;  //ok
uniform int flatMode; //ok

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec3 N;
out vec4 P;


vec4 elementwiseMult(vec4 a, vec4 b)
{
  vec4 result;
  for(int i=0; i< 4; i++)
  {
    result[i] = a[i]*b[i];
  }
  return result;
}

void main()
{
  P = transform * position;
  vec3 L = normalize(lights[0].lightPosition - vec3(P));
  N = normalize(normalMatrix * normal);
  vec4 A = ambientLight * float(1 - flatMode); //Ia
  

  gl_Position = vpMatrix * P;  
}//end main

