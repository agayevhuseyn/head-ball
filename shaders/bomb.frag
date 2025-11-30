#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;
uniform float maxtime;

void main()
{
    vec4 tex = texture(texture0, fragTexCoord) * fragColor;

    float flash = (sin(time * time) * 0.5 + 0.5) * (time / maxtime);

    float brightness = 1.0 + flash * 0.8;

    vec3 tinted = mix(tex.rgb, vec3(1.0, 0.2, 0.2), flash);

    finalColor = vec4(tinted * brightness, tex.a);
}
