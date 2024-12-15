#pragma once
#include <GEngine/Graphics/Graphics.h>


namespace Genesis
{
	class GContext
	{
		// Public methods
	public:
		void Initialize(chil::win::IWindow& window);

		// Getters
		inline int32_t GetWidth() const
		{
			return width;
		}

		inline int32_t GetHeight() const
		{
			return height;
		}
				
		inline Graphics& GetGraphics()
		{
			return *m_graphics;
		}
		// Private methods
	private:

		// Public variables
	public:

		// Private variables
	private:
		Graphics* m_graphics;
		int32_t width = 1920;
		int32_t height = 1080;

#pragma region Singleton
		// Singleton
	public:
		inline static GContext& Get()
		{
			static GContext* instance = new GContext();
			return *instance;
		}
		// Delete copy constructor and assignment operator
		GContext(const GContext&) = delete;
		GContext& operator=(const GContext&) = delete;
	private:
		GContext() = default;
		~GContext() = default;
	};
#pragma endregion
}
