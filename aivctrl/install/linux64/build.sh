clear

PRODUCT=aivctrl

echo
echo "********************** Build $PRODUCT *************************"

QTDIR=/home/$USER/Qt5.8.0/5.8/gcc_64
DEVEL=/home/$USER/Documents/Development/QtUPnP
OUTPUT=$DEVEL/$PRODUCT/install/linux64/Output
TARGET=$OUTPUT/debian/usr/bin/$PRODUCT
BINARIES=/home/$USER/Documents/Development/build-$PRODUCT-Desktop_Qt_5_8_0_GCC_64bit-Release
VERSION=1.0.0-beta2

if [ -d $TARGET ]
then
echo Remove $TARGET
rm -r $TARGET
fi

if [ -f $OUTPUT/$PRODUCT-$VERSION.deb ]
then
echo Remove $OUTPUT/$PRODUCT-$VERSION.deb
chmod +w $OUTPUT/$PRODUCT-$VERSION.deb
rm $OUTPUT/$PRODUCT-$VERSION.deb
fi

if [ -f $OUTPUT/$PRODUCT-$VERSION.zip ]
then
echo Remove $OUTPUT/$PRODUCT-$VERSION.zip
chmod +w $OUTPUT/$PRODUCT-$VERSION.zip
rm $OUTPUT/$PRODUCT-$VERSION.zip
fi

mkdir "$TARGET"

echo Copy binary files
cp $BINARIES/$PRODUCT/$PRODUCT $TARGET/$PRODUCT
cp ../../icons/*48.png $TARGET
cp $DEVEL/$PRODUCT/install/linux64/$PRODUCT $TARGET/$PRODUCT.run

echo Copy translation files
mkdir $TARGET/languages
cp ../../languages/*_??.qm $TARGET/languages
cp ../../languages/*.png $TARGET/languages

echo Copy .destktop file
cp $OUTPUT/*.desktop $TARGET

echo Copy Qt Images plugins
mkdir $TARGET/imageformats
cp $QTDIR/plugins/imageformats/*.* $TARGET/imageformats

echo Copy Qt Bearer plugins
mkdir $TARGET/bearer
cp $QTDIR/plugins/bearer/*.* $TARGET/bearer

echo Copy Qt platforms
mkdir $TARGET/platforms
cp $QTDIR/plugins/platforms/*.* $TARGET/platforms

echo Copy Qt binary files
mkdir $TARGET/lib
cp $QTDIR/lib/libicudata.so.56 $TARGET/lib
cp $QTDIR/lib/libicui18n.so.56 $TARGET/lib
cp $QTDIR/lib/libicuuc.so.56.1 $TARGET/lib/libicuuc.so.56
cp $QTDIR/lib/libQt5Core.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5DBus.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5Gui.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5Network.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5Widgets.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5XcbQpa.so.5 $TARGET/lib
cp $QTDIR/lib/libQt5Xml.so.5 $TARGET/lib
cp $QTDIR/bin/linguist $TARGET

echo Build debian package
cd Output
chmod 755 debian/DEBIAN/post*
chmod 755 debian/DEBIAN/pre*
dpkg-deb --build debian $PRODUCT-$VERSION.deb
cd ..

echo Build zip file
zip -q -r -9 $OUTPUT/$PRODUCT-$VERSION.zip $TARGET
echo "********************** End build *************************"


