#pragma once

#include "d3d11.h"
#include "d3d12.h"
#include "d3dx12.h"
#include "wrl/client.h"
#include "dxgi1_6.h"
#include "d3dcompiler.h"
#include "DirectXMath.h"
#include "DirectXColors.h"
#include "DirectXCollision.h"
#include "DirectXPackedVector.h"

#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_dx12.h"

#include <wrl.h>
using namespace Microsoft::WRL;