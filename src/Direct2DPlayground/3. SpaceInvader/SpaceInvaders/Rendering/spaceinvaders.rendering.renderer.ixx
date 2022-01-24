module;

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
			virtual ~Renderer();
			Renderer();
			Renderer(const D2D1_FACTORY_TYPE factoryType);

		public:
			virtual operator bool() const noexcept;
			virtual operator ID2D1Factory* () const noexcept;

		public:
			virtual void Close();
			virtual Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> CreateHwndRenderTarget(
				const HWND hwnd,
				const UINT32 width,
				const UINT32 height
			);
			virtual void Initialise(const D2D1_FACTORY_TYPE factoryType);

			// Getters, setters
		public:
			virtual D2D1_FACTORY_TYPE GetFactoryType() const noexcept;
			virtual Microsoft::WRL::ComPtr<ID2D1Factory> Get() const noexcept;
			virtual ID2D1Factory* GetRaw() const noexcept;

		protected:
			Microsoft::WRL::ComPtr<ID2D1Factory> m_pDirect2dFactory;
			D2D1_FACTORY_TYPE m_factoryType;
	};
}