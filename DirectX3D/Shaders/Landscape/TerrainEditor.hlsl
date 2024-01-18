#include "../VertexHeader.hlsli"
#include "../PixelHeader.hlsli"

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDir : VIEWDIR;
	float3 worldPos : POSITION;
	float4 alpha : ALPHA;
};


PixelInput VS(VertexUVNormalTangentAlpha input)
{
	PixelInput output;
	output.pos = mul(input.pos, world);
	
	output.viewDir = normalize(invView._31_32_33);
	output.worldPos = output.pos;
	
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	output.uv = input.uv;
	
	output.normal = mul(input.normal, (float3x3) world);
	output.tangent = mul(input.tangent, (float3x3) world);
	output.binormal = cross(output.normal, output.tangent);
	
	output.alpha = input.alpha;
	
	return output;
}

cbuffer BrushBuffer : register(b10)
{
	int type;
	float3 pickingPos;
	
	float range;
	float3 color;
}

float4 BrushColor(float3 pos)
{
	if (type == 0 || type == 1)
	{
		float2 direction = pos.xz - pickingPos.xz;
		
		float distance = length(direction);

		if (distance <= range)
			return float4(color, 0);
	}
	else if (type == 2)
	{
		float size = range * 0.5f;
		
		if (abs(pos.x - pickingPos.x) < size
			&& abs(pos.z - pickingPos.z) < size)
		{
			return float4(color, 0);
		}
	}
	
	return float4(0, 0, 0, 0);
}

Texture2D secondDiffuseMap : register(t11);
Texture2D thirdDiffuseMap : register(t12);

float4 PS(PixelInput input) : SV_TARGET
{
	//float4 albedo = diffuseMap.Sample(samp, input.uv);
	//float4 second = secondDiffuseMap.Sample(samp, input.uv);
	//float4 third = thirdDiffuseMap.Sample(samp, input.uv);
	//
	//albedo = lerp(albedo, second, input.alpha.r);
	//albedo = lerp(albedo, third, input.alpha.g);
	//
	//float3 T = normalize(input.tangent);
	//float3 B = normalize(input.binormal);
	//float3 N = normalize(input.normal);
	//
	//float3 normal = N;
	//float3 light = normalize(lightDirection);	
	//
	//if (hasNormalMap)
	//{
	//	float3 normalMapColor = normalMap.Sample(samp, input.uv).rgb;
	//	normal = normalMapColor * 2.0f - 1.0f; //0~1 -> -1~1
	//	float3x3 TBN = float3x3(T, B, N);
	//	normal = normalize(mul(normal, TBN));
	//}
	//
	//float diffuseIntensity = saturate(dot(normal, -light));
	//
	//float4 specular = 0;
	//if (diffuseIntensity > 0)
	//{
	//	float3 halfWay = normalize(input.viewDir + light);
	//	specular = saturate(dot(normal, -halfWay));
	//	
	//	float4 specularIntensity = specularMap.Sample(samp, input.uv);
	//	
	//	specular = pow(specular, shininess) * specularIntensity * mSpecular * lightColor;
	//}
	//
	//float4 diffuse = albedo * diffuseIntensity * mDiffuse * lightColor;
	//float4 ambient = albedo * ambientColor * mAmbient;
	//
	//float4 brushColor = BrushColor(input.worldPos);
	//
	//return diffuse + specular + ambient + brushColor;
	return float4(1, 1, 1, 1);
}