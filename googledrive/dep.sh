echo "plugin dependencies"
cp $1/googledrive.png $2googledrive.png
cp $1/googledrive.ids $2googledrive.ids
mv $2/libgoogledrive.so.1.0.0 $2googledrive.so
rm $2libgoogledrive.*