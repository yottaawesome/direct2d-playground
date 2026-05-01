export module shared:comptr;
import std;
import :win32;

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
		constexpr auto operator=(this Ptr& self, const Ptr& other) noexcept -> Ptr&
		{
			if (&self == &other)
				return self;
			self.reset();
			self.ptr = other.ptr;
			if (self.ptr)
				self.ptr->AddRef();
			return self;
		}

		// Movable
		constexpr Ptr(Ptr&& other) noexcept
			: ptr(other.ptr)
		{
			other.ptr = nullptr;
		}
		constexpr auto operator=(this Ptr& self, Ptr&& other) noexcept -> Ptr&
		{
			if (&self == &other)
				return self;
			self.reset();
			self.ptr = other.ptr;
			other.ptr = nullptr;
			return self;
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

		T* ptr = nullptr;
	};

	// TODO: add static tests for Ptr to ensure it behaves as expected.
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
}
