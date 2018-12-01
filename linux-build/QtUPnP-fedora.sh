# Build and launch chupnp on fedora

# Get qmake location
QMAKE=$(which qmake-qt5)

# Create the Linux Makefile
$QMAKE ../QtUPnP.pro

# Build chupnp 
make -j$(nproc)

# Launch chupnp
chupnp/chupnp                           
