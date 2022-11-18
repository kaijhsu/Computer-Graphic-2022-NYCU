#version 430

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D ourTexture;

uniform vec3 viewPos;
uniform vec3 cameraDir;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirectionLight
{
    int enable;
    vec3 direction;
    vec3 lightColor;
};

struct PointLight {
    int enable;
    vec3 position;  
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;
};

struct Spotlight {
    int enable;
    vec3 position;
    vec3 direction;
    vec3 lightColor;
    float cutOff;

    // Paramters for attenuation formula
    float constant;
    float linear;
    float quadratic;      
}; 

uniform Material material;
uniform DirectionLight dl;
uniform PointLight pl;
uniform Spotlight sl;
uniform int scaleEnable;

vec3 ambient(vec3 la){
    return material.ambient * la;
}

vec3 diffuse(vec3 ld, vec3 l){
    return material.diffuse * ld * max(dot(Normal, l), 0.0);
}

vec3 specular(vec3 ls, vec3 l, vec3 v){
    vec3 h = normalize(l + v);
    return material.specular * ls * pow(max(dot(Normal, h), 0.0), material.shininess);
}

float attenuation(vec3 lightPos, float a, float b, float c){
    float d = distance(lightPos, FragPos);
    return 1/(a + b*d + c*d*d);
}

float inCutOff(vec3 l){
    float value = dot(l, normalize(-sl.direction));
    if(value > sl.cutOff){
        return 1.0;
    }
    return 0.0;
}


void main() {
    vec3 v = normalize(viewPos - FragPos);
    vec3 spotLightDir = normalize(sl.position - FragPos);
    vec3 spot      = attenuation(sl.position, sl.constant, sl.linear, sl.quadratic) * (
                        ambient(sl.lightColor) + 
                        inCutOff(spotLightDir) * (
                            diffuse(sl.lightColor, spotLightDir) + 
                            specular(sl.lightColor, spotLightDir, v)
                        )
                     );

    vec3 pointLightDir = normalize(pl.position - FragPos);
    vec3 point     = attenuation(pl.position, pl.constant, pl.linear, pl.quadratic) * (
                        ambient(pl.lightColor) + 
                        diffuse(pl.lightColor, pointLightDir) +
                        specular(pl.lightColor, pointLightDir, v)
                     );
    vec3 direction = ambient(dl.lightColor) + 
                     diffuse(dl.lightColor, -dl.direction) + 
                     specular(dl.lightColor, -dl.direction, v); 
    vec3 lightColor = direction * dl.enable + point * pl.enable + spot * sl.enable;
    color = vec4(lightColor, 1.0);
    if(scaleEnable == 1){
        
        float edge = dot(normalize(Normal), normalize(-cameraDir));
        if(edge <= 0.05){
            color *= 0.5;
        }
        int x = int(color.x * 255) / 8; x *= 8;
        int y = int(color.y * 255) / 8; y *= 8;
        int z = int(color.z * 255) / 8; z *= 8;
        color = vec4(float(x)/255, float(y)/255, float(z)/255, 1.0);
    }
    color *= texture(ourTexture, TexCoord);

}
