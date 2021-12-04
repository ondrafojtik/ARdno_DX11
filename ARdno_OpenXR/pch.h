#pragma once

#define _XM_NO_INTRINSICS_

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <array>
#include <map>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <d3d11.h>

#include <unknwn.h>
#include <Unknwn.h>

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11
//#include <../packages/OpenXR.Headers.1.0.10.2/include/openxr/openxr.h>
//#include <../packages/OpenXR.Headers.1.0.10.2/include/openxr/openxr_platform.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

//#include <openxr/openxr.h>
//#include <openxr/openxr_platform.h>

#include <XrError.h>
#include <XrHandle.h>
#include <XrMath.h>
#include <XrString.h>
#include <XrExtensions.h>
#include <XrSpatialGraphBridge.h>
//#include "XrUtility/XrSceneUnderstanding.h"
//#include "XrUtility/XrSceneUnderstanding.hpp"



#include <winrt/base.h> // winrt::com_ptr
#include "winrt/windows.Storage.h"
#include "winrt/windows.Foundation.h"
#include "winrt/windows.Foundation.Collections.h"

#include "winrt/Microsoft.MixedReality.QR.h"
#include "winrt/windows.Perception.Spatial.h"
#include "winrt/windows.Perception.Spatial.Preview.h"

#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <agile.h>
#include <concrt.h>
