/* Author: Daniel Rehberg, Henry Jochaniewicz
Date Modified: November 13, 2025
*/

#version 410

// The final color output
out vec4 color;

smooth in vec2 theTexCoord;
in vec3 normal;
smooth in vec4 thePosition;

// The texture containing our height data
uniform usampler2D tex;
uniform sampler2D heightTex;

vec3 dynamicTextureColor() {
    // Fetch the raw data from the texture
    uvec4 data = texture(tex, theTexCoord);
    
    // Normalize the height value (alpha channel) to a 0.0 to 1.0 range
    float height = float(data.a) / 255.0f;
    
    // Define the colors for our sand dune gradient
    vec3 lowColor = vec3(0.76, 0.65, 0.45);  // Dark Sand
    vec3 highColor = vec3(0.9, 0.8, 0.5); // Light Sandy Yellow
    
    // Blend between the two colors based on the height
    return mix(vec3(0.0), vec3(1.0), height);
} 

void main()
{
    vec3 lightColor = vec3(1.0);
    vec3 objectColor = vec3(0.9, 0.6, 0.2);

    vec3 lightSource = vec3(0.0f, -2.0f, 0.0f);

    vec3 eye = normalize(vec3(0.0) - thePosition.xyz);
    vec3 light = normalize(lightSource - thePosition.xyz);
    vec3 reflectDir = reflect(-light, normal);

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = lightColor * max(0.0, dot(normal, light));
    vec3 specular = lightColor * 0.5 * pow(max(dot(eye, reflectDir), 0.0), 8);

    vec3 dynamicColor = dynamicTextureColor();
    
    // Set the final output color with full opacity
    float map = texture(heightTex, theTexCoord).r;

    color = vec4(vec3(map), 1.0);
    color = vec4(min(lightColor, (specular + diffuse + ambient) * objectColor), 1.0);
}

