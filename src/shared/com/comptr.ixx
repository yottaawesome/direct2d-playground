export module shared:comptr;
import std;
import :win32;
import :error;

export namespace Shared
{
	template<typename T>
	struct Ptr
	{
		constexpr ~Ptr() noexcept
		{
			reset();
		}

		constexpr Ptr() = default;

		constexpr explicit Ptr(T* typePtr) noexcept
			: ptr(typePtr)
		{}

		// Copyable
		constexpr Ptr(const Ptr& other) noexcept
			: ptr(other.ptr)
		{
			if (ptr)
				ptr->AddRef();
		}
		constexpr auto operator=(const Ptr& other) noexcept -> Ptr&
		{
			if (this == &other)
				return *this;
			reset();
			ptr = other.ptr;
			if (ptr)
				ptr->AddRef();
			return *this;
		}

		// Movable
		constexpr Ptr(Ptr&& other) noexcept
			: ptr(other.ptr)
		{
			other.ptr = nullptr;
		}
		constexpr auto operator=(Ptr&& other) noexcept -> Ptr&
		{
			if (this == &other)
				return *this;
			reset();
			ptr = std::exchange(other.ptr, nullptr);
			return *this;
		}

		constexpr explicit operator bool(this const Ptr& self) noexcept
		{
			return self.ptr != nullptr;
		}

		constexpr auto operator==(this const Ptr& self, const Ptr& other) noexcept -> bool
		{
			return self.ptr == other.ptr;
		}

		constexpr auto operator==(this const Ptr& self, std::nullptr_t) noexcept -> bool
		{
			return self.ptr == nullptr;
		}

		constexpr auto operator*(this auto&& self) noexcept -> T&
		{
			return *self.ptr;
		}

		constexpr auto operator->(this auto&& self) noexcept -> T*
		{
			return self.ptr;
		}

		constexpr auto reset(this Ptr& self) noexcept -> void
		{
			if (self.ptr)
			{
				self.ptr->Release();
				self.ptr = nullptr;
			}
		}

		constexpr auto detach(this Ptr& self) noexcept -> T*
		{
			T* temp = self.ptr;
			self.ptr = nullptr;
			return temp;
		}

		constexpr auto Get(this const Ptr& self) noexcept -> T*
		{
			return self.ptr;
		}

		constexpr auto swap(this Ptr& self, Ptr& other) noexcept -> void
		{
			auto temp = self.ptr;
			self.ptr = other.ptr;
			other.ptr = temp;
		}

		// Releases the held interface and returns the address of the internal
		// pointer for use as an out-parameter (the typical COM idiom).
		constexpr auto ReleaseAndGetAddressOf(this Ptr& self) noexcept -> T**
		{
			self.reset();
			return &self.ptr;
		}

		constexpr auto Release() -> std::uint32_t
		{
			if (not ptr)
				return 0;
			auto count =  ptr->Release();
			ptr = nullptr;
			return count;
		}

		constexpr auto AddRef(this Ptr& self) -> std::uint32_t
		{
			if (not self.ptr)
				return 0;
			return self.ptr->AddRef();
		}

		// Convenience aliases for ReleaseAndGetAddressOf, kept because the
		// existing call sites use them. Both release first to avoid leaks.
		constexpr auto AddressOf(this Ptr& self) noexcept -> void**
		{
			self.reset();
			return reinterpret_cast<void**>(&self.ptr);
		}

		constexpr auto AddressOfTyped(this Ptr& self) noexcept -> T**
		{
			return self.ReleaseAndGetAddressOf();
		}

		constexpr auto GetUuid(this const Ptr&) noexcept -> Win32::GUID
		{
			return __uuidof(T);
		}

		template <typename U>
		auto As(this const Ptr& self) -> Ptr<U>
		{
			if (not self.ptr)
				return {};

			auto rawPtr = static_cast<U*>(nullptr);
			auto hr = HResult{
				self.ptr->QueryInterface(__uuidof(U), reinterpret_cast<void**>(&rawPtr))
			};
			if (not hr)
				throw ComError{ hr, "QueryInterface() failed in As<>()" };
			return Ptr<U>{ rawPtr };
		}

		T* ptr = nullptr;
	};
}

namespace
{
	static_assert(std::is_default_constructible<Shared::Ptr<int>>::value, "Ptr should be default constructible");
	static_assert(std::copyable<Shared::Ptr<int>>, "Ptr should be copyable");
	static_assert(std::movable<Shared::Ptr<int>>, "Ptr should be movable");

	struct TestInterface : public Win32::IUnknown
	{
		unsigned RefCount = 1;
		constexpr auto AddRef() noexcept -> unsigned long override { return ++RefCount; }
		constexpr auto Release() noexcept -> unsigned long override { return --RefCount; }
		constexpr auto QueryInterface(const Win32::GUID& iid, void** ppv) noexcept -> Win32::HRESULT override { return 0; }
	};

	// Basic AddRef/Release test
	static_assert(
		[] -> bool
		{
			auto rawPtr = std::make_unique<TestInterface>();
			auto ptr1 = Shared::Ptr<TestInterface>{ rawPtr.get() };
			if (ptr1.AddRef() != 2)
				throw "AddRef did not return 2";
			if (ptr1.Release() != 1)
				throw "Release did not return 1";
			return true;
		}(), "Ptr should call AddRef and Release correctly"
	);
	// Move test
	static_assert(
		[] -> bool
		{
			auto rawPtr = std::make_unique<TestInterface>();
			auto ptr1 = Shared::Ptr<TestInterface>{ rawPtr.get() };
			auto ptr2 = std::move(ptr1);
			if (ptr1.Get() != nullptr)
				throw "Moved-from Ptr should have null pointer";
			if (ptr2.Get() != rawPtr.get())
				throw "Moved-to Ptr should have the original pointer";
			return true;
		}(), "Moving Ptr should transfer ownership correctly"
	);
	// Copy test
	static_assert(
		[] -> bool
		{
			auto rawPtr = std::make_unique<TestInterface>();
			auto ptr1 = Shared::Ptr<TestInterface>{ rawPtr.get() };
			auto ptr2 = ptr1; // Copy constructor
			if (ptr1.Get() != rawPtr.get() || ptr2.Get() != rawPtr.get())
				throw "Both Ptr instances should point to the same object";
			if (ptr1.AddRef() != 3)
				throw "AddRef did not return 3 after copy";
			if (ptr2.Release() != 2)
				throw "Release did not return 2 after copy";
			return true;
		}(), "Copying Ptr should call AddRef correctly"
	);
	// Combined copy and move test
	static_assert(
		[] -> bool
		{
			auto rawPtr = std::make_unique<TestInterface>();
			auto ptr1 = Shared::Ptr<TestInterface>{ rawPtr.get() };
			auto ptr2 = ptr1; // Copy constructor
			auto ptr3 = std::move(ptr1); // Move constructor
			if (ptr1.Get() != nullptr)
				throw "Moved-from Ptr should have null pointer";
			if (ptr2.Get() != rawPtr.get())
				throw "Copy-constructed Ptr should still point to the original object";
			if (ptr3.Get() != rawPtr.get())
				throw "Move-constructed Ptr should point to the original object";
			if (ptr2.AddRef() != 3)
				throw "AddRef did not return 3 after copy and move";
			if (ptr3.Release() != 2)
				throw "Release did not return 2 after copy and move";
			return true;
		}(), "Moving Ptr should transfer ownership correctly"
	);
}
