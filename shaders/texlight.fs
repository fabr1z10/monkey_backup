R"(

#version 330 core

in vec2 tex;
in vec3 Normal;
//in vec4 eye;

uniform sampler2D texture_diffuse1;
uniform vec3 lightDir;
uniform vec4 diffuse;
uniform vec4 ambient;

out vec4 fragColor;

void main()
{
    vec3 n = normalize(Normal);
    //vec3 e = normalize(vec3(eye));
    vec4 texColor = texture (texture_diffuse1, tex);
	float intensity = max (dot(n, -lightDir), 0.0);

    vec3 d = intensity * texColor.rgb; //(intensity * diffuse + ambient) * texColor;
    // TODO replace with
    //vec3 d = (intensity + ambientStrength) * lightColor (which will be vec3) * texColor.rgb

    //d = texColor;
    fragColor = vec4(d, 1.);
}
)"
