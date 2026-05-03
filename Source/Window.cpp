#include "../Include/Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Starlight
{
	Starlight::Window::Window()
	{
		StarlightZoneScoped;

		window = nullptr;
		width = 0u;
		height = 0u;
		title = "Untitled";
	}

	StarlightErr Starlight::Window::Create(const uint16_t width, const uint16_t height, const std::string& title)
	{
		StarlightZoneScoped;

		this->width = width;
		this->height = height;
		this->title = title;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		if (!window)
		{
			Starlight:Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Failed to create window\n");
			return StarlightErr::Cannot_Init;
		}

		glfwSetWindowUserPointer(window, this);

		return StarlightErr::Success;
	}

	StarlightErr Starlight::Window::Destroy()
	{
		StarlightZoneScoped;

		if (!window)
		{
			Starlight::Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Cannot destroy window, window is null\n");
		}

		glfwDestroyWindow(window);
		window = nullptr;

		return StarlightErr::Success;
	}

	bool Starlight::Window::PollEvents()
	{
		StarlightZoneScoped;

		if (!window)
		{
			Starlight::Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Cannot poll events, window is null\n");
			return false;
		}

		if (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			return true;
		}

		return false;
	}

	void Starlight::Window::SetWindowTitle(const std::string& title)
	{
		StarlightZoneScoped;

		this->title = title;

		glfwSetWindowTitle(window, title.c_str());
	}

	const std::string& Starlight::Window::GetWindowTitle() const
	{
		StarlightZoneScoped;

		return title;
	}

	void Starlight::Window::SetWidth(const uint16_t width)
	{
		StarlightZoneScoped;

		this->width = width;

		glfwSetWindowSize(window, width, height);
	}

	const uint16_t Starlight::Window::GetWidth() const
	{
		StarlightZoneScoped;

		return width;
	}

	void Starlight::Window::SetHeight(const uint16_t height)
	{
		StarlightZoneScoped;

		this->height = height;

		glfwSetWindowSize(window, width, height);
	}

	const uint16_t Starlight::Window::GetHeight() const
	{
		StarlightZoneScoped;

		return height;
	}

	void Starlight::Window::SetIcon(const std::filesystem::path& path)
	{
		StarlightZoneScoped;

		if (!std::filesystem::exists(path))
		{
			Starlight::Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Cannot set window icon, file does not exist\n");
			return;
		}
		
		int x, y, numChannels;
		uint8_t* data = stbi_load((const char*)path.generic_u8string().c_str(), &x, &y, &numChannels, 4);

		if (!data)
		{
			Starlight::Logger::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Did not load image from path %s", path.c_str());
		}

		GLFWimage icon;
		icon.pixels = data;
		icon.width = x;
		icon.height = y;

		glfwSetWindowIcon(window, 1, &icon);

		stbi_image_free(data);
	}
}