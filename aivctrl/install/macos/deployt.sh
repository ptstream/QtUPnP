PRODUCT=aivctrl
QTDIR=/Users/$USER/Qt/5.12.3/clang_64
DEVEL=/Users/$USER/Documents/Development/QtUPnP
OUTPUT=$DEVEL/$PRODUCT/install/macos/Output
TARGET=$OUTPUT/$PRODUCT
BINARIES=/Users/$USER/Documents/Development/build-$PRODUCT-Desktop_Qt_5_12_3_clang_64bit-Release
VERSION=1.1.4

WD=$PWD
cd $BINARIES/$PRODUCT
$QTDIR/bin/macdeployqt $PRODUCT.app -dmg
cd $WD
if [ -f Output/aivctrl-$VERSION.dmg ]
then
rm Output/aivctrl-$VERSION.dmg
fi
mv $BINARIES/$PRODUCT/$PRODUCT.dmg Output/
mv Output/aivctrl.dmg Output/aivctrl-$VERSION.dmg



