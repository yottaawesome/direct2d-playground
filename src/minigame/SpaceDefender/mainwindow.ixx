export module spacedefender:spacedefenderwindow;
import std;
import shared;

export namespace SpaceDefender
{
	class MainWindow final : public Shared::GameWindow
	{
	public:
		MainWindow(Shared::GameWindow::OnEvent handlers)
			: Shared::GameWindow{ std::move(handlers), Shared::NoInit }
		{
			Init();
		}
		auto GetCreateWindowParams(this auto&& self) noexcept -> CreateWindowParams
		{
			auto params = self.GameWindow::GetCreateWindowParams();
			params.WindowName = L"Space Defender";
			return params;
		}
	private:
	};
}
