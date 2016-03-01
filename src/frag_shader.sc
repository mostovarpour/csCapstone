$input TexCoord
$output outColor

#include "../common/common.sh"

SAMPLER2D(tex, 0);

void main()
{
    vec4 tmpr = texture2D(tex, TexCoord);
    outColor = vec4_splat(tmpr.r, tmpr.r, tmpr.r, 1.0f);
}
