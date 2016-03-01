$input texCoord, position
$output TexCoord

#include "../common/common.sh"

void main()
{
    TexCoord = texCoord;
    gl_Position = vec4_splat(position, 0.0, 1.0);
}
