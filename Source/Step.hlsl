//
#include "ShadersCommon.h"


VSOutput mainVS (VSInput v)
{
        VSOutput o;
        o.p = v.p;
        return o;
};



float IsActiveCell(float2 p)
{
  float2 uv = p / fViewportDimensions.xy;
  float value = tex2D(sourceTexture, uv).r;
  return value;
}


float4 mainPS (PSInput p) : COLOR 
{

  // https://en.wikipedia.org/wiki/Conway's_Game_of_Life
  //
  // Conway's "Life"
  //
  //  Any live cell with fewer than two live neighbours dies, as if caused by under-population.
  //  Any live cell with two or three live neighbours lives on to the next generation.
  //  Any live cell with more than three live neighbours dies, as if by over-population.
  //  Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.


  float2 pix = (p.vp + float2(0.5, 0.5));

  float2 n0 = pix + float2(  0.0, -1.0);
  float2 n1 = pix + float2(  0.0,  1.0);
  float2 n2 = pix + float2( -1.0,  0.0);
  float2 n3 = pix + float2(  1.0,  0.0);
  float2 n4 = pix + float2( -1.0,  1.0);
  float2 n5 = pix + float2(  1.0,  1.0);
  float2 n6 = pix + float2( -1.0, -1.0);
  float2 n7 = pix + float2(  1.0, -1.0);

  float v0 = IsActiveCell(n0);
  float v1 = IsActiveCell(n1);
  float v2 = IsActiveCell(n2);
  float v3 = IsActiveCell(n3);
  float v4 = IsActiveCell(n4);
  float v5 = IsActiveCell(n5);
  float v6 = IsActiveCell(n6);
  float v7 = IsActiveCell(n7);

  float oldState = IsActiveCell(pix);

  float v = v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7;

  float newState = oldState;

  // is active
  [unroll]
  if (oldState > 0.0)
  {
    [unroll]
    if (v < 2)
    {
      newState = 0.0;
    }

    [unroll]
    if (v > 3)
    {
      newState = 0.0;
    }
  } else
  {
    [unroll]
    if (v == 3)
    {
      newState = 1.0;
    }
  }

  return newState;
}