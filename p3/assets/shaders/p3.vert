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
uniform vec3 camPos;
uniform vec4 ambientLight = vec4(0.2, 0.2, 0.2, 1); 
uniform int flatMode;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec4 vertexColor;


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
  vec4 P = transform * position;
  vec3 L = normalize(lights[0].lightPosition - vec3(P));
  vec3 N = normalize(normalMatrix * normal);
  vec4 A = ambientLight * float(1 - flatMode); //Ia

  gl_Position = vpMatrix * P;
  //vertexColor = A + color * lights[0].lightColor * max(dot(N, L), float(flatMode));
  vec4 OaIa;
  vec4 OdIl;
  vec4 OsIl;

  vec3 Ll;
  vec3 Rl;
  vec4 Cl;  
  vec4 Il;  
  vec4 I = vec4(0);

  float dl;
  float phiL;
  float gammaL;

  OaIa = elementwiseMult(material.ambient, A);  
  I = OaIa;
  //tem que rodar nos bangs aqui e ir computando as paradas igual ta na descricao do trabalho
  for (int i = 0; i < numLights; i++)
  {
    switch(lights[i].type)
    {
      case 0: //directional
        Il = Cl;
        Ll = lights[i].direction;
        break;
        
      case 1: //point
        Ll = normalize(lights[i].lightPosition - vec3(P)); //aqui ja ta invertido o lL
        dl = distance(vec3(P), lights[i].lightPosition);
        Il = lights[i].lightColor/pow(dl,lights[i].fl);
        break;
        
      case 2: //spot
        gammaL = radians(float(lights[i].gammaL));                
        phiL = dot(lights[i].direction, Ll);
        Ll = normalize(lights[i].lightPosition - vec3(P)); //aqui ja ta invertido o lL
        Il = gammaL >= phiL ? lights[i].lightColor/pow(dl,lights[i].fl)*cos(phiL) : vec4(0);        
        break;
    }
    OdIl = elementwiseMult(material.diffuse, Il);
    OsIl = elementwiseMult(material.spot, Il);
    Rl = Ll - 2*(dot(N,Ll))*N;
    I += OdIl*(dot(N,Ll)) + OsIl *(dot(-Rl, camPos));

  }//end for

  vertexColor = I;
}//end main

