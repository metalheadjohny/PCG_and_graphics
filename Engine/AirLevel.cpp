#include "AirLevel.h"



void AirLevel::init(Systems& sys)
{
	skybox.LoadModel(device, "../Models/Skysphere.fbx");
	skyboxCubeMapper.LoadFromFiles(device, "../Textures/day.dds");

	/*Procedural::Terrain barrensTerrain;
	auto fltVec = Texture::generateRidgey(256, 256, 0.f, 1.61803f, 0.5793f, 1.f, 6u);
	barrensTerrain.setScales(2, 128, 2);
	barrensTerrain.GenFromTexture(256, 256, fltVec);
	barrensTerrain.setOffset(-256, 0, -256);
	barrensTerrain.setTextureData(device, 10, 10, { "../Textures/LavaCracks/diffuse.png", "../Textures/LavaCracks/normal.png" });
	barrensTerrain.SetUp(device);
	
	barrens = Model(barrensTerrain, device);
	*/

	LightData lightData(SVec3(0.1, 0.7, 0.9), .03f, SVec3(0.8, 0.8, 1.0), .2, SVec3(0.3, 0.5, 1.0), 0.7);
	pointLight = PointLight(lightData, SVec4(0, 500, 0, 1));
	dirLight = DirectionalLight(lightData, SVec4(0, -1, 0, 1));

	//light "camera matrix"
	lightView = SMatrix::CreateFromAxisAngle(SVec3(1, 0, 0), PI * 0.5) * SMatrix::CreateTranslation(SVec3(pointLight.pos));
	lightView.Invert();		//get the view matrix of the light
	lightView.Transpose();	//transpose so it doesn't have to be transposed by the shader class each frame

	worley = Texture(device, "../Textures/worley.png");

	headModel.LoadModel(device, "../Models/Ball.fbx");	//../Models/Dragon/dragonhead.obj
	segmentModel.LoadModel(device, "../Models/cate.fbx");

	dragon.init(10, SVec3(0, 0, 200));
	instanceData.resize(10);
}



void AirLevel::draw(const RenderContext& rc)
{
	rc.d3d->ClearColourDepthBuffers(rc.d3d->clearColour);
	ProcessSpecialInput(rc.dTime);
	updateCam(rc.dTime);

	dragon.update(rc, windDir * windInt);

	for (int i = 0; i < dragon.springs.size(); ++i)
		instanceData[i]._m = dragon.springs[i].transform.Transpose();

	shady.instanced.UpdateInstanceData(instanceData);
	shady.instanced.SetShaderParameters(context, segmentModel, *rc.cam, pointLight, rc.dTime);
	segmentModel.Draw(context, shady.instanced);
	shady.instanced.ReleaseShaderParameters(context);

	//_sys._D3D.TurnOnAlphaBlending();
	//shady.strife.SetShaderParameters(context, headModel, *rc.cam, dirLight, rc.elapsed, worley.srv, lightView);
	//headModel.Draw(context, shady.strife);
	//shady.strife.ReleaseShaderParameters(context);
	//_sys._D3D.TurnOffAlphaBlending();

	//shady.terrainNormals.SetShaderParameters(context, barrens.transform, *rc.cam, pointLight, rc.dTime);
	//barrens.Draw(context, shady.terrainNormals);

	randy.RenderSkybox(*rc.cam, skybox, skyboxCubeMapper);

	rc.d3d->EndScene();
}
