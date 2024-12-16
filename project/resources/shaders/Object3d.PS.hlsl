#include "Object3d.hlsli"

struct Material{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight{
    float32_t4 color;//!<ライトの色
    float32_t3 direction;//!< ライトの向き
    float intensity;//!< 輝度
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Camera{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

struct PointLight
{
    float32_t4 color; //!<ライトの色
    float32_t3 position; //!< ライトの場所
    float intensity; //!< 輝度
    float radius;//!<ライトの届く最大距離
    float decay;//!< 減衰率
};

ConstantBuffer<PointLight> gPointLight : register(b3);
struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    //テクスチャUV
    float4 transformedUV = mul(float32_t4(input.texcoord,0.0f, 1.0f), gMaterial.uvTransform);
    //テクスチャカラー
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    if (textureColor.a == 0.0){ discard;}
    if (textureColor.a <= 0.5){ discard;}
    //Cameraへの方向
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    //outputカラー
    if (gMaterial.enableLighting != 0){ //Lightingする場合
        //DirectionalLight
        //拡散反射
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float32_t3 diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        //鏡面反射
        //入射角の反射ベクトル
        //Phong
        //float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        //float RdotE = dot(reflectLight, toEye);
        // float specularPow = pow(saturate(RdotE), gMaterial.shininess);//反射強度
        //Blinn-Phong
        float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NDotH), gMaterial.shininess);//反射強度
        float32_t3 specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        //PointLight
        float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
        float32_t distance = length(gPointLight.position - input.worldPosition);//ポイントライトへの距離
        float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f),gPointLight.decay);//指数によるコントロール
        float32_t3 LightColor = gPointLight.color.rgb * gPointLight.intensity * factor;
        //拡散反射
        NdotL = dot(normalize(input.normal), -pointLightDirection);
        cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float32_t3 diffusePointLight = gMaterial.color.rgb * textureColor.rgb * LightColor.rgb;
        //鏡面反射
        //入射角の反射ベクトル
        //Blinn-Phong
        halfVector = normalize(-pointLightDirection + toEye);
        NDotH = dot(normalize(input.normal), halfVector);
        specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
        float32_t3 specularPointLight = LightColor.rgb * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        
        //拡散反射・鏡面反射
        output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else{
        output.color = gMaterial.color * textureColor;
    }
    if (output.color.a == 0.0){ discard;}
    
    return output;
}