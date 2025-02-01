#include "Object3d.hlsli"

struct Material{
    float32_t4 color;
    float32_t4 highLightColor;
    float32_t4x4 uvTransform;
    int32_t enableLighting;
    float32_t shininess;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight{
    float32_t4 color;//!<ライトの色
    float32_t3 direction;//!< ライトの向き
    float32_t intensity; //!< 輝度
    int32_t isBlinnPhong; //!<BlinnPhongかどうか
    int32_t PointLightCount;
    int32_t SpotLightCount;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Camera{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PointLight{
    float32_t4 color; //!<ライトの色
    float32_t3 position; //!< ライトの場所
    float32_t intensity; //!< 輝度
    float32_t radius; //!<ライトの届く最大距離
    float32_t decay; //!< 減衰率
    float32_t padding[2];
};
StructuredBuffer<PointLight> gPointLight : register(t1);

struct SpotLight{
    float32_t4 color; //!<ライトの色
    float32_t3 position; //!< ライトの場所
    float32_t intensity; //!< 輝度
    float32_t3 direction; //!< ライトの向き
    float32_t distance; //!<ライトの届く最大距離
    float32_t decay; //!<減衰率
    float32_t cosAngle; //!<スポットライトの余弦
    float32_t cosFalloffStart;//!<Falloffの開始角度
    float32_t padding;
};
StructuredBuffer<SpotLight> gSpotLight : register(t2);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

float32_t3 Specular(VertexShaderOutput input, float32_t3 lightDirection, float32_t3 lightColor)
{
    //Cameraへの方向
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    if (gDirectionalLight.isBlinnPhong != 0)
    {
        //Blinn-Phong
        float32_t3 halfVector = normalize(-lightDirection + toEye); //入射角の反射ベクトル
        float32_t NDotH = dot(normalize(input.normal), halfVector);
        float32_t specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
        return lightColor * specularPow * gMaterial.highLightColor.rgb;
    }
    else
    {
        //Phong
        float32_t3 reflectLight = reflect(lightDirection, normalize(input.normal));
        float32_t RdotE = dot(reflectLight, toEye);
        float32_t specularPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
        return lightColor * specularPow * gMaterial.highLightColor.rgb;
    }
}
float32_t3 MakePointLightColor(VertexShaderOutput input, float32_t4 textureColor, PointLight pointLight)
{
    float32_t3 pointLightDirection = normalize(input.worldPosition - pointLight.position);
    float32_t distance = length(pointLight.position - input.worldPosition); //ポイントライトへの距離
    float32_t factor = pow(saturate(-distance / pointLight.radius + 1.0f), pointLight.decay); //指数によるコントロール
    float32_t3 lightColor = pointLight.color.rgb * pointLight.intensity * factor;
    //拡散反射
    float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * lightColor.rgb;
    //鏡面反射
    float32_t3 specular = Specular(input, pointLightDirection, lightColor);
        
    return diffuse + specular;
}

float32_t3 MakeSpotLightColor(VertexShaderOutput input, float32_t4 textureColor, SpotLight spotLight)
{
    float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - spotLight.position);
    float32_t distance = length(spotLight.position - input.worldPosition); //スポットライトへの距離
    float32_t attenuationFactor = pow(saturate(-distance / spotLight.distance + 1.0f), spotLight.decay); //距離による減衰
    float32_t cosAngle = dot(spotLightDirectionOnSurface, spotLight.direction);
        //if (spotLight.cosFalloffStart - spotLight.cosAngle == 0)//ゼロ除算の時は赤くする
        //{
        //    output.color = float32_t4(1.0f, 0.0f, 0.0f, 1.0f);
        //    return output;
        //}
    float32_t falloffFactor = saturate((cosAngle - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle + 0.001f));
    float32_t3 lightColor = spotLight.color.rgb * spotLight.intensity * attenuationFactor * falloffFactor;
        //拡散反射
    float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * lightColor.rgb;
        //鏡面反射
    float32_t3 specular = Specular(input, spotLightDirectionOnSurface, lightColor);
    
    return diffuse + specular;
}

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    //テクスチャUV
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    //テクスチャカラー
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    if (textureColor.a == 0.0){ discard;}
    if (textureColor.a <= 0.5){ discard;}
    
    //outputカラー
    if (gMaterial.enableLighting != 0){ //Lightingする場合
        //DirectionalLight
        //拡散反射
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        //鏡面反射
        float32_t3 specular = Specular(input,gDirectionalLight.direction,gDirectionalLight.color.rgb * gDirectionalLight.intensity);
        
        float32_t3 directionalLightCollor = diffuse + specular;
        
        //PointLight
        float32_t3 pointLightColor = float32_t3(0.0f,0.0f,0.0f);
        for (uint32_t i = 0; i < gDirectionalLight.PointLightCount; ++i)
        {
            pointLightColor += MakePointLightColor(input, textureColor, gPointLight[i]);
        }
        
        //SpotLight
        float32_t3 spotLightColor = float32_t3(0.0f, 0.0f, 0.0f);
        for (uint32_t j = 0; j < gDirectionalLight.SpotLightCount; ++j)
        {
            spotLightColor += MakeSpotLightColor(input, textureColor, gSpotLight[j]);
        }
        
        //拡散反射・鏡面反射
        output.color.rgb = directionalLightCollor + pointLightColor + spotLightColor;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else{
        output.color = gMaterial.color * textureColor;
    }
    if (output.color.a == 0.0){ discard;}
    
    return output;
}