module;

#include <functional>
#include <string>
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

export module spaceinvaders.ui.mainwindow;
import core.direct2d.d2d1factory;
import spaceinvaders.ui.input;

export namespace SpaceInvaders::UI
{
    class MainWindow
    {
        public:
            using OnResizeEvt = std::function<void(const unsigned width, const unsigned height)>;
            using OnInputEvt = std::function<void(const InputType type, const wchar_t key)>;

        public:
            virtual ~MainWindow();
            MainWindow();
            MainWindow(MainWindow&& other) noexcept;
            MainWindow(const MainWindow&) = delete;

        public:
            virtual MainWindow& operator=(MainWindow&& other) noexcept;
            virtual MainWindow& operator=(const MainWindow&) = delete;

        public:
            virtual void Initialise();
            virtual HWND GetHandle() const noexcept;
            virtual unsigned GetWidth() const;
            virtual unsigned GetHeight() const;
            virtual unsigned GetClientWidth() const;
            virtual unsigned GetClientHeight() const;
            virtual void SetCaption(const std::wstring& caption);

        public:
            // Note that these need to be initialised to at least the empty function or 
            // you'll get a bad function call exception
            OnResizeEvt OnResize;
            OnInputEvt OnInputPressed;
            OnInputEvt OnInputReleased;

        protected:
            // Draw content.
            virtual void OnRender();

            virtual LRESULT HandleMessage(
                HWND hWnd,
                UINT message,
                WPARAM wParam,
                LPARAM lParam
            );
            
            virtual const WNDCLASSEX& GetClass() const noexcept;

            static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

            virtual void Destroy();

            virtual MainWindow& Move(MainWindow& other) noexcept;

        protected:
            HWND m_hwnd;
            DWORD m_windowStyle;
    };
}
