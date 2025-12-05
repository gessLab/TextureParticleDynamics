/* Author: Daniel Rehberg, Henry Jochaniewicz
Date Modified: November 20, 2025
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
uniform float heightTexRes;
uniform float dynTexRes;
// uniform mat3 normalMat;

#define VOODOO_NUMBER 600.0

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

vec3 calculateNormal(vec2 uv) {
    float difference = 1.0 / heightTexRes;
    float above = texture(heightTex, theTexCoord + vec2(0.0, difference)).r ; // + texture(tex, theTexCoord + vec2(0.0, 1.0 / dynTexRes)).r;
    float below = texture(heightTex, theTexCoord + vec2(0.0, -difference)).r; // + texture(tex, theTexCoord + vec2(0.0, -1.0 / dynTexRes)).r;
    float left  = texture(heightTex, theTexCoord + vec2(-difference, 0.0)).r; // + texture(tex, theTexCoord + vec2(-1.0 / dynTexRes, 0.0)).r;
    float right = texture(heightTex, theTexCoord + vec2(difference, 0.0)).r ; // + texture(tex, theTexCoord + vec2(1.0 / dynTexRes, 0.0)).r;
    return normalize(vec3(below - above, 1.0 / 420.0, left - right));
} 

vec3 dynamicTextureColor() {
    // Fetch the raw data from the texture
    uvec4 data = texture(tex, theTexCoord);

    // Normalize the height value (alpha channel) to a 0.0 to 1.0 range
    float height = float(data.a) / 255.0f;

    // Define the colors for our sand dune gradient
    vec3 lowColor = vec3(0.6, 0.5, 0.3);  // Dark Sand
    vec3 highColor = vec3(1.0, 0.7, 0.5); // Light Sandy Yellow

    // Blend between the two colors based on the height
    return mix(lowColor, highColor, height);
    // return mix(vec3(0.0), vec3(1.0), height);
}

void main()
{
    // vec3 normal = /* normalMat * */calculateNormal(theTexCoord);
    // vec3 normal = calculateNormal(theTexCoord);
    vec3 lightColor = vec3(1.0);

    // vec3 lightSource = vec3(0.0f, 1.0f, -1.5f);
    vec3 lightSource = vec3(0.0);

    /*
    color = vec4(vec3(1.0f / pow(distance(lightSource, thePosition.xyz), 4.0) * 2.0f), 1.0);
    return;
    vec3 color1 = vec3(step(0.99, normal.y));
    color = vec4(color1, 1.0);
    return;
    */

    // light directions
    vec3 eye = normalize(vec3(0.0) - thePosition.xyz);
    vec3 light = normalize(lightSource - thePosition.xyz);
    vec3 reflectDir = reflect(-light, normal);

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = max(0.0, dot(normal, light)) * lightColor;

    /*
    float specularPower = 4.0 + random(theTexCoord) * 4.0;

    // a little sparkle randomization. Currently no time element,
    // so it's kind of hard to see.
    float sparkle = smoothstep(0.95, 1.0, random(theTexCoord));
    vec3 sparkleColor = vec3(1.0, 0.9, 0.7);
    */

    vec3 specular = 0.125 * pow(max(dot(eye, reflectDir), 0.0), 12.0) * lightColor;

    // vec3 objectColor = dynamicTextureColor();
    vec3 objectColor = vec3(1.0, 0.7, 0.5); 

    vec3 lightStrength = (specular + diffuse + ambient); // / (1.0 * length(light));

    vec3 outputColor = min(lightColor, lightStrength * objectColor);
    color = vec4(outputColor, 1.0);
}

