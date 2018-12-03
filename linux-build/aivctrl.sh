# Build and launch aivctrl
# The environment variable QMAKEFOLDEER must point to the qmake folder.
# e.g. For my Kubuntu Qt intallation, the folder is $HOME/Qt5.8.0/5.8/gcc_64/bin
#      For my Raspberry PI 3, the folder is /usr/bin

# Create the qmake folder environmen variable
QMAKE=$HOME/Qt5.8.0/5.8/gcc_64/bin/qmake

# Create the Linux Makefile
$QMAKEFOLDER/qmake ../aivctrl.pro        

# Build chupnp  
make               

# Launch aivctrl
aivctrl/aivctrl                          
