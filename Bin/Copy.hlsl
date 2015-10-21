//
#include "ShadersCommon.h"


VSOutput mainVS (VSInput v)
{
        VSOutput o;
        o.p = v.p;
        return o;
};



float4 mainPS (PSInput p) : COLOR 
{
  float2 uv = (p.vp + float2(0.5, 0.5)) / fViewportDimensions.xy;
  float4 texel = tex2D(sourceTexture, uv);
  return texel;
}