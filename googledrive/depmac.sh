echo "plugin dependencies"
cp $1/googledrive.png $2googledrive.png
cp $1/googledrive.ids $2googledrive.ids
mv $2/libgoogledrive.dylib $2googledrive.dylib
rm $2libgoogledrive.*