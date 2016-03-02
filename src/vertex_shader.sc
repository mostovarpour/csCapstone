$input texCoord, position
$output TexCoord

void main()
{
    TexCoord = texCoord;
    gl_Position = vec4(position, 0.0, 1.0);
}
