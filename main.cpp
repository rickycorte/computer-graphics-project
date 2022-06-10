// This has been adapted from the Vulkan tutorial

#include <BaseProject.hpp>

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec3 missLightPos;
	alignas(16) glm::vec3 missLightDir;
};

struct SkyboxBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

// MAIN ! 
class MissileSimulator : public BaseProject {
protected:
	// Here you list all the Vulkan objects you need:

	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout standardDSL;

	// Pipelines [Shader couples]
	Pipeline standardPipeline;

	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout skyboxDSL;

	// Pipelines [Shader couples]
	Pipeline skyboxPipeline;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model Terrain;
	Texture TerrainTexture;
	DescriptorSet TerrainDs;

	glm::vec3 terrainPosition = glm::vec3(0, -2, -57);
	float terrainScale = 0.2f;

	Model Missile;
	Texture MissileTexture;
	DescriptorSet MissileDs;

	glm::vec3 missilePosition = glm::vec3(20, 1.4f, 60);
	glm::vec3 missileStartPostion = missilePosition;
	glm::vec3 oldMissilePosition = missilePosition;
	glm::vec3 missileDirection = glm::vec3(0.f, 1.f, 0.f); //missileBottomLightDir = - missiledirection
	float missileScale = 0.02;

	Model AimItem;
	Texture AimItemTexture;
	DescriptorSet AimItemDs;

	Model skybox;
	Texture skyboxTexture;
	DescriptorSet skyboxDs;

	//cam settings
	glm::vec3 CamAng = glm::vec3(1, 0, 0);

	float tpcRadius = 10.0f;

	const float ROT_SPEED = glm::radians(60.0f);
	const float MOUSE_RES = 500.0f;

	const float AIM_SPEED = 50;
	const float AIM_DISTANCE = 60; // max distance where yopu can move dest around terrain center

	// simulation settings and vars
	bool isSimulationRunning = false;
	bool aimMode = false;

	glm::vec3 missileDestination = glm::vec3(-10.0f, 0.0f, 15.0f);
	float missileSpeed = 8.0f;
	float missileTopHeight = 100.0f;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Missile Simulator";
		initialBackgroundColor = { 0, 0, 0.0f, 0.1f };

		// Descriptor pool sizes
		// 2 objs + 6 skybox blocks
		uniformBlocksInPool = 8;
		texturesInPool = 8;
		setsInPool = 8;
	}

	// Here you load and setup all your Vulkan objects
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		standardDSL.init(this, {
			// this array contains the binding:
			// first  element : the binding number
			// second element : the time of element (buffer or texture)
			// third  element : the pipeline stage where it will be used
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		standardPipeline.init(this, "shaders/vert.spv", "shaders/frag.spv", { &standardDSL });

		// Models, textures and Descriptors (values assigned to the uniforms)
		Terrain.init(this, "models/terrain.obj");
		TerrainTexture.init(this, "textures/terrain/diff.png");
		TerrainDs.init(this, &standardDSL, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &TerrainTexture}
			});

		Missile.init(this, "models/missile.obj");
		MissileTexture.init(this, "textures/missile/diff.jpg");
		MissileDs.init(this, &standardDSL, {
				{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
				{1, TEXTURE, 0, &MissileTexture}
			});


		AimItem.init(this, "models/sky_sphere.obj");
		AimItemTexture.init(this, "textures/aim.png");
		AimItemDs.init(this, &standardDSL, {
				{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
				{1, TEXTURE, 0, &AimItemTexture}
			});

		/************************************************************************************/
		skyboxDSL.init(this, {
				{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
				{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		
		//skyboxPipeline.init(this, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", { &skyboxDSL });
		skyboxPipeline.init(this, "shaders/sky/vert.spv", "shaders/sky/frag.spv", { &skyboxDSL});

		skybox.init(this, "models/sky_sphere.obj");
		//skyboxTexture.init(this, "textures/sky/");
		skyboxTexture.init(this, "textures/animecloud.png");
		skyboxDs.init(this, &skyboxDSL, {
				{0, UNIFORM, sizeof(SkyboxBufferObject), nullptr},
				{1, TEXTURE, 0, &skyboxTexture}
			});
	}

	// Here you destroy all the objects you created!		
	void localCleanup() {
		TerrainDs.cleanup();
		TerrainTexture.cleanup();
		Terrain.cleanup();

		MissileDs.cleanup();
		MissileTexture.cleanup();
		Missile.cleanup();

		standardPipeline.cleanup();
		standardDSL.cleanup();

		skyboxDs.cleanup();
		skyboxTexture.cleanup();
		skybox.cleanup();

		skyboxPipeline.cleanup();
		skyboxDSL.cleanup();
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			standardPipeline.graphicsPipeline);

		VkBuffer vertexBuffers[] = { Terrain.vertexBuffer };
		// property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		// property .indexBuffer of models, contains the VkBuffer handle to its index buffer
		vkCmdBindIndexBuffer(commandBuffer, Terrain.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);

		// property .pipelineLayout of a pipeline contains its layout.
		// property .descriptorSets of a descriptor set contains its elements.
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			standardPipeline.pipelineLayout, 0, 1, &TerrainDs.descriptorSets[currentImage],
			0, nullptr);

		// property .indices.size() of models, contains the number of triangles * 3 of the mesh.
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Terrain.indices.size()), 1, 0, 0, 0);


		VkBuffer vertexBuffers2[] = { Missile.vertexBuffer };
		VkDeviceSize offsets2[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers2, offsets2);
		vkCmdBindIndexBuffer(commandBuffer, Missile.indexBuffer, 0,VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			standardPipeline.pipelineLayout, 0, 1, &MissileDs.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Missile.indices.size()), 1, 0, 0, 0);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			skyboxPipeline.graphicsPipeline);

		VkBuffer vertexBuffers4[] = { AimItem.vertexBuffer };
		VkDeviceSize offsets4[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers4, offsets4);
		vkCmdBindIndexBuffer(commandBuffer, AimItem.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			standardPipeline.pipelineLayout, 0, 1, &AimItemDs.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(AimItem.indices.size()), 1, 0, 0, 0);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			skyboxPipeline.graphicsPipeline);

		VkBuffer vertexBuffers3[] = { skybox.vertexBuffer };
		VkDeviceSize offsets3[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers3, offsets3);
		vkCmdBindIndexBuffer(commandBuffer, skybox.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			skyboxPipeline.pipelineLayout, 0, 1, &skyboxDs.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(skybox.indices.size()), 1, 0, 0, 0);
	}

	//crea world matrix del missile
	glm::mat4 getMissileWorldMatrix(glm::vec3 startpoint, glm::vec3 destination, float dt) {

		oldMissilePosition = missilePosition;
		//aggiornamento posizione e direzione
		float yaw = 0.0f; //angolo di rotazione attorno asse y,  asse_x -> 0.0 radianti
		float pitch = 0.0f;

		//simulazione in esecuzione
		yaw = atan2(destination[2] - startpoint[2], destination[0] - startpoint[0]);
		//std::cout << "Yaw (deg): " << 180 * yaw / 3.14 << std::endl;
		//aggiornamento posizione
		missilePosition[0] += glm::cos(yaw) * missileSpeed * dt;
		missilePosition[2] += glm::sin(yaw) * missileSpeed * dt;

		//distanza piano su piano xz
		float actual_distance = sqrt(pow(missilePosition[0] - startpoint[0], 2) + pow(missilePosition[2] - startpoint[2], 2));
		float total_distance = sqrt(pow(startpoint[0] - destination[0], 2) + pow(startpoint[2] - destination[2], 2));

		//traiettoria parabola
		//il sistema di riferimento è il piano della parabola e l'origine è nello startpoint->punto0, destination->punto1, punto arbitrario->punto2
		glm::vec2 point0 = glm::vec2(0.0f);
		glm::vec2 point1 = glm::vec2(total_distance, destination[1] - startpoint[1]);
		glm::vec2 point2 = glm::vec2(total_distance / 2, missileTopHeight + std::max(point0[1], point1[1]));//terzo punto arbitrario per calcolare traiettoria parabolica
		glm::vec3 parab_param = CalcParabolaParam(point0, point1, point2);

		missilePosition[1] = startpoint[1] + parab_param[0] * pow(actual_distance, 2) + parab_param[1] * actual_distance + parab_param[2]; //calcolo y in cui si trova y = y_start + y_parabola
		float derivata = 2 * parab_param[0] * actual_distance + parab_param[1]; //tangente per calcolare l'angolo rispetto al piano xz
		pitch = atan(derivata) - glm::radians(90.0f); //offset di 90deg perchè il missile inizialmente parte verticale, se pitch=90 e yaw=0 il missile è diretto verso asse z+
		yaw = glm::radians(-90.0f) - yaw; //si passa da ssitema di riferimento xz (orario) a rotazione di yaw crescente anti-oraria 

		//std::cout << "Pitch (deg): " << 180 * pitch / 3.14 << std::endl;
		//std::cout << "Pos: " << missilePosition[0] << " " << missilePosition[1] << " " << missilePosition[2] << "\n";

		//ricavo direzione missile
		missileDirection = glm::normalize(missilePosition - oldMissilePosition);


		glm::mat4 out = glm::translate(glm::mat4(1.0), missilePosition) *
			glm::rotate(glm::mat4(1.0), yaw, glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0), pitch, glm::vec3(1, 0, 0));
		return out;
	}

	//ritorna dati 3 punti i parametri A,B,C dell'eq y = Ax^2 + Bx + C
	glm::vec3 CalcParabolaParam(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2)
	{
		float x1 = p0[0], x2 = p1[0], x3 = p2[0], y1 = p0[1], y2 = p1[1], y3 = p2[1];
		float denom = (x1 - x2) * (x1 - x3) * (x2 - x3);
		float A = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
		float B = (x3 * x3 * (y1 - y2) + x2 * x2 * (y3 - y1) + x1 * x1 * (y2 - y3)) / denom;
		float C = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;

		return glm::vec3(A, B, C);
	}

	glm::mat4 getDefaultMissileWorldMatrix()
	{
		return glm::translate(glm::mat4(1.0), missileStartPostion) *
			glm::rotate(glm::mat4(1.0), 0.0f, glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0), 0.0f, glm::vec3(1, 0, 0));
	}


	float getTerrainHeigh(glm::vec3 position)
	{
		auto relativePostion = position - terrainPosition;
		float bestDistance = 1000000000;
		float h;

		for (auto vertex : Terrain.vertices)
		{
			auto relativeVertex = vertex.pos * terrainScale;
			relativeVertex.y = 0;

			auto distance = glm::distance(relativeVertex, relativePostion);
			if (distance < bestDistance) {
				bestDistance = distance;
				h = vertex.pos.y * terrainScale;
			}
			if (distance < 0.1f) break;

		}
		return h;
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		void* data;
		UniformBufferObject ubo{};

		static auto startTime = std::chrono::high_resolution_clock::now();
		static float lastTime = 0.0f;

		static float spaceToggleTimer = 0.0f;

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		float deltaT = time - lastTime;
		lastTime = time;

		/************************************************************************************************************/
		//  input

		static double old_xpos = 0, old_ypos = 0;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		double m_dx = xpos - old_xpos;
		double m_dy = ypos - old_ypos;
		old_xpos = xpos; old_ypos = ypos;


		spaceToggleTimer = spaceToggleTimer > 0 ? spaceToggleTimer - deltaT : 0; //this timer is used to block multiplte inputs on spacebar

		glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			CamAng.y += m_dx * ROT_SPEED / MOUSE_RES;
			CamAng.x += m_dy * ROT_SPEED / MOUSE_RES;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			if (aimMode)
				missileDestination.x -= deltaT * AIM_SPEED;
			else
				CamAng.y += deltaT * ROT_SPEED;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			if (aimMode)
				missileDestination.x += deltaT * AIM_SPEED;
			else
				CamAng.y -= deltaT * ROT_SPEED;
		}
		if (glfwGetKey(window, GLFW_KEY_UP)) {
			if (aimMode)
				missileDestination.z -= deltaT * AIM_SPEED;
			else
				CamAng.x -= deltaT * ROT_SPEED;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			if (aimMode)
				missileDestination.z += deltaT * AIM_SPEED;
			else
				CamAng.x += deltaT * ROT_SPEED;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) && spaceToggleTimer == 0.0f)
		{
			// reset current position before starting 
			if (!isSimulationRunning) {
				missilePosition = missileStartPostion;
				std::cout << "Missile current position resetted\n";
				aimMode = false;
			}

			spaceToggleTimer += .5f; // delay until next input
			isSimulationRunning = !isSimulationRunning;
			std::cout << "Simulation " << (isSimulationRunning ? "started" : "reset") << "\n";

			// on start calculate the destination height
			if (isSimulationRunning)
			{
				missileDestination.y = getTerrainHeigh(missileDestination) - 1; // remove a little to have the missile pass a bit the terrain
				std::cout << "Computed destination height: " << missileDestination.y << "\n";
			}
		}

		if (glfwGetKey(window, GLFW_KEY_M) && spaceToggleTimer == 0.0f && !isSimulationRunning)
		{
			spaceToggleTimer += .5f; // delay until next input
			aimMode = !aimMode;
			std::cout << "Aim mode " << (aimMode ? "enabled" : "disabled") << "\n";
		}

		/************************************************************************************************************/
		//  shared matrices

		glm::vec3 aroundRotation = tpcRadius * glm::vec3(glm::cos(CamAng.y) * glm::sin(CamAng.x), glm::cos(CamAng.x), glm::sin(CamAng.y) * glm::sin(CamAng.x));


		if (aimMode)
		{
			ubo.view = glm::lookAt(
				terrainPosition + glm::vec3(0, 270, 0),
				terrainPosition + glm::vec3(0, 1, 0), // missle center is at bottom so we need to offset it a bit
				glm::vec3(0, 0, -1) // up
			);

		}
		else {
			ubo.view = glm::lookAt(
				missilePosition + aroundRotation,
				missilePosition + glm::vec3(0, 1, 0), // missle center is at bottom so we need to offset it a bit
				glm::vec3(0, 1, 0) // up
			);
		}

		ubo.proj = glm::perspective(glm::radians(45.0f),
			swapChainExtent.width / (float)swapChainExtent.height,
			0.1f, 1000.0f);

		ubo.proj[1][1] *= -1;

		/************************************************************************************************************/
		// missile

		missileDestination.x = std::clamp(missileDestination.x, terrainPosition.x - AIM_DISTANCE, terrainPosition.x + AIM_DISTANCE);
		missileDestination.z = std::clamp(missileDestination.z, terrainPosition.z - AIM_DISTANCE, terrainPosition.z + AIM_DISTANCE);

		if (isSimulationRunning)
		{
			ubo.model = getMissileWorldMatrix(missileStartPostion, missileDestination, deltaT); // calculate moving position

			if (glm::distance(missilePosition, missileDestination) < 0.3f)
			{
				isSimulationRunning = false;
				std::cout << "Simulation ended\n";
			}
		}
		else
		{
			ubo.model = getDefaultMissileWorldMatrix(); // reset position
			missilePosition = missileStartPostion;
		}

		ubo.model = glm::scale(ubo.model, missileScale * glm::vec3(1));

		ubo.missLightDir = -missileDirection;
		ubo.missLightPos = missilePosition;


		vkMapMemory(device, MissileDs.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, MissileDs.uniformBuffersMemory[0][currentImage]);

		/************************************************************************************************************/
		// aim dot
		ubo.model = glm::translate(glm::mat4(1.0f), missileDestination + glm::vec3(0, aimMode ? 100 : -1000,0));
		ubo.model = glm::scale(ubo.model, .005f * glm::vec3(1));

		// Here is where you actually update your uniforms
		vkMapMemory(device, AimItemDs.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, AimItemDs.uniformBuffersMemory[0][currentImage]);

		/************************************************************************************************************/
		// terrain
		ubo.model = glm::translate(glm::mat4(1.0f), terrainPosition);
		ubo.model = glm::scale(ubo.model, terrainScale * glm::vec3(1));

		// Here is where you actually update your uniforms
		vkMapMemory(device, TerrainDs.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, TerrainDs.uniformBuffersMemory[0][currentImage]);

		/************************************************************************************************************/
		//  skybox
		SkyboxBufferObject sbo;

		glm::mat3 CamDir = glm::mat3(glm::rotate(glm::mat4(1.0f), -CamAng.y, glm::vec3(0.0f, 1.0f, 0.0f))) *
				glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f) + CamAng.x, glm::vec3(1.0f, 0.0f, 0.0f)));

		sbo.model = glm::scale(glm::mat4(1.0f), 1.0f * glm::vec3(1));
		sbo.view = glm::mat4(1.0f);
		sbo.proj = ubo.proj * glm::transpose(glm::mat4(CamDir));

		vkMapMemory(device, skyboxDs.uniformBuffersMemory[0][currentImage], 0, sizeof(sbo), 0, &data);
		memcpy(data, &sbo, sizeof(sbo));
		vkUnmapMemory(device, skyboxDs.uniformBuffersMemory[0][currentImage]);
	}
};

// This is the main: probably you do not need to touch this!
int main() {
	MissileSimulator app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}