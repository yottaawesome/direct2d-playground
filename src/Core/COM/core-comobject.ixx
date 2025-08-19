export module core:com_comobject;
import std;
import :win32;
import :error;

export namespace COM
{
	struct InitType {} constexpr Init;
	template<auto T>
	concept NotNullPtr = not std::is_null_pointer_v<decltype(T)>;

	template<typename TInterface, auto VClsId = nullptr>
	struct ComObject final
	{
		~ComObject() { Close(); }
		
		constexpr ComObject() = default;
		constexpr ComObject(TInterface* ptr) : m_ptr(ptr) {}

		ComObject(const InitType&) requires NotNullPtr<VClsId>
		{
			Initialize();
		}

		ComObject(ComObject&& other) { Mover(other); }
		ComObject& operator=(ComObject&& other) noexcept
		{
			return Move(other);
		}

		ComObject(const ComObject& other) { Copy(other); }
		ComObject& operator=(const ComObject& other) noexcept
		{
			return Copy(other);
		}

		void Close()
		{
			m_ptr ? (m_ptr->Release(), m_ptr = nullptr) : m_ptr;
		}

		TInterface* operator->() noexcept { return m_ptr; }

	private:
		ComObject& Move(ComObject& other) noexcept
		{
			Close();
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
			return *this;
		}

		ComObject& Copy(const ComObject& other) noexcept
		{
			other.m_ptr->AddRef();
			Close();
			m_ptr = other->m_ptr;
			return *this;
		}

		void Initialize() requires NotNullPtr<VClsId>
		{
			Win32::HRESULT hr = Win32::CoCreateInstance(
				VClsId,
				nullptr,
				Win32::CLSCTX::CLSCTX_INPROC_SERVER,
				__uuidof(TInterface),
				(void**)&m_ptr
			);
			if (Win32::HrFailed(hr))
				throw Error::COMError("Failed to create object.");
		}

		TInterface* m_ptr = nullptr;
	};
	using C = ComObject<Win32::IWICImagingFactory2, Win32::ClsWICImagingFactory2>;
}
