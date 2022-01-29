module;

#include <string>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wrl/client.h>

export module spaceinvaders.engine.d2dfactory;
import spaceinvaders.engine.renderer;

export namespace SpaceInvaders::Engine
{
	class D2DFactory
	{
		public:
			virtual ~D2DFactory();
			D2DFactory();
			D2DFactory(const D2D1_FACTORY_TYPE factoryType);

		public:
			virtual operator ID2D1Factory*() const noexcept;
			virtual operator bool() const noexcept;

		public:
			virtual void Close();
			virtual Renderer BindRenderTarget(
				const HWND hwnd,
				const UINT32 width,
				const UINT32 height
			);
			virtual void Initialise(
				const D2D1_FACTORY_TYPE factoryType = D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED
			);
			[[nodiscard]] virtual D2D1_FACTORY_TYPE GetFactoryType() const noexcept;
			[[nodiscard]] virtual Microsoft::WRL::ComPtr<ID2D1Factory> Get() const noexcept;
			[[nodiscard]] virtual ID2D1Factory* GetRaw() const noexcept;
			
		protected:
			Microsoft::WRL::ComPtr<ID2D1Factory> m_D2DFactory;
			D2D1_FACTORY_TYPE m_factoryType;
	};
}