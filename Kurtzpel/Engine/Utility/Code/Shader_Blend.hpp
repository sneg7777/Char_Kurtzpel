texture			g_AlbedoTexture;
float			g_Power = 0.8f;
bool			g_Active;

sampler AlbedoSampler = sampler_state
{
	texture = g_AlbedoTexture;

	minfilter = linear;
	magfilter = linear;
};

texture			g_ShadeTexture;

sampler ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;

	minfilter = linear;
	magfilter = linear;
};


texture			g_SpecularTexture;

sampler SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;

	minfilter = linear;
	magfilter = linear;
};

struct PS_IN
{
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector			vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vAlbedo = tex2D(AlbedoSampler, In.vTexUV);
	if (g_Active) {
		float2 vDir = 0.5f - In.vTexUV;
		float fDist = length(vDir);

		vDir /= fDist;
		vector vSum = vAlbedo;

		vSum += tex2D(AlbedoSampler, In.vTexUV + vDir * -0.04f * g_Power);
		vSum += tex2D(AlbedoSampler, In.vTexUV + vDir * -0.03f * g_Power);
		vSum += tex2D(AlbedoSampler, In.vTexUV + vDir * -0.02f * g_Power);
		vSum += tex2D(AlbedoSampler, In.vTexUV + vDir * -0.01f * g_Power);

		vSum += tex2D(AlbedoSampler, In.vTexUV + vDir * 0.01f * g_Power);
		vSum += tex2D(AlbedoSampler, In.vTexUV + vDir * 0.02f * g_Power);
		vSum += tex2D(AlbedoSampler, In.vTexUV + vDir * 0.03f * g_Power);
		vSum += tex2D(AlbedoSampler, In.vTexUV + vDir * 0.04f * g_Power);

		vSum *= (1.f / 9.f);
		float fT = saturate(fDist * 2.2f);

		vAlbedo = lerp(vAlbedo, vSum, fT);
	}
	vector		vShade = tex2D(ShadeSampler, In.vTexUV);

	//vector		vSpecular = tex2D(SpecularSampler, In.vTexUV);
	 
	Out.vColor = vAlbedo * vShade;// +vSpecular;

	return Out;
}

technique Default_Device
{
	// ±â´ÉÀÇ Ä¸½¶È­
	pass Blend
	{
		zwriteenable = false;
		
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = NULL;
		pixelshader = compile ps_3_0 PS_MAIN();
	}
};