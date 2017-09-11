QtUPnP framework is a C++ framework, based on QT5, to build easily an UPnP control point.
It focuses on the UPnP/AV standards.
For this, a set of classes are created to handle easily media servers and media renderers and to hide the UPnP protocol.
Of course UPnP protocol (UPnP/AV use UPnP) is implemented and your can handle every compatible devices.

For a better description, unzip /upnp/docs/html.zip and choose index.html.

Build the framework and the test program need to install QT5. QtUPnP has been coded from:
  - Windows "qt-opensource-windows-x86-mingw492-5.6.0.exe" installer download from "https://download.qt.io/official_releases/qt/5.6/5.6.0/".
    The built version is a 32 bits. It runs from Windows Vista to Windows 10 32 or 64 bits. It has been tested on Vista 32 bits and Windows 10 32 and 64 bits.

  - Linux "qt-opensource-linux-x64-5.8.0.run" installer download from https://download.qt.io/official_releases/qt/5.8/5.8.0/.
    The built version is a 64 bits. It has been tested on Kubuntu under VirtualBox and Ubuntu native.

The framework use only QT API, no other system dependent API.





