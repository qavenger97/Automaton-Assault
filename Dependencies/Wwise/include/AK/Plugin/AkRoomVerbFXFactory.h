#pragma once
/// \file
/// Registers the Wwise RoomVerb plugin automatically.
/// This file should be included once in a .CPP (not a .h, really).  The simple inclusion of this file and the linking of the library is enough to use the plugin.
/// <b>WARNING</b>: Include this file only if you wish to link statically with the plugins.  Dynamic Libaries (DLL, so, etc) are automatically detected and do not need this include file.
/// <br><b>Wwise plugin name:</b>  Wwise RoomVerb
/// <br><b>Library file:</b> AkRoomVerbFX.lib


#if !defined AK_3DS && !defined AK_VITA_HW
AK_STATIC_LINK_PLUGIN(AkRoomVerbFX)
#endif
