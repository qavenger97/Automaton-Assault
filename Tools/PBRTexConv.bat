@echo off
for %%f in (*Base_Color.tga) do ("%~dp0texconv.exe" -f BC1_UNORM %%f)
for %%f in (*Emissive.tga) do ("%~dp0texconv.exe" -f BC1_UNORM %%f)
for %%f in (*Normal_OpenGL.tga) do ("%~dp0texconv.exe" -f BC5_SNORM %%f)
for %%f in (*Metallic.tga) do ("%~dp0texconv.exe" -f B8G8R8A8_UNORM %%f)
for %%f in (*Roughness.tga) do ("%~dp0texconv.exe" -f B8G8R8A8_UNORM %%f)
pause
