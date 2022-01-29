module;

#include <functional>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wrl/client.h>

export module spaceinvaders.rendering.renderer;

export namespace SpaceInvaders::Rendering
{
	class Renderer
	{
		public:
			using DrawFunction = std::function<void()>;

		public:
			virtual ~Renderer();
			Renderer();
			Renderer(const D2D1_FACTORY_TYPE factoryType);

		public:
			virtual operator bool() const noexcept;
			virtual operator ID2D1Factory* () const noexcept;

		public:
			virtual void Close();
			virtual void BindRenderTarget(
				const HWND hwnd,
				const UINT32 width,
				const UINT32 height
			);
			virtual void Initialise(
				const D2D1_FACTORY_TYPE factoryType = D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED
			);
			virtual void Resize(const unsigned width, const unsigned height);
			[[nodiscard]] virtual Microsoft::WRL::ComPtr<ID2D1Bitmap> LoadBitmap(const std::wstring& path);
			virtual void Clear(const D2D1::ColorF& color);
			virtual void Draw(const DrawFunction& DrawToTarget);

		// Getters, setters
		public:
			[[nodiscard]] virtual D2D1_FACTORY_TYPE GetFactoryType() const noexcept;
			[[nodiscard]] virtual Microsoft::WRL::ComPtr<ID2D1Factory> Get() const noexcept;
			[[nodiscard]] virtual ID2D1Factory* GetRaw() const noexcept;
			[[nodiscard]] virtual ID2D1HwndRenderTarget* GetRawRenderTarget() const noexcept;
			
		protected:
			Microsoft::WRL::ComPtr<ID2D1Factory> m_D2DFactory;
			Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_hwndRenderTarget;
			D2D1_FACTORY_TYPE m_factoryType;
	};
}