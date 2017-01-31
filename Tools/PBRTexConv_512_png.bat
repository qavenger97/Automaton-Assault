@echo off
for %%f in (*_c.png) do ("%~dp0texconv.exe" -f BC1_UNORM -w 512 -h 512 %%f)
for %%f in (*_e.png) do ("%~dp0texconv.exe" -f BC1_UNORM -w 512 -h 512 %%f)
for %%f in (*_n.png) do ("%~dp0texconv.exe" -f BC5_SNORM -w 512 -h 512 %%f)
for %%f in (*_m.png) do ("%~dp0texconv.exe" -f B8G8R8A8_UNORM -w 512 -h 512 %%f)
for %%f in (*_r.png) do ("%~dp0texconv.exe" -f B8G8R8A8_UNORM -w 512 -h 512 %%f)
pause
