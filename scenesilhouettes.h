#pragma once

#include "scene.h"
#include "glslprogram.h"
#include "quad.h"
#include "objmesh.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <string>

class SceneSilhouettes : public Scene
{
private:
	static const int DEPTH_PEELING_NUM = 7;

	GLSLProgram silhouetteProg;
	GLSLProgram gBufferProg;
	GLSLProgram edgeMapProg;
	GLSLProgram blueprintProg;
	
	std::string objFilePath;
	int renderOption;

	// for silhouette
	GLuint silhouetteFBO;
	GLuint silhouetteTex;
	
	// for blueprint
	GLuint gBufferFBOs[DEPTH_PEELING_NUM];
	GLuint gBufferTex[DEPTH_PEELING_NUM];
	GLuint colorLayerTex[DEPTH_PEELING_NUM];
	GLuint edgeMapFBOs[DEPTH_PEELING_NUM];
	GLuint edgeMapTex[DEPTH_PEELING_NUM];	

	Quad quad;
	std::unique_ptr<ObjMesh> mesh;

	void setMatrices(GLSLProgram& prog);

	void compileAndLinkShader();
	void setupFBOForSilhouette();
	void renderSilhouette();
	void renderToTexture();
	void renderScene();

	void setupFBOForBlueprint();
	void renderBlueprint();
	void renderGBuffer();
	void renderEdgemap();
	void renderComposingBluprint();

public:
	SceneSilhouettes(std::string filePath);

	void initScene();
	void update(float deltaTime);
	void render();
	void resize(int, int);

	void setRenderOption(int option);

	enum
	{
		N_DOT_V,
		SILHOUETTE,
		BLUEPRINT
	};
};