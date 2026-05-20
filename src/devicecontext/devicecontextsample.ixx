module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11.h>
#include <dxgi1_2.h>

export module devicecontextsample;

import std;
import shared;

export namespace DeviceContextSample
{
	class SampleApp final : public Shared::Window
	{
	public:
		SampleApp()
			: Shared::Window{ true },
			dpi(GetDpi())
		{
			CreateDeviceIndependentResources();
			CreateDeviceResources();
		}

		auto OnMessage(this auto&& self, const Shared::Messages::Paint&) -> Win32::LRESULT
		{
			auto paint = PaintScope{ self.GetHandle() };
			if (not self.IsIconic())
				self.Render();
			return 0;
		}

		auto OnMessage(this auto&& self, const Shared::Messages::Size& msg) -> Win32::LRESULT
		{
			auto width = static_cast<std::uint32_t>(Win32::LoWord(msg.LParam));
			auto height = static_cast<std::uint32_t>(Win32::HiWord(msg.LParam));
			if (width != 0 and height != 0)
				self.Resize(width, height);
			return 0;
		}

		auto OnMessage(this auto&& self, const Shared::Messages::DisplayChange&) -> Win32::LRESULT
		{
			self.Invalidate();
			return 0;
		}

		auto OnMessage(this auto&& self, const Shared::Messages::DpiChanged& msg) -> Win32::LRESULT
		{
			self.dpi = self.GetDpi();
			auto* suggestedRect = reinterpret_cast<const Win32::RECT*>(msg.LParam);

			auto success = 
				Win32::SetWindowPos(
					self.GetHandle(),
					nullptr,
					suggestedRect->left,
					suggestedRect->top,
					suggestedRect->right - suggestedRect->left,
					suggestedRect->bottom - suggestedRect->top,
					Win32::SetWindowPosFlags::NoZOrder | Win32::SetWindowPosFlags::NoActivate
				);
			if (not success)
				throw Shared::Win32Error{ Win32::GetLastError(), "SetWindowPos() failed" };

			self.RecreateTargetBitmap();
			self.Invalidate();
			return 0;
		}

	private:
		struct PaintScope
		{
			explicit PaintScope(Win32::HWND handle)
				: Handle(handle)
			{
				if (not Win32::BeginPaint(Handle, &Paint))
					throw Shared::Win32Error{ Win32::GetLastError(), "BeginPaint() failed" };
			}

			~PaintScope()
			{
				Win32::EndPaint(Handle, &Paint);
			}

			Win32::HWND Handle{};
			Win32::PAINTSTRUCT Paint{};
		};

		void CreateDeviceIndependentResources(this auto&& self)
		{
			auto hr = Shared::HResult{
				D2D1CreateFactory(
					D2D1_FACTORY_TYPE_SINGLE_THREADED,
					self.d2dFactory.ReleaseAndGetAddressOf()
				)
			};
			if (not hr)
				throw Shared::ComError{ hr, "D2D1CreateFactory() failed" };
		}

		void CreateDeviceResources(this auto&& self)
		{
			if (self.d2dContext)
				return;

			constexpr auto creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
			constexpr auto featureLevels = std::array{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0
			};

			auto featureLevel = D3D_FEATURE_LEVEL{};
			auto hr = Shared::HResult{
				D3D11CreateDevice(
					nullptr,
					D3D_DRIVER_TYPE_HARDWARE,
					nullptr,
					creationFlags,
					featureLevels.data(),
					static_cast<unsigned int>(featureLevels.size()),
					D3D11_SDK_VERSION,
					self.d3dDevice.ReleaseAndGetAddressOf(),
					&featureLevel,
					self.d3dContext.ReleaseAndGetAddressOf()
				)
			};
			if (not hr)
				throw Shared::ComError{ hr, "D3D11CreateDevice() failed" };

			self.dxgiDevice = self.d3dDevice.As<IDXGIDevice>();

			hr = Shared::HResult{
				self.d2dFactory->CreateDevice(
					self.dxgiDevice.Get(),
					self.d2dDevice.ReleaseAndGetAddressOf()
				)
			};
			if (not hr)
				throw Shared::ComError{ hr, "ID2D1Factory1::CreateDevice() failed" };

			hr = Shared::HResult{
				self.d2dDevice->CreateDeviceContext(
					D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
					self.d2dContext.ReleaseAndGetAddressOf()
				)
			};
			if (not hr)
				throw Shared::ComError{ hr, "ID2D1Device::CreateDeviceContext() failed" };

			self.CreateSwapChain();
			self.CreateTargetBitmap();
			self.CreateBrushes();
		}

		void CreateSwapChain(this auto&& self)
		{
			auto adapter = Shared::Ptr<IDXGIAdapter>{};
			auto hr = Shared::HResult{ self.dxgiDevice->GetAdapter(adapter.ReleaseAndGetAddressOf()) };
			if (not hr)
				throw Shared::ComError{ hr, "IDXGIDevice::GetAdapter() failed" };

			auto factory = Shared::Ptr<IDXGIFactory2>{};
			hr = adapter->GetParent(
				__uuidof(IDXGIFactory2),
				reinterpret_cast<void**>(factory.ReleaseAndGetAddressOf())
			);
			if (not hr)
				throw Shared::ComError{ hr, "IDXGIAdapter::GetParent() failed" };

			auto swapChainDesc = DXGI_SWAP_CHAIN_DESC1{
				.Width = 0,
				.Height = 0,
				.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
				.Stereo = false,
				.SampleDesc = DXGI_SAMPLE_DESC{ .Count = 1, .Quality = 0 },
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = 2,
				.Scaling = DXGI_SCALING_STRETCH,
				.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
				.AlphaMode = DXGI_ALPHA_MODE_IGNORE,
				.Flags = 0
			};

			hr = factory->CreateSwapChainForHwnd(
				self.d3dDevice.Get(),
				self.GetHandle(),
				&swapChainDesc,
				nullptr,
				nullptr,
				self.swapChain.ReleaseAndGetAddressOf()
			);
			if (not hr)
				throw Shared::ComError{ hr, "IDXGIFactory2::CreateSwapChainForHwnd() failed" };
		}

		void CreateTargetBitmap(this auto&& self)
		{
			auto backBuffer = Shared::Ptr<IDXGISurface>{};
			auto hr = Shared::HResult{
				self.swapChain->GetBuffer(
					0,
					__uuidof(IDXGISurface),
					reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf())
				)
			};
			if (not hr)
				throw Shared::ComError{ hr, "IDXGISwapChain::GetBuffer() failed" };

			auto bitmapProperties = 
				D2D1_BITMAP_PROPERTIES1{
					.pixelFormat = D2D1_PIXEL_FORMAT{
						.format = DXGI_FORMAT_B8G8R8A8_UNORM,
						.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED
					},
					.dpiX = static_cast<float>(self.dpi),
					.dpiY = static_cast<float>(self.dpi),
					.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
					.colorContext = nullptr
				};

			hr = self.d2dContext->CreateBitmapFromDxgiSurface(
				backBuffer.Get(),
				&bitmapProperties,
				self.targetBitmap.ReleaseAndGetAddressOf()
			);
			if (not hr)
				throw Shared::ComError{ hr, "ID2D1DeviceContext::CreateBitmapFromDxgiSurface() failed" };

			self.d2dContext->SetTarget(self.targetBitmap.Get());
		}

		void CreateBrushes(this auto&& self)
		{
			auto hr = Shared::HResult{
				self.d2dContext->CreateSolidColorBrush(
					D2D1::ColorF{ D2D1::ColorF::LightSlateGray },
					self.gridBrush.ReleaseAndGetAddressOf()
				)
			};
			if (not hr)
				throw Shared::ComError{ hr, "CreateSolidColorBrush() failed [grid]" };

			hr = Shared::HResult{
				self.d2dContext->CreateSolidColorBrush(
					D2D1::ColorF{ D2D1::ColorF::CornflowerBlue },
					self.accentBrush.ReleaseAndGetAddressOf()
				)
			};
			if (not hr)
				throw Shared::ComError{ hr, "CreateSolidColorBrush() failed [accent]" };
		}

		void DiscardDeviceResources(this auto&& self)
		{
			if (self.d2dContext)
				self.d2dContext->SetTarget(nullptr);
			self.accentBrush.reset();
			self.gridBrush.reset();
			self.targetBitmap.reset();
			self.swapChain.reset();
			self.d2dContext.reset();
			self.d2dDevice.reset();
			self.dxgiDevice.reset();
			self.d3dContext.reset();
			self.d3dDevice.reset();
		}

		void RecreateTargetBitmap(this auto&& self)
		{
			if (not self.d2dContext or not self.swapChain)
				return;
			self.d2dContext->SetTarget(nullptr);
			self.targetBitmap.reset();
			self.CreateTargetBitmap();
		}

		void Resize(this auto&& self, std::uint32_t width, std::uint32_t height)
		{
			if (not self.swapChain)
				return;

			self.d2dContext->SetTarget(nullptr);
			self.targetBitmap.reset();

			auto hr = Shared::HResult{
				self.swapChain->ResizeBuffers(
					0,
					width,
					height,
					DXGI_FORMAT_UNKNOWN,
					0
				)
			};
			if (not hr)
				throw Shared::ComError{ hr, "IDXGISwapChain::ResizeBuffers() failed" };

			self.CreateTargetBitmap();
			self.Invalidate();
		}

		void Render(this auto&& self)
		{
			self.CreateDeviceResources();

			self.d2dContext->BeginDraw();
			self.d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
			self.d2dContext->Clear(D2D1::ColorF{ D2D1::ColorF::White });

			self.DrawGrid();
			self.DrawGeometry();

			auto hr = Shared::HResult{ self.d2dContext->EndDraw() };
			if (not hr)
			{
				if (hr.Code == D2DERR_RECREATE_TARGET)
				{
					self.DiscardDeviceResources();
					self.CreateDeviceResources();
					return;
				}
				throw Shared::ComError{ hr, "ID2D1DeviceContext::EndDraw() failed" };
			}

			hr = Shared::HResult{ self.swapChain->Present(1, 0) };
			if (not hr)
				throw Shared::ComError{ hr, "IDXGISwapChain::Present() failed" };
		}

		void DrawGrid(this auto&& self)
		{
			auto size = self.d2dContext->GetSize();
			for (auto x = 0.0f; x < size.width; x += 24.0f)
			{
				self.d2dContext->DrawLine(
					D2D1::Point2F(x, 0.0f),
					D2D1::Point2F(x, size.height),
					self.gridBrush.Get(),
					0.5f
				);
			}
			for (auto y = 0.0f; y < size.height; y += 24.0f)
			{
				self.d2dContext->DrawLine(
					D2D1::Point2F(0.0f, y),
					D2D1::Point2F(size.width, y),
					self.gridBrush.Get(),
					0.5f
				);
			}
		}

		void DrawGeometry(this auto&& self)
		{
			auto size = self.d2dContext->GetSize();
			auto center = D2D1::Point2F(size.width / 2.0f, size.height / 2.0f);
			auto rectangle = D2D1::RectF(
				center.x - 120.0f,
				center.y - 80.0f,
				center.x + 120.0f,
				center.y + 80.0f
			);
			auto ellipse = D2D1_ELLIPSE{
				.point = center,
				.radiusX = 72.0f,
				.radiusY = 72.0f
			};

			self.d2dContext->FillRectangle(&rectangle, self.gridBrush.Get());
			self.d2dContext->DrawRectangle(&rectangle, self.accentBrush.Get(), 3.0f);
			self.d2dContext->DrawEllipse(&ellipse, self.accentBrush.Get(), 4.0f);
		}

		std::uint32_t dpi = 96;
		Shared::Ptr<ID2D1Factory1> d2dFactory; // Device-independent; keep across DPI changes, resize, minimisation, and device loss.
		Shared::Ptr<ID3D11Device> d3dDevice; // Device-dependent; recreate on device loss only, not DPI changes, resize, or minimisation.
		Shared::Ptr<ID3D11DeviceContext> d3dContext; // Tied to d3dDevice; recreate with the device on device loss only.
		Shared::Ptr<IDXGIDevice> dxgiDevice; // Interface on d3dDevice; recreate with the device on device loss only.
		Shared::Ptr<ID2D1Device> d2dDevice; // Wraps dxgiDevice; recreate with the D3D device on device loss only.
		Shared::Ptr<ID2D1DeviceContext> d2dContext; // Device-dependent; recreate on device loss, but keep across DPI changes, resize, and minimisation.
		Shared::Ptr<IDXGISwapChain1> swapChain; // Window/device-dependent; recreate on device loss or window replacement, resize buffers on resize, keep while minimised.
		Shared::Ptr<ID2D1Bitmap1> targetBitmap; // Back-buffer target; recreate after DPI changes, swap-chain resize, and device loss; keep while minimised.
		Shared::Ptr<ID2D1SolidColorBrush> gridBrush; // Device-context resource; recreate on device loss only, not DPI changes, resize, or minimisation.
		Shared::Ptr<ID2D1SolidColorBrush> accentBrush; // Device-context resource; recreate on device loss only, not DPI changes, resize, or minimisation.
	};

	void Run()
	{
		auto app = SampleApp{};
		auto msg = Win32::MSG{};
		while (msg.message != Win32::Messages::Quit)
		{
			if (Win32::PeekMessageW(&msg, nullptr, 0, 0, Win32::PeekMessageFlags::Remove))
			{
				Win32::TranslateMessage(&msg);
				Win32::DispatchMessageW(&msg);
			}
			else
			{
				if (not Win32::WaitMessage())
					throw Shared::Win32Error{ Win32::GetLastError(), "WaitMessage() failed" };
			}
		}
	}
}
