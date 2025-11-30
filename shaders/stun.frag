#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    vec4 tex = texture(texture0, fragTexCoord) * fragColor;

    vec3 tint = vec3(0.2, 0.2, 0.5);

    float amt = 0.35;

    tex.rgb = mix(tex.rgb, tint, amt);

    finalColor = tex;
}
