matrix			g_matWorld;		// 상수 테이블
matrix			g_matView;
float4x4		g_matProj;

float			g_fTimeDelta;
float			g_fBoldSize;
vector			g_vColor;

bool			g_bIsDissolve = false;

float			g_uOffset;
float			g_vOffset;
float			g_uStep;
float			g_vStep;

texture			g_BaseTexture;
sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
};

texture			g_DissolveTexture;
sampler DissolveSampler = sampler_state
{
	texture = g_DissolveTexture;

	minfilter = linear;
	magfilter = linear;
};

struct	VS_IN 
{
	vector		vPosition	: POSITION;
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct	VS_OUT
{
	vector		vPosition	: POSITION;
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	vector		vProjPos	: TEXCOORD1;
};

// 버텍스 쉐이더

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);

	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));

	float2 texUV = In.vTexUV;
	texUV.x = (g_uOffset * texUV.x) + g_uStep * g_uOffset;
	texUV.y = (g_vOffset * texUV.y) + g_vStep * g_vOffset;
	Out.vTexUV = texUV;

	//Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT VS_OUTLINE(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matW, matWV, matWVP;

	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));
	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), g_matWorld);
	Out.vPosition += Out.vNormal * g_fBoldSize;

	Out.vPosition = mul(vector(Out.vPosition.xyz, 1.f), g_matView);
	Out.vPosition = mul(vector(Out.vPosition.xyz, 1.f), g_matProj);

	//Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct	PS_IN
{
	vector		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	vector		vProjPos : TEXCOORD1;
};

struct	PS_OUT
{
	vector		vColor : COLOR0;
	vector		vNormal : COLOR1;	// 노말이라는 렌더타겟(텍스쳐)에 출력하기 위한 색상
	vector		vDepth : COLOR2;
};

PS_OUT		PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	vector vDissolve = tex2D(DissolveSampler, In.vTexUV);
	
	if (g_bIsDissolve)
	{
		float fAmount = vDissolve.r + g_fTimeDelta;
		if (fAmount >= 1.f)
			Out.vColor.a = 0.f;
	}
	Out.vColor.r *= 0.7f;//g_fAlpha;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w * 0.01f, 0.f, 0.f);
	
	return Out;
}


PS_OUT		PS_OUTLINE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//float fColor = 0.f;
	//Out.vColor = vector(fColor.xxx, 1.f);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 얻어오는 함수, 반환 타입이 vector 타입
	Out.vColor = vector(g_vColor.x, g_vColor.y, g_vColor.z, 1.f);
	vector vDissolve = tex2D(DissolveSampler, In.vTexUV);

	if (g_bIsDissolve)
	{
		float fAmount = vDissolve.r + g_fTimeDelta;
		if (fAmount >= 1.f)
			Out.vColor.a = 0.f;
	}


	return Out;
}

technique Default_Device
{
	// 기능의 캡슐화
	pass Default
	{
		alphatestenable = true;
		alpharef = 0xc0;
		alphafunc = greater;
		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}

	pass Outline
	{
		alphatestenable = true;
		alpharef = 0xc0;
		alphafunc = greater;
		cullmode = cw;

		vertexshader = compile vs_3_0 VS_OUTLINE();
		pixelshader = compile ps_3_0 PS_OUTLINE();
	}	
};