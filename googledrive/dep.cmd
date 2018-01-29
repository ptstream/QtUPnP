echo off
copy /Y %1\*.png %2
copy /Y %1\*.ids %2
del %2\libgoogledrive.a