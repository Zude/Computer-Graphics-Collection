#version 330 core

/**
 * Interpolierte Textur-Koordinate des Fragments.
 */
in vec2 fTexCoord;

in float fHeight;
in vec3 fNormal;
in vec3 fPosition;

in vec3 LightPos;
in vec3 LightColor;

uniform vec3 CamPosition;

uniform bool ShadingBool;
uniform bool TexBool;

/**
 * Farbe des Fragments, die auf den Framebuffer geschrieben wird.
 */
out vec4 FragColor;

/**
 * Texture-Sampler, um auf die Textur zuzugreifen. Die Einstellungen
 * des Samplers (Interpolation und Randbehandlung) werden beim Erstellen
 * der Textur gesetzt.
 */
uniform sampler2D UpperTex;

uniform sampler2D LowerTex;


vec3 PhongShading2(vec3 N){
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


vec3 combineTextures() {

    vec4 UpperColor;
    UpperColor = texture(UpperTex, fTexCoord);
    vec4 LowerColor;
    LowerColor = texture(LowerTex, fTexCoord);

    vec4 TexColor;
    vec3 resTex;

    if(fHeight > 0.6){
        TexColor = UpperColor;
    }

    else if(fHeight < 0.3){
         TexColor = LowerColor;
    }

    else {
        float s = smoothstep(0.3, 0.6, fHeight);
        TexColor = mix (LowerColor, UpperColor, s);         
    }

    resTex.x = TexColor.x;
    resTex.z = TexColor.z;
    resTex.y = TexColor.y;

    return resTex;
}


/**
 * Hauptprogramm des Fragment-Shaders.
 * Diese Funktion wird für jedes Fragment ausgeführt!
 */
void main(void)
{
    vec3 Tex;
    if(TexBool){
        Tex = combineTextures();
    } else {
        Tex = vec3(0.2);
    }

    if(ShadingBool){
        vec3 shading = PhongShading2(fNormal);

        FragColor = vec4(shading * Tex, 1.0);
    } else {
        FragColor = vec4(fNormal * Tex, 1.0);
    }
}


