cbuffer CameraCB : register(b1)
{
    float3 g_cameraPosition;
    float _g_camera_cb_pad1;
    float3 g_cameraVertical;
    float _g_camera_cb_pad2;
    float3 g_cameraHorizontal;
    float _g_camera_cb_pad3;
    float3 g_cameraLeftTopCorner;
    float _g_camera_cb_pad4;
    float4x4 g_worldToView;
    float4x4 g_viewToClip;
    float4x4 g_clipToView;
    float4x4 g_viewToWorld;
};