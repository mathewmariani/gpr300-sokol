#version 300 es

precision mediump float;

out vec4 frag_color;

uniform sampler2D depth;
uniform vec2 screen_size;

void main()
{
    vec2 uv = gl_FragCoord.xy / screen_size;  
    float sceneDepth = texture(depth, uv).r * 2.0 - 1.0;

    if (gl_FragCoord.z > sceneDepth) {
        discard;
    }

    frag_color = vec4(1.0, 1.0, 1.0, 1.0);
}