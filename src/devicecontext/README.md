# Direct2D Device Context

Demonstrates Direct2D 1.1 rendering through an `ID2D1DeviceContext` backed by a DXGI swap chain.

The sample creates a D3D11 device, wraps it with an `ID2D1Device`, creates an `ID2D1DeviceContext`, binds the swap-chain back buffer as a Direct2D bitmap target, and renders a simple grid and geometry.
