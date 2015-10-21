
float4 fViewportDimensions;

sampler sourceTexture;



struct VSInput
{
        float4 p : POSITION;
};

struct VSOutput
{
        float4 p : POSITION;
};


struct PSInput
{
        float2 vp : VPOS;
};

