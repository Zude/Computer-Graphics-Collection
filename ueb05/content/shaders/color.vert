#version 330 core

/** 
 * Position des Vertex. 
 * Wird von der Host-Anwendung über den Attribut-Pointer 0 
 * bereitgestellt. 
 */
layout (location = 0) in vec4 vPosition;
/**
 * Textur-Koordinate des Vertex. 
 * Wird von der Host-Anwendung über den Attribut-Pointer 1
 * bereitgestellt. 
 */
layout (location = 1) in vec2 vTexCoord;

/** 
 * Ausgabe des Vertex-Shaders. 
 * Die Textur-Koordinate wird interpoliert vom Fragment-Shader 
 * entgegengenommen.
 */
out vec2 fTexCoord;

out float fHeight;

out vec3 fNormal;

out vec3 fPosition;

/** 
 * Projektions-Matrix.
 * Wird dem Shader mit einem Aufruf von glUniformMatrix4fv übergeben.
 */
uniform mat4 Projection;
/**
 * View-Matrix. 
 * Wird dem Shader mit einem Aufruf von glUniformMatrix4fv übergeben.
 */
uniform mat4 ModelView;

uniform sampler2D HeightMap;

uniform float Time;

uniform bool Sine;

uniform float StepSize;

uniform float TexStepSize;
uniform bool ShadingBool;
uniform bool HeightBool;

uniform vec3 CamPosition;

out vec3 LightPos;
out vec3 LightColor;



#define TEX_RANGE_A 0.0f
#define TEX_RANGE_B 1.0f
#define COORD_RANGE_A -1.0f
#define COORD_RANGE_B 1.0f


float mapRange(float a1, float a2, float b1, float b2,
                  float s) {
  return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

float coordToTex (float coord) {
  return mapRange(COORD_RANGE_A, COORD_RANGE_B, TEX_RANGE_A, TEX_RANGE_B,
                  coord);
}

vec3 GouraudShading(vec3 N){
 int shininess = 32;
 vec3 lightDir = normalize(LightPos.xyz - fPosition);
 vec3 viewDir = normalize(CamPosition-fPosition);
 vec3 reflectDir = normalize(-reflect(lightDir, N));
 float ambientStrength = 0.1f;

 vec3 ambient = ambientStrength * LightColor;

 float diff = max(dot(N,lightDir), 0.0);
 vec3 diffuse = diff * LightColor;

 float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);
vec3 specular = spec * LightColor;

 vec3 result = ambient + diffuse + specular;

 return result;
}

float getHeight(vec3 pos){
       
    float sineMult;
    float heightMult;

    vec2 tex;
    tex.x = coordToTex(pos.x);
    tex.y = coordToTex(pos.z);

    sineMult = sin((pos.x + pos.z + Time) * 5) / 4;
    heightMult = texture(HeightMap, tex).r;

    float res = 0.0;
    
    if(Sine){
    res += sineMult;
    } 
    if(HeightBool) {
    res += heightMult;
    } 

    return res;
}

vec3 calculateNormal(vec3 p1) {
	float delta = StepSize;

	vec3 p2 = p1;
	vec3 p3 = p1;

  p2.x += delta;
  p3.z += delta;

  if(p2.x > 1.0f){
    p2.x = p1.x - delta;
  }

  if(p3.x > 1.0f){
    p3.x = p1.x - delta;
  }

	p2.y = getHeight(p2);
	p3.y = getHeight(p3);
  
	vec3 u = p2 - p1;
	vec3 v = p3 - p1;

	vec3 normal = vec3(0.0f);
  normal = cross(u,v);
  normal = normal ;
	return normalize(normal*(-1));
}

/**
 * Hauptprogramm des Vertex-Shaders.
 * Diese Funktion wird für jeden Vertex ausgeführt!
 */
void main(void)
{
    vec4 vertPos = vPosition;

    vertPos.y = getHeight(vPosition.xyz);

    gl_Position = Projection * ModelView * vertPos;


    //Daten für den Frag-Shader
    fPosition = gl_Position.xyz;    
    fHeight = vertPos.y;
    fTexCoord = vTexCoord;
    
    LightColor = vec3(1.0);
    LightPos = vec3(1.2f, 1.0f, 2.0f);

    if(!ShadingBool){
      fNormal = GouraudShading(calculateNormal(vertPos.xyz));
    } else {
      fNormal = calculateNormal(vertPos.xyz);
    }

    

    ;

}
