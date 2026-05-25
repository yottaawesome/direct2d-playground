#include <box2d/box2d.h>

#pragma comment(lib, "d2d1.lib")

import std;
import shared;

namespace
{
	constexpr auto WindowClassName = L"Box2DDirect2DDemoWindow";
	constexpr auto SimulationTimerId = Win32::UINT_PTR{1};
	constexpr auto SimulationTimerMs = Win32::UINT{16};
	constexpr auto TimeStep = 1.0f / 60.0f;
	constexpr auto SubStepCount = 4;
	constexpr auto PixelsPerMeter = 32.0f;
	constexpr auto GroundBottomMargin = 80.0f;
	constexpr auto BoxHalfExtent = 1.0f;
	constexpr auto GroundHalfWidth = 9.0f;
	constexpr auto GroundHalfHeight = 0.5f;

	void CheckHr(Win32::HRESULT hr, char const* message)
	{
		if (Win32::Failed(hr))
			throw std::runtime_error{ message };
	}

	class Box2DDemoApp
	{
	public:
		Box2DDemoApp()
		{
			CreatePhysicsWorld();
			CheckHr(
				D2D1::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, direct2dFactory.ReleaseAndGetAddressOf()),
				"D2D1CreateFactory() failed"
			);
		}

		Box2DDemoApp(Box2DDemoApp const&) = delete;
		auto operator=(Box2DDemoApp const&) -> Box2DDemoApp& = delete;

		~Box2DDemoApp()
		{
			if (b2World_IsValid(world))
				b2DestroyWorld(world);
		}

		auto Run() -> int
		{
			RegisterWindowClass();

			window = Win32::CreateWindowExW(
				0,
				WindowClassName,
				L"Box2D 3 + Direct2D demo",
				Win32::WindowStyles::OverlappedWindow,
				Win32::CwUseDefault,
				Win32::CwUseDefault,
				1024,
				720,
				nullptr,
				nullptr,
				Win32::GetModuleHandleW(nullptr),
				this
			);
			if (!window)
				throw std::runtime_error{ std::format("CreateWindowExW() failed {}", Win32::GetLastError()) };

			Win32::ShowWindow(window, Win32::ShowWindowFlags::ShowDefault);
			Win32::UpdateWindow(window);

			auto msg = Win32::MSG{};
			while (Win32::GetMessageW(&msg, nullptr, 0, 0) > 0)
			{
				Win32::TranslateMessage(&msg);
				Win32::DispatchMessageW(&msg);
			}

			return static_cast<int>(msg.wParam);
		}

	private:
		void RegisterWindowClass()
		{
			auto wc = Win32::WNDCLASSEXW{
				.cbSize = sizeof(wc),
				.style = Win32::ClassStyles::HRedraw | Win32::ClassStyles::VRedraw,
				.lpfnWndProc = &Box2DDemoApp::WindowProc,
				.hInstance = Win32::GetModuleHandleW(nullptr),
				.hCursor = Win32::LoadCursorW(nullptr, Win32::IdcArrow),
				.lpszClassName = WindowClassName
			};

			if (!Win32::RegisterClassExW(&wc))
				throw std::runtime_error{ "RegisterClassExW() failed" };
		}

		void CreatePhysicsWorld()
		{
			auto worldDef = b2DefaultWorldDef();
			worldDef.gravity = { 0.0f, -10.0f };
			world = b2CreateWorld(&worldDef);

			auto groundBodyDef = b2DefaultBodyDef();
			groundBodyDef.position = { 0.0f, 0.0f };
			groundBody = b2CreateBody(world, &groundBodyDef);

			auto groundShape = b2MakeBox(GroundHalfWidth, GroundHalfHeight);
			auto groundShapeDef = b2DefaultShapeDef();
			groundShapeDef.material.friction = 0.8f;
			b2CreatePolygonShape(groundBody, &groundShapeDef, &groundShape);

			auto boxBodyDef = b2DefaultBodyDef();
			boxBodyDef.type = b2_dynamicBody;
			boxBodyDef.position = { 0.0f, 5.0f };
			boxBodyDef.rotation = b2MakeRot(0.35f);
			boxBody = b2CreateBody(world, &boxBodyDef);

			auto boxShape = b2MakeBox(BoxHalfExtent, BoxHalfExtent);
			auto boxShapeDef = b2DefaultShapeDef();
			boxShapeDef.density = 1.0f;
			boxShapeDef.material.friction = 0.45f;
			boxShapeDef.material.restitution = 0.2f;
			b2CreatePolygonShape(boxBody, &boxShapeDef, &boxShape);
			b2Body_SetAngularVelocity(boxBody, 1.0f);
		}

		void CreateDeviceResources()
		{
			if (renderTarget)
				return;

			auto rc = Win32::RECT{};
			Win32::GetClientRect(window, &rc);
			auto const width = static_cast<Win32::UINT32>(rc.right - rc.left);
			auto const height = static_cast<Win32::UINT32>(rc.bottom - rc.top);
			auto const size = D2D1::SizeU(width == 0 ? 1 : width, height == 0 ? 1 : height);

			CheckHr(
				direct2dFactory->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(window, size),
					renderTarget.ReleaseAndGetAddressOf()
				),
				"ID2D1Factory::CreateHwndRenderTarget() failed"
			);

			CheckHr(
				renderTarget->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::DarkSlateGray),
					groundBrush.ReleaseAndGetAddressOf()
				),
				"ID2D1HwndRenderTarget::CreateSolidColorBrush() failed for ground"
			);
			CheckHr(
				renderTarget->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
					boxBrush.ReleaseAndGetAddressOf()
				),
				"ID2D1HwndRenderTarget::CreateSolidColorBrush() failed for box"
			);
			CheckHr(
				renderTarget->CreateSolidColorBrush(
					D2D1::ColorF(D2D1::ColorF::LightSteelBlue, 0.35f),
					gridBrush.ReleaseAndGetAddressOf()
				),
				"ID2D1HwndRenderTarget::CreateSolidColorBrush() failed for grid"
			);
		}

		void DiscardDeviceResources()
		{
			gridBrush.reset();
			boxBrush.reset();
			groundBrush.reset();
			renderTarget.reset();
		}

		void StepSimulation()
		{
			b2World_Step(world, TimeStep, SubStepCount);

			auto const position = b2Body_GetPosition(boxBody);
			auto title = std::wstring{ L"Box2D 3 + Direct2D demo - box y = " } + std::to_wstring(position.y);
			Win32::SetWindowTextW(window, title.c_str());
			Win32::InvalidateRect(window, nullptr, Win32::False);
		}

		void Render()
		{
			CreateDeviceResources();

			renderTarget->BeginDraw();
			renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			DrawGrid();
			DrawBodyBox(groundBody, GroundHalfWidth, GroundHalfHeight, groundBrush.Get());
			DrawBodyBox(boxBody, BoxHalfExtent, BoxHalfExtent, boxBrush.Get());

			auto const hr = renderTarget->EndDraw();
			if (hr == D2D1::Error::RecreateTarget)
			{
				DiscardDeviceResources();
				return;
			}
			CheckHr(hr, "ID2D1HwndRenderTarget::EndDraw() failed");
		}

		void DrawGrid()
		{
			auto const size = renderTarget->GetSize();
			for (float x = 0.0f; x < size.width; x += PixelsPerMeter)
				renderTarget->DrawLine({ x, 0.0f }, { x, size.height }, gridBrush.Get(), 0.5f);

			for (float y = 0.0f; y < size.height; y += PixelsPerMeter)
				renderTarget->DrawLine({ 0.0f, y }, { size.width, y }, gridBrush.Get(), 0.5f);
		}

		void DrawBodyBox(b2BodyId body, float halfWidth, float halfHeight, D2D1::ID2D1Brush* brush)
		{
			auto const position = b2Body_GetPosition(body);
			auto const rotation = b2Body_GetRotation(body);
			auto const center = ToScreen(position);
			auto const rect = D2D1::RectF(
				center.x - halfWidth * PixelsPerMeter,
				center.y - halfHeight * PixelsPerMeter,
				center.x + halfWidth * PixelsPerMeter,
				center.y + halfHeight * PixelsPerMeter
			);

			renderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(
				-b2Rot_GetAngle(rotation) * 180.0f / B2_PI,
				center
			));
			renderTarget->FillRectangle(rect, brush);
			renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		}

		auto ToScreen(b2Vec2 worldPoint) const -> D2D1::D2D1_POINT_2F
		{
			auto const size = renderTarget->GetSize();
			return {
				size.width * 0.5f + worldPoint.x * PixelsPerMeter,
				size.height - GroundBottomMargin - worldPoint.y * PixelsPerMeter
			};
		}

		auto HandleMessage(Win32::UINT message, Win32::WPARAM wParam, Win32::LPARAM lParam) -> Win32::LRESULT
		{
			switch (message)
			{
			case Win32::Messages::NonClientCreate:
				Win32::SetTimer(window, SimulationTimerId, SimulationTimerMs, nullptr);
				return true;

			case Win32::Messages::Timer:
				if (wParam == SimulationTimerId)
					StepSimulation();
				return 0;

			case Win32::Messages::Size:
				if (renderTarget)
				{
					auto const width = Win32::LoWord(lParam);
					auto const height = Win32::HiWord(lParam);
					auto const hr = renderTarget->Resize(D2D1::SizeU(width, height));
					if (Win32::Failed(hr))
						DiscardDeviceResources();
				}
				return 0;

			case Win32::Messages::Paint:
			{
				auto paint = Win32::PAINTSTRUCT{};
				Win32::BeginPaint(window, &paint);
				Render();
				Win32::EndPaint(window, &paint);
				return 0;
			}

			case Win32::Messages::Destroy:
				Win32::KillTimer(window, SimulationTimerId);
				Win32::PostQuitMessage(0);
				return 0;

			default:
				return Win32::DefWindowProcW(window, message, wParam, lParam);
			}
		}

		static auto WindowProc(Win32::HWND hwnd, Win32::UINT message, Win32::WPARAM wParam, Win32::LPARAM lParam) -> Win32::LRESULT
		{
			auto app = reinterpret_cast<Box2DDemoApp*>(Win32::GetWindowLongPtrW(hwnd, Win32::Gwlp::UserData));
			if (message == Win32::Messages::NonClientCreate)
			{
				auto const createStruct = reinterpret_cast<Win32::CREATESTRUCTW const*>(lParam);
				app = static_cast<Box2DDemoApp*>(createStruct->lpCreateParams);
				app->window = hwnd;
				Win32::SetWindowLongPtrW(hwnd, Win32::Gwlp::UserData, reinterpret_cast<Win32::LONG_PTR>(app));
			}

			if (!app)
				return Win32::DefWindowProcW(hwnd, message, wParam, lParam);

			try
			{
				return app->HandleMessage(message, wParam, lParam);
			}
			catch (std::exception const& ex)
			{
				Win32::MessageBoxA(hwnd, ex.what(), "Box2D sample error", Win32::MessageBoxOptions::Critical | Win32::MessageBoxOptions::Ok);
				Win32::DestroyWindow(hwnd);
				return 0;
			}
		}

		Win32::HWND window{};
		Shared::Ptr<D2D1::ID2D1Factory> direct2dFactory;
		Shared::Ptr<D2D1::ID2D1HwndRenderTarget> renderTarget;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> groundBrush;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> boxBrush;
		Shared::Ptr<D2D1::ID2D1SolidColorBrush> gridBrush;
		b2WorldId world = b2_nullWorldId;
		b2BodyId groundBody = b2_nullBodyId;
		b2BodyId boxBody = b2_nullBodyId;
	};
}

auto main() -> int
try
{
	auto app = Box2DDemoApp{};
	return app.Run();
}
catch (std::exception const& ex)
{
	std::cerr << ex.what() << '\n';
	return 1;
}
