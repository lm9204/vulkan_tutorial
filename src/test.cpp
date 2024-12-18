#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp> // to_string 활성화
#include <iostream>

int main() {
    // GLFW 초기화
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Vulkan 지원 활성화
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Vulkan Instance 생성
    VkInstance instance;
	std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,          // 표준 표면 확장
        "VK_MVK_macos_surface",                 // macOS 표면 확장
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME // Portability 확장
    };

    // VkInstanceCreateInfo 초기화
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // Portability 활성화
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
	
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		std::cerr << "Failed to create Vulkan instance! Error code: " << result << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
		return -1;
	}

    // GLM 사용 예제
    glm::vec3 position(0.0f, 0.0f, 0.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

    std::cout << "GLM Matrix:\n"
              << glm::to_string(model) << std::endl;

    // 메인 루프
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // 리소스 정리
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}