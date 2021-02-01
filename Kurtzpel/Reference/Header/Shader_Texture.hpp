#include "Shader_Base.hpp"

texture		g_BaseTexture;

Matrix		g_matInv;

int		iCnt;
int		iMaxCnt;
float	fTexCX;
float	fTexCY;
float	fDrawX;
float	fDrawY;
float	fDrawCX;
float	fDrawCY;
float	fAlpha;

sampler BaseSampler = sampler_state
{
	texture	= g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
};

struct VS_IN
{
	vector		vPos : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPos : POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float3 vPos = In.vPos.xyz;

	/*vPos.x += (iCnt - (int)(iMaxCnt * 0.5f)) * 2.f;

	if(iMaxCnt % 2 == 0)
		vPos.x += 1.f;*/

	Matrix		matWV, matWVP, matWVPB;

	matWV = mul(g_matWorld, g_matView);

	matWVP = mul(matWV, g_matProj);
	
 

	Out.vPos = mul(vector(vPos, 1.f), matWVP);
	
	float2 uv = In.vTexUV;

	if(uv.x == 0 && uv.y == 0)
	{
		uv.x = fDrawX/fTexCX;
		uv.y = fDrawY/fTexCY;
	}
	else if(uv.x == 1 && uv.y == 0)
	{
		uv.x = (fDrawX + fDrawCX) / fTexCX;
		uv.y = fDrawY/fTexCY;
	}
	else if(uv.x == 1 && uv.y == 1)
	{
		uv.x = (fDrawX + fDrawCX) / fTexCX;
		uv.y = (fDrawY + fDrawCY)/ fTexCY;
	}
	else
	{
		uv.x = fDrawX / fTexCX;
		uv.y = (fDrawY + fDrawCY)/fTexCY;
	}

	Out.vTexUV = uv;

	return Out;
}

VS_OUT VS_UI(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vPos = In.vPos;
	Out.vTexUV = In.vTexUV;
	Out.vPos.x += 0.2f;
	Out.vPos.y += 0.4f;
	Out.vTexUV.y *= 2.f;

	//Out.vPos.y += 0.5f;

	//float2 uv = In.vTexUV;

	//if (uv.x == 0 && uv.y == 0)
	//{
	//	uv.x = fDrawX / fTexCX;
	//	uv.y = fDrawY / fTexCY;
	//}
	//else if (uv.x == 1 && uv.y == 0)
	//{
	//	uv.x = (fDrawX + fDrawCX) / fTexCX;
	//	uv.y = fDrawY / fTexCY;
	//}
	//else if (uv.x == 1 && uv.y == 1)
	//{
	//	uv.x = (fDrawX + fDrawCX) / fTexCX;
	//	uv.y = (fDrawY + fDrawCY) / fTexCY;
	//}
	//else
	//{
	//	uv.x = fDrawX / fTexCX;
	//	uv.y = (fDrawY + fDrawCY) / fTexCY;
	//}

	//Out.vTexUV = uv;

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

	Out.vColor.a *= fAlpha;

	return Out;
}

PS_OUT PS_UI(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	//Out.vColor.a *= fAlpha;
	//if (In.vTexUV.y > 0.2f) {
	//	Out.vColor.a = 0.f;
	//}

	return Out;
}

technique Default_Technique
{
	pass Default
	{	
		zenable = false;
		zwriteenable = false;

// 		alphatestenable = true;
// 		alphafunc = greaterEqual;
// 		alpharef = 0x00000080;

		AlphaBlendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
		
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass UI
	{
		//zenable = false;
		//zwriteenable = false;
		//AlphaBlendenable = true;
		//srcblend = srcalpha;
		//destblend = invsrcalpha;

		//alphatestenable = true;
		//alphafunc = greaterEqual;
		//alpharef = 0x00000080;


		VertexShader = compile vs_3_0 VS_UI();
		PixelShader = compile ps_3_0 PS_UI();
	}

};
