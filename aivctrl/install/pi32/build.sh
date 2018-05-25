clear

PRODUCT=aivctrl

echo
echo "********************** Build $PRODUCT *************************"

QTDIR=/usr/lib/arm-linux-gnueabihf/qt5
DEVEL=/home/$USER/Devel/QtUPnP
OUTPUT=$DEVEL/$PRODUCT/install/pi32/Output
TARGET=$OUTPUT/debian/usr/bin/$PRODUCT
BINARIES=/home/$USER/Devel/build-$PRODUCT-Desktop-Release
VERSION=1.1.2

echo $TARGET

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

echo Copy plugins files
mkdir $TARGET/plugins
cp $BINARIES/$PRODUCT/plugins/*.* $TARGET/plugins

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
cp $QTDIR/bin/linguist $TARGET

echo Build debian package
cd Output
chmod 755 debian/DEBIAN/post*
chmod 755 debian/DEBIAN/pre*
dpkg-deb --build debian $PRODUCT-pi3-$VERSION.deb
cd ..

echo Build zip file
zip -q -r -9 $OUTPUT/$PRODUCT-pi3-$VERSION.zip $TARGET
echo "********************** End build *************************"


