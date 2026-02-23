@echo off
::This file was created automatically by CrossIDE to compile with C51.
D:
cd "\Coding\2026\291\lab4\"
"D:\CrossIDE\Call51\Bin\c51.exe" --use-stdout  "D:\Coding\2026\291\lab4\lab4.c"
if not exist hex2mif.exe goto done
if exist lab4.ihx hex2mif lab4.ihx
if exist lab4.hex hex2mif lab4.hex
:done
echo done
echo Crosside_Action Set_Hex_File D:\Coding\2026\291\lab4\lab4.hex
