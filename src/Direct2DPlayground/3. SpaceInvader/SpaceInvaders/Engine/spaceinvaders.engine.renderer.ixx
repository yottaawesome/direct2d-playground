module;

#include <functional>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wrl/client.h>

export module spaceinvaders.engine.renderer;

export namespace SpaceInvaders::Engine
{
	class Renderer
	{
		public:
			using DrawFunction = std::function<void()>;

		public:
			virtual ~Renderer();
			Renderer();
			Renderer(
				const Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget>& hwndRenderTarget
			);

			Renderer(const Renderer&) = delete;
			virtual Renderer& operator=(const Renderer&) = delete;

			Renderer(Renderer&& other) noexcept;
			virtual Renderer& operator=(Renderer&&) noexcept;

		public:
			virtual operator bool() const noexcept;

		public:
			virtual void Close();
			virtual void Resize(const unsigned width, const unsigned height);
			[[nodiscard]] virtual Microsoft::WRL::ComPtr<ID2D1Bitmap> LoadBitmap(const std::wstring& path);
			virtual void Clear(const D2D1::ColorF& color);
			virtual void Draw(const DrawFunction& DrawToTarget);

		// Getters, setters
		public:
			[[nodiscard]] virtual ID2D1HwndRenderTarget* GetRaw() const noexcept;
			
		protected:
			Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_hwndRenderTarget;
	};
}