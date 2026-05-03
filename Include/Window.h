#ifndef __WINDOW_H__
#define __WINDOW_H__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../Include/Profiler.h"

#include <filesystem>
#include <string>

namespace Starlight
{
	class Window
	{
	public:

		Window();

		StarlightErr Create(const uint16_t width, const uint16_t height, const std::string& title);
		StarlightErr Destroy();

		bool PollEvents();

		void SetWindowTitle(const std::string& title);
		const std::string& GetWindowTitle() const;

		void SetWidth(const uint16_t width);
		const uint16_t GetWidth() const;

		void SetHeight(const uint16_t height);
		const uint16_t GetHeight() const;

		void SetIcon(const std::filesystem::path& path);

	private:

	private:
				
		GLFWwindow* window;
		
		std::string title;

		uint16_t width;
		uint16_t height;

	};
}

#endif
