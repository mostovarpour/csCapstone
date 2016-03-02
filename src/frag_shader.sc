$input TexCoord

uniform sampler2D tex;
void main()
{
    vec4 tmpr = texture2D(tex, TexCoord);
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

