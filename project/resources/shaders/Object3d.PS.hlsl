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
    float32_t intensity; //!< 輝度
    int32_t isBlinnPhong; //!<BlinnPhongかどうか
    
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Camera{
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

struct SpotLight{
    float32_t4 color; //!<ライトの色
    float32_t3 position; //!< ライトの場所
    float32_t intensity; //!< 輝度
    float32_t3 direction; //!< ライトの向き
    float32_t distance; //!<ライトの届く最大距離
    float32_t decay; //!<減衰率
    float32_t cosAngle; //!<スポットライトの余弦
    float32_t cosFalloffStart;//!<Falloffの開始角度
};
ConstantBuffer<SpotLight> gSpotLight : register(b4);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PointLight{
    float32_t4 color; //!<ライトの色
    float32_t3 position; //!< ライトの場所
    float32_t intensity; //!< 輝度
    float32_t radius; //!<ライトの届く最大距離
    float32_t decay; //!< 減衰率
};
ConstantBuffer<PointLight> gPointLight : register(b3);
//StructuredBuffer<PointLight> gPointLight : register(t1);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    //テクスチャUV
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
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
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        //鏡面反射
        //入射角の反射ベクトル
        
        float32_t3 specular;
        float32_t3 halfVector;
        float NDotH;
        float specularPow;
        float32_t3 reflectLight;
        float RdotE;
        
        if (gDirectionalLight.isBlinnPhong != 0){
            //Blinn-Phong
            halfVector = normalize(-gDirectionalLight.direction + toEye);
            NDotH = dot(normalize(input.normal), halfVector);
            specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
            specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        }
        else{
            //Phong
            reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
            RdotE = dot(reflectLight, toEye);
            specularPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
            specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        }
        
        float32_t3 directionalLightCollor = diffuse + specular;
        
        //PointLight
        float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
        float32_t distance = length(gPointLight.position - input.worldPosition);//ポイントライトへの距離
        float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f),gPointLight.decay);//指数によるコントロール
        float32_t3 pointLightColor = gPointLight.color.rgb * gPointLight.intensity * factor;
        //拡散反射
            diffuse = gMaterial.color.rgb * textureColor.rgb * pointLightColor.rgb;
        //鏡面反射
        //入射角の反射ベクトル
        //Blinn-Phong
        halfVector = normalize(-pointLightDirection + toEye);
        NDotH = dot(normalize(input.normal), halfVector);
        specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
        specular = pointLightColor.rgb * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        pointLightColor = diffuse + specular;
        
        //SpotLight
        float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
        distance = length(gSpotLight.position - input.worldPosition); //ポイントライトへの距離
        float32_t attenuationFactor = pow(saturate(-distance / gSpotLight.distance + 1.0f), gSpotLight.decay); //距離による減衰
        float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
        //if (gSpotLight.cosFalloffStart - gSpotLight.cosAngle == 0)
        //{
        //    output.color = float32_t4(1.0f, 0.0f, 0.0f, 1.0f);
        //    return output;
        //}
        float32_t falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle + 0.001f));
        float32_t3 spotLightColor = gSpotLight.color.rgb * gSpotLight.intensity * attenuationFactor * falloffFactor;
        //拡散反射
        diffuse = gMaterial.color.rgb * textureColor.rgb * spotLightColor.rgb;
        //鏡面反射
        //入射角の反射ベクトル
        //Blinn-Phong
        halfVector = normalize(-spotLightDirectionOnSurface + toEye);
        NDotH = dot(normalize(input.normal), halfVector);
        specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
        specular = spotLightColor.rgb * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        spotLightColor = diffuse + specular;
        
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