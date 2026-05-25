export module spacedefender:input;
import std;
import shared;

export namespace SpaceDefender
{
	struct KeyPressState
	{
		bool Previous = false;
		bool Current = false;
		bool ChangeWasChecked = false;
	};
	using Array = std::array<KeyPressState, 256>;

	class InputState
	{
	public:
		// Set the state of the button to pressed and return whether it was previously pressed.
		auto WasPressed(this auto&& self, Win32::WPARAM key) -> bool
		{
			if (key >= self.Keys.size())
				return false;
			self.Keys[key].Previous = std::exchange(self.Keys[key].Current, true);
			self.Keys[key].ChangeWasChecked = false;
			return self.Keys[key].Previous;
		}

		// Set the state of the button to released and return whether it was previously released.
		auto WasReleased(this auto&& self, Win32::WPARAM key) -> bool
		{
			if (key >= self.Keys.size())
				return false;
			self.Keys[key].Previous = std::exchange(self.Keys[key].Current, false);
			self.Keys[key].ChangeWasChecked = false;
			return not self.Keys[key].Previous;
		}

		// Returns whether the button is currently released.
		auto IsReleased(this auto&& self, Win32::WPARAM key) -> bool
		{
			if (key >= self.Keys.size())
				return false;
			return not self.Keys[key].Current;
		}

		// Returns whether the button is currently pressed.
		auto IsPressed(this auto&& self, Win32::WPARAM key) -> bool
		{
			if (key >= self.Keys.size())
				return false;
			return self.Keys[key].Current;
		}

		auto CheckReleasedAndReset(this auto&& self, Win32::WPARAM key) -> bool
		{
			if (key >= self.Keys.size())
				return false;
			self.Keys[key].Previous = std::exchange(self.Keys[key].Current, false);
			self.Keys[key].ChangeWasChecked = false;
			return not self.Keys[key].Previous;
		}

		auto ResetCheckedAll(this auto&& self)
		{
			std::for_each(self.Keys, [](auto& key) static { key.ChangeWasChecked = false; });
		}

	private:
		Array Keys{};
	};

	auto KeysPressedMap = std::map<Win32::WPARAM, bool>{
		{ Win32::Keys::Left, false },
		{ Win32::Keys::Right, false },
		{ Win32::Keys::Space, false }
	};
	struct KeysPressed
	{
		bool Left = false;
		bool Right = false;
		bool Space = false;
	};
}
