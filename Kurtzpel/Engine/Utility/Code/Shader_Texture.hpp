#include "Shader_Base.hpp"

texture		g_BaseTexture;

Matrix		g_matInv;

float	g_fAlpha;
float	g_fRatioY;

sampler BaseSampler = sampler_state
{
	texture	= g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
};

struct VS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;


	return Out;
}

VS_OUT VS_UI(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	if (Out.vColor.r != 0.f) {
		Out.vColor.a *= g_fAlpha;
	}
	if (g_fRatioY != 1.f) {
		if (In.vTexUV.y < g_fRatioY)
			Out.vColor.a = 0.f;
	}

	
	return Out;
}

PS_OUT PS_UI(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

technique Default_Technique
{
	pass Default
	{	
// 		alphatestenable = true;
// 		alphafunc = greaterEqual;
// 		alpharef = 0x00000080;

		AlphaBlendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		ZEnable = false;
		ZWriteEnable = true;
		
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass UI
	{
		AlphaBlendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		ZEnable = false;
		ZWriteEnable = true;


		VertexShader = compile vs_3_0 VS_UI();
		PixelShader = compile ps_3_0 PS_UI();
	}

};
