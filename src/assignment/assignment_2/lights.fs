#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform DirLight bgLight; // additional background directional light (e.g., top-right of the screen)
uniform Material material;
uniform sampler2D faceDiffuse;
uniform sampler2D faceSpecular;
uniform float faceThreshold; // dot(normal, viewDir) threshold to select face texture
uniform float useFace; // 0.0 = disable face texture, 1.0 = allow face texture selection
uniform float forceUseBody; // when 1.0, force using body textures only (disable face)
uniform vec3 glowColor;
uniform float glowStrength;
uniform float glowPower;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float faceMix);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float faceMix);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float faceMix);

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    // per-fragment selector: 1 => use face texture, 0 => use body texture
    // faceMix = 1.0 when this fragment should use the face texture.
    // forceUseBody overrides and forces the body textures when set to 1.0.
    float faceMix = step(faceThreshold, dot(norm, viewDir)) * useFace * (1.0 - forceUseBody);
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir, faceMix);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, faceMix);    
    // phase 3: spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir, faceMix);
    // additional background directional light (top-right-ish) to add a highlight
    result += CalcDirLight(bgLight, norm, viewDir, faceMix);

    // emissive / rim glow: stronger on edges where viewDir is nearly perpendicular to the surface normal
    float ndotv = max(dot(norm, viewDir), 0.0);
    float rim = pow(1.0 - ndotv, glowPower);
    vec3 emissive = glowColor * rim * glowStrength;
    result += emissive;
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float faceMix)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 bodyDiffuse = vec3(texture(material.diffuse, TexCoords));
    vec3 bodySpec = vec3(texture(material.specular, TexCoords));
    vec3 faceDiff = vec3(texture(faceDiffuse, TexCoords));
    vec3 faceSpec = vec3(texture(faceSpecular, TexCoords));
    vec3 diffTex = mix(bodyDiffuse, faceDiff, faceMix);
    vec3 specTex = mix(bodySpec, faceSpec, faceMix);
    vec3 ambient = light.ambient * diffTex;
    vec3 diffuse = light.diffuse * diff * diffTex;
    vec3 specular = light.specular * spec * specTex;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float faceMix)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 bodyDiffuse = vec3(texture(material.diffuse, TexCoords));
    vec3 bodySpec = vec3(texture(material.specular, TexCoords));
    vec3 faceDiff = vec3(texture(faceDiffuse, TexCoords));
    vec3 faceSpec = vec3(texture(faceSpecular, TexCoords));
    vec3 diffTex = mix(bodyDiffuse, faceDiff, faceMix);
    vec3 specTex = mix(bodySpec, faceSpec, faceMix);
    vec3 ambient = light.ambient * diffTex;
    vec3 diffuse = light.diffuse * diff * diffTex;
    vec3 specular = light.specular * spec * specTex;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float faceMix)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 bodyDiffuse = vec3(texture(material.diffuse, TexCoords));
    vec3 bodySpec = vec3(texture(material.specular, TexCoords));
    vec3 faceDiff = vec3(texture(faceDiffuse, TexCoords));
    vec3 faceSpec = vec3(texture(faceSpecular, TexCoords));
    vec3 diffTex = mix(bodyDiffuse, faceDiff, faceMix);
    vec3 specTex = mix(bodySpec, faceSpec, faceMix);
    vec3 ambient = light.ambient * diffTex;
    vec3 diffuse = light.diffuse * diff * diffTex;
    vec3 specular = light.specular * spec * specTex;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}