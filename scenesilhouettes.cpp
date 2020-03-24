#include "scenesilhouettes.h"

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
using glm::vec3;
using glm::mat4;

SceneSilhouettes::SceneSilhouettes(std::string filePath) : renderOption(N_DOT_V)
{
	objFilePath = "../" + filePath;
}

void SceneSilhouettes::initScene()
{
	mesh = ObjMesh::load(objFilePath.c_str());

	compileAndLinkShader();

	setupFBOForSilhouette();
	setupFBOForBlueprint();
}

void SceneSilhouettes::update(float deltaTime)
{
}

void SceneSilhouettes::setupFBOForSilhouette()
{
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &silhouetteFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, silhouetteFBO);

	// Create the texture object
	glGenTextures(1, &silhouetteTex);
	glActiveTexture(GL_TEXTURE0);  // Use texture unit 0
	glBindTexture(GL_TEXTURE_2D, silhouetteTex);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, silhouetteTex, 0);

	// Create the depth buffer
	GLuint depthBuf;
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	// Bind the depth buffer to the FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

	// Set the targets for the fragment output variables
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "Framebuffer is complete" << endl;
	}
	else
	{
		cout << "Framebuffer error: " << result << endl;
	}

	// Unbind the framebuffer, and revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneSilhouettes::setupFBOForBlueprint()
{
	// set up G-buffer FBO
	glGenFramebuffers(DEPTH_PEELING_NUM, gBufferFBOs);
	glGenTextures(DEPTH_PEELING_NUM, gBufferTex);
	glGenTextures(DEPTH_PEELING_NUM, colorLayerTex);
	for (int i = 0; i < DEPTH_PEELING_NUM; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBOs[i]);

		glActiveTexture(GL_TEXTURE0); // Use texture unit 0
		glBindTexture(GL_TEXTURE_2D, gBufferTex[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		glActiveTexture(GL_TEXTURE1); // Use texture unit 1
		glBindTexture(GL_TEXTURE_2D, colorLayerTex[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBufferTex[i], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorLayerTex[i], 0);

		GLuint depthBuf;
		glGenRenderbuffers(1, &depthBuf);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, drawBuffers);

		GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (result == GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "Framebuffer is complete" << endl;
		}
		else
		{
			cout << "Framebuffer error: " << result << endl;
		}
	}

	// set up edge-map FBO
	glGenFramebuffers(DEPTH_PEELING_NUM, edgeMapFBOs);
	glGenTextures(DEPTH_PEELING_NUM, edgeMapTex);
	for (int i = 0; i < DEPTH_PEELING_NUM; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, edgeMapFBOs[i]);

		glActiveTexture(GL_TEXTURE2); // Use texture unit 2
		glBindTexture(GL_TEXTURE_2D, edgeMapTex[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, edgeMapTex[i], 0);

		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (result == GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "Framebuffer is complete" << endl;
		}
		else
		{
			cout << "Framebuffer error: " << result << endl;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneSilhouettes::render()
{
	switch (renderOption)
	{
	case N_DOT_V:
	case SILHOUETTE:
		renderSilhouette();
		break;

	case BLUEPRINT:
		renderBlueprint();
		break;
	}
}

void SceneSilhouettes::renderSilhouette()
{
	silhouetteProg.use();
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, silhouetteFBO);
	renderToTexture();
	glFlush();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderScene();
}

void SceneSilhouettes::renderToTexture()
{
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model = glm::mat4(1.0f);
	view = camera->GetViewMatrix();
	projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 100.0f);

	silhouetteProg.setUniform("Pass", 1);
	silhouetteProg.setUniform("EdgeWidth", 0.01f);
	silhouetteProg.setUniform("PctExtend", 0.15f);

	setMatrices(silhouetteProg);
	mesh->render();
}

void SceneSilhouettes::renderScene()
{
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, silhouetteTex);

	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	silhouetteProg.setUniform("Pass", 2);

	if (renderOption == N_DOT_V)
	{
		silhouetteProg.setUniform("SilhouetteMode", false);
	}
	else if (renderOption == SILHOUETTE)
	{
		silhouetteProg.setUniform("SilhouetteMode", true);
	}

	setMatrices(silhouetteProg);
	quad.render();
}


void SceneSilhouettes::renderBlueprint()
{
	renderGBuffer();
	renderEdgemap();
	renderComposingBluprint();
}

void SceneSilhouettes::renderGBuffer()
{
	gBufferProg.use();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	model = glm::mat4(1.0f);
	view = camera->GetViewMatrix();
	projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 100.0f);

	for (int i = 0; i < DEPTH_PEELING_NUM; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBOs[i]);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (i != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gBufferTex[i - 1]);
		}

		gBufferProg.setUniform("Depth", i);

		setMatrices(gBufferProg);
		mesh->render();

		glFlush();
	}
}

void SceneSilhouettes::renderEdgemap()
{
	edgeMapProg.use();
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	for (int i = 0; i < DEPTH_PEELING_NUM; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, edgeMapFBOs[i]);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBufferTex[i]);

		quad.render();

		glFlush();
	}
}

void SceneSilhouettes::renderComposingBluprint()
{
	blueprintProg.use();
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBufferTex[4]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorLayerTex[4]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, edgeMapTex[4]);

	quad.render();
	
	/*
	for (int i = 5; i >= 0; --i)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBufferTex[i]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, colorLayerTex[i]);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, edgeMapTex[i]);

		quad.render();
	}
	*/
}

void SceneSilhouettes::setMatrices(GLSLProgram& prog)
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix",
		glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("ProjectionMatrix", projection);
	prog.setUniform("MVP", projection * mv);
}

void SceneSilhouettes::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneSilhouettes::compileAndLinkShader()
{
	try
	{
		silhouetteProg.compileShader("silhouettes.vert");
		silhouetteProg.compileShader("silhouettes.geom");
		silhouetteProg.compileShader("silhouettes.frag");
		silhouetteProg.link();

		gBufferProg.compileShader("makeGBuffer.vert");
		gBufferProg.compileShader("makeGBuffer.frag");
		gBufferProg.link();

		edgeMapProg.compileShader("makeEdgeMap.vert");
		edgeMapProg.compileShader("makeEdgeMap.frag");
		edgeMapProg.link();

		blueprintProg.compileShader("blueprint.vert");
		blueprintProg.compileShader("blueprint.frag");
		blueprintProg.link();
	}
	catch (GLSLProgramException & e)
	{
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneSilhouettes::setRenderOption(int option)
{
	renderOption = option;
}