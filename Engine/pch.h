#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

//#include <vld.h>

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <wrl\client.h>

using Microsoft::WRL::ComPtr;

#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")

#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <algorithm>

//#include <G_System\GFile.h>
//#include <G_System\GLog.h>

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>		// include in Math.h
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")

#include <comdef.h>

#include <fstream>
#include <sstream>
#include <stdarg.h>

#include "Core\Math.h"
#include "Core\StrID.h"
#include "TinyXML\tinyxml2.h"

#define FORCE_INLINE __forceinline

#define DEBUG_RENDER 0

#define BITSETVAL(data, bit, val) (data) = ((data) & ~(1 << (bit))) | ((val) << (bit))
#define FINAL
namespace Hourglass {}
namespace hg = Hourglass; // aliasing
//namespace gwc = GW::CORE;

namespace sm = DirectX::SimpleMath;
