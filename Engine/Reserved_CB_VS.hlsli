cbuffer PerCameraBuffer : register(b10)
{
	matrix projectionMatrix;
	float scr_w;
	float scr_h;
	float zNear;
	float zFar;
};

cbuffer VSPerFrameBuffer : register(b11)
{
	matrix viewMatrix;
	float dTime;
	float eTime;
	float2 padding;
};

cbuffer WMBuffer: register(b0)
{
	matrix worldMatrix;
};