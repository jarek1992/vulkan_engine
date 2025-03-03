﻿// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_descriptors.h>

struct DeletionQueue {

	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		//reverse iterate the deletion queue to execute all the fucntions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call functors
		}

		deletors.clear();
	}
};

//hold the structures and commands to draw a given frame
struct FrameData {

	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	DeletionQueue _deletionQueue;
};

constexpr unsigned int FRAME_OVERLAP = 2;

struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

struct ComputeEffect {
	const char* name;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};

class VulkanEngine {
public:
	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1200 , 800 };

	struct SDL_Window* _window{ nullptr };

	VkInstance _instance; //vulkan library handle 
	VkDebugUtilsMessengerEXT _debug_messenger; //vulkan debug output handle
	VkPhysicalDevice _chosenGPU; //GPU chosen as the default device
	VkDevice _device; //vulkan device for commands

	FrameData _frames[FRAME_OVERLAP];

	FrameData& get_current_frame() { 
		return _frames[_frameNumber % FRAME_OVERLAP]; 
	};

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;
	
	VkSurfaceKHR _surface; //vulkan window surface
	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;
	
	//draw resources
	AllocatedImage _drawImage;
	AllocatedImage _depthImage;

	VkExtent2D _swapchainExtent;
	VkExtent2D _drawExtent;

	DescriptorAllocator globalDescriptorAllocator;

	VkPipeline _gradientPipeline;
	VkPipelineLayout _gradientPipelineLayout;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;

	DeletionQueue _mainDeletionQueue;

	//vma lib allocator
	VmaAllocator _allocator;

	VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;

	VkPipelineLayout _meshPipelineLayout;
	VkPipeline _meshPipeline;

	GPUMeshBuffers rectangle;

	//immediate submit structures
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;

	std::vector<ComputeEffect> backgroundEffects;

	int currentBackgroundEffect {0};

	static VulkanEngine& Get();

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();
	void draw_background(VkCommandBuffer cmd);
	void draw_geometry(VkCommandBuffer cmd);
	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);

	//run main loop
	void run();

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
	
	GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	void destroy_buffer(const AllocatedBuffer& buffet);

private:
	void init_vulkan();
	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
	void init_swapchain();
	void init_commands();
	void init_background_pipeline();
	void init_pipelines();
	void init_triangle_pipeline();
	void init_mesh_pipeline();
	void init_descriptors();
	void init_sync_structures();
	void init_imgui();
};
