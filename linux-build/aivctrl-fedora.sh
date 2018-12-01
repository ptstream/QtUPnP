# Build and launch aivctrl on fedora

# Get qmake location
QMAKE=$(which qmake-qt5)

# Create the Linux Makefile
$QMAKE ../aivctrl.pro

# Build chupnp  
make -j$(nproc)

# Launch aivctrl
aivctrl/aivctrl                          
