export module shared:comptr;
import :win32;

export namespace Shared
{
	template<typename T>
	struct Ptr
	{
		using pointer = T*;

		constexpr ~Ptr() noexcept
		{
			reset();
		}

		constexpr Ptr() = default;

		constexpr Ptr(T* typePtr) noexcept
			: ptr(typePtr)
		{}

		// Copyable
		constexpr Ptr(const Ptr& typePtr) noexcept
			: ptr(typePtr.ptr)
		{
			if (ptr)
				ptr->AddRef();
		}
		constexpr auto operator=(this Ptr& self, const Ptr& other) noexcept -> Ptr&
		{
			self.reset();
			self.ptr = other.ptr;
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
			self.reset();
			self.swap(other);
			return self;
		}

		constexpr operator Win32::GUID(this const Ptr& self) noexcept
		{
			return self.GetUuid();
		}

		constexpr operator bool(this const Ptr& self) noexcept
		{
			return self.ptr != nullptr;
		}

		constexpr auto operator==(this const Ptr& self, const Ptr& other) noexcept -> bool
		{
			return self.ptr == other.ptr;
		}

		constexpr auto operator*(this auto&& self) noexcept -> T*
		{
			return self.ptr;
		}

		constexpr auto operator->(this auto&& self) noexcept -> T*
		{
			return self.ptr;
		}

		constexpr auto reset(this Ptr& self) noexcept -> Ptr&
		{
			if (self.ptr)
			{
				self.ptr->Release();
				self.ptr = nullptr;
			}
			return self;
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

		constexpr auto get(this const Ptr& self) noexcept -> T*
		{
			return self.ptr;
		}

		constexpr auto swap(this Ptr& self, Ptr& other) noexcept -> void
		{
			auto temp = self.ptr;
			self.ptr = other.ptr;
			other.ptr = temp;
		}

		constexpr auto AddressOf(this Ptr& self) noexcept -> void**
		{
			return reinterpret_cast<void**>(&self.ptr);
		}

		constexpr auto AddressOfTyped(this Ptr& self) noexcept -> T**
		{
			return &self.ptr;
		}

		constexpr auto GetUuid(this const Ptr& self) noexcept -> Win32::GUID
		{
			return __uuidof(T);
		}

		constexpr auto ReleaseAndGetAddressOf(this Ptr& self) noexcept -> T**
		{
			self.reset();
			return &self.ptr;
		}

		T* ptr = nullptr;
	};

	// TODO: add static tests for Ptr to ensure it behaves as expected.
}
