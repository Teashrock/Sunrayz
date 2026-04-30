#include "base.h"
#include "error.h"

#ifdef __HAIKU__
static char* SDFShader = "#version 140\n\n\
varying vec2 fragTexCoord;\
varying vec4 fragColor;\
uniform sampler2D texture0;\
uniform vec4 colDiffuse;\
const float smoothing = 1.0/16.0;\n\n\
void main()\
{\
    // Texel color fetching from texture sampler\
    // NOTE: Calculate alpha using signed distance field (SDF)\
    float distance = texture2D(texture0, fragTexCoord).a;\n\n\
    if (distance > 0.0)\
        float alpha = 1.0;\
    else\
        float alpha = 0.0;\
    gl_FragColor = vec4(fragColor.rgb, alpha);\
}\
\0";
#else
static char* SDFShader = "#version 330\n\n\
in vec2 fragTexCoord;\
in vec4 fragColor;\
uniform sampler2D texture0;\
uniform vec4 colDiffuse;\
out vec4 finalColor;\n\n\
void main()\
{\
    // Texel color fetching from texture sampler\
    // NOTE: Calculate alpha using signed distance field (SDF)\
    float distanceFromOutline = texture(texture0, fragTexCoord).a - 0.5;\
    float distanceChangePerFragment = length(vec2(dFdx(distanceFromOutline), dFdy(distanceFromOutline)));\
    float alpha = smoothstep(-distanceChangePerFragment, distanceChangePerFragment, distanceFromOutline);\n\n\
    finalColor = vec4(fragColor.rgb, fragColor.a*alpha);\
}\
\0";
#endif

SzConstruct* CreateConstruct(SzConstruct* parent)
{
    SzConstruct* c = (SzConstruct*)MemAlloc(sizeof(SzConstruct));

    *c = (SzConstruct){
        .parent = parent,
        .child = NULL,
        .parts = NULL
    };

    return c;
}
