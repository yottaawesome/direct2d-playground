module;

#include <functional>
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
            MainWindow();
            virtual ~MainWindow();

        public:
            virtual void Initialise();
            virtual HWND GetHandle() const noexcept;
            virtual unsigned GetWidth() const;
            virtual unsigned GetHeight() const;
            virtual unsigned GetClientWidth() const;
            virtual unsigned GetClientHeight() const;

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

        protected:
            HWND m_hwnd;
            DWORD m_windowStyle;
    };
}
