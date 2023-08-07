#ifndef TEXTURES_DECLARATION_HLSLI
#define TEXTURES_DECLARATION_HLSLI

Texture2D g_albedoMap : register(t0);
Texture2D g_metallicMap : register(t1);
Texture2D g_roughnessMap : register(t2);
Texture2D g_normalMap : register(t3);
Texture2D g_flashlightMask : register(t11);
TextureCube g_diffuseIrradianceCubemap : register(t20);
TextureCube g_specularIrradianceCubemap : register(t21);
Texture2D g_specularReflectanceTexture2D : register(t22);
Texture2D g_shadowMaps2D[8] : register(t23);
TextureCube g_shadowMapsCubemaps[8] : register(t35);

#endif //TEXTURES_DECLARATION_HLSLI