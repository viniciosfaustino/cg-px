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
  
  vec4 OaIa;
  vec4 OdIl;
  vec4 OsIl;
  vec3 V = vec3(P) - camPos;

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
        Il = lights[i].lightColor;
        Ll = -normalize(lights[i].direction);
        break;
        
      case 1: //point
        Ll = normalize(lights[i].lightPosition - vec3(P)); //aqui ja ta invertido o lL
        dl = distance(vec3(P), lights[i].lightPosition);
        Il = lights[i].lightColor/pow(dl,lights[i].fl);
        break;
        
      case 2: //spot
        gammaL = radians(float(lights[i].gammaL));
        Ll = normalize(lights[i].lightPosition - vec3(P)); //aqui ja ta invertido o lL
        phiL = abs(acos(dot(lights[i].direction,-Ll)));
        dl = distance(vec3(P), lights[i].lightPosition);
        Il = gammaL < phiL ? vec4(0) : lights[i].lightColor/pow(dl,lights[i].fl)*pow(cos(phiL), lights[i].decayExponent);
        break;
    }

    OdIl = elementwiseMult(material.diffuse, Il);
    OsIl = elementwiseMult(material.spot, Il);
    Rl = reflect(Ll, N);
    I += OdIl*max(dot(N,Ll),flatMode) + OsIl * pow(min(max(dot(Rl,V), 0),1 - float(flatMode)), material.shine);

  }//end for

  gl_Position = vpMatrix * P;
  vertexColor = I;
}//end main

