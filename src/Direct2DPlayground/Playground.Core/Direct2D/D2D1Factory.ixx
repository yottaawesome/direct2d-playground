module;

#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wrl/client.h>

export module core.direct2d.d2d1factory;

export namespace Core::Direct2D::D2D1Factory
{
	// Represents an ID2D1Factory. See also https://docs.microsoft.com/en-us/windows/win32/api/d2d1/nn-d2d1-id2d1factory
	// Make sure to link to #pragma comment(lib, "d2d1.lib")
	class D2D1Factory
	{
		public:
			virtual ~D2D1Factory();
			D2D1Factory();
			D2D1Factory(const D2D1_FACTORY_TYPE factoryType);

		public:
			virtual void Close();
			virtual Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> CreateHwndRenderTarget(
				const HWND hwnd,
				const UINT32 width,
				const UINT32 height
			);
			virtual void Initialise(const D2D1_FACTORY_TYPE factoryType);
			virtual D2D1_FACTORY_TYPE GetFactoryType() const noexcept;

		protected:
			Microsoft::WRL::ComPtr<ID2D1Factory> m_pDirect2dFactory;
			D2D1_FACTORY_TYPE m_factoryType;
	};
}
