QtUPnP framework is a C++ framework, based on QT5, to build easily an UPnP control point.
It focuses on the UPnP/AV standards.
For this, a set of classes are created to handle easily media servers and media renderers and to hide the UPnP protocol.
Of course UPnP protocol (UPnP/AV use UPnP) is implemented and your can handle every compatible devices.

For a better description, unzip /upnp/docs/html.zip and choose index.html.

Build the framework and the test program need to install QT5. QtUPnP has been coded from:
  - Windows "qt-opensource-windows-x86-mingw530-5.8.0.exe" installer download from https://download.qt.io/official_releases/qt/5.8/5.8.0/.
    The built version is a 32 bits. It runs from Windows 7 to Windows 10 32 or 64 bits. 
    It has been tested on Windows 10 32 and 64 bits.
    Do not forget to add to the path (global or QtCreator) the access to qtupnp.dll or change qtupnp library by a static library.

  - Linux "qt-opensource-linux-x64-5.8.0.run" installer download from https://download.qt.io/official_releases/qt/5.8/5.8.0/.
    The built version is a 64 bits. It has been tested on Kubuntu under VirtualBox and Ubuntu native.
    The actual pro file creates a static library.

The framework use only QT API, no other system dependent API.

upnp is the framework to encapsulate the UPnP protocol.


*************** chupnp *******************

chupnp is an application for developers to test the framework. You can see the documentation at /QtUPnP/chupnp/docs/Test QtUPnP with chupnp.pdf
The main functionalities are:
  - Discover devices
  - Choose devices.
  - Invoke the actions.
  - and see the detailed results.
  - ...
  
  

*************** AIVCtrl *******************

aivctrl is a control point for users to drive media servers and media renderers. The user documentation it in /QtUPnP/aivctrl/doc
The main fonctionnalities are:
  - A remote control format (small size on the screen).
  - Discover and show servers and renderers.
  - Browse the servers content.
  - Manage renderers like a player (volume, seek, mute, suffle, repeat, cover, previous, next...).
  - Manage audio, image, video playlists and favorites.
  - Verify playlist elements.
  - And a lot of another things.
  
To test easily AIVCtrl, some installers are available at: https://drive.google.com/drive/folders/1buJLY6XTpp-wZo90_Iixgs8oof-Ij1yu?usp=sharing
  - Windows 7 to 10: Download install-AIVCtrl-100.exe and double click on it to install.
    Tested with Windows 10. To use Vista the application must be rebuilt with Qt 5.6.
  - Kubuntu and Ubuntu: Download aivctrl-1.0.0.deb double click on it to install.
    Tested with Kubuntu 17.10, and Ubuntu 16.04 LTS (for this, use preferably /user/share/aivctrl/aivctrl.run.desktop to launch AIVCtrl).
  - All Linux: Download aivctrl-1.0.0.zip for a manual installation.
    Not tested (for this, use preferably /user/share/aivctrl/aivctrl.run.desktop to launch AIVCtrl).)
  - Documentation in English and French.
  
Some hardware or software devices are tested :
  
 Hardware servers

    - NAS Synology (DSM 6).
    - NAS Western Digital My Cloud.
    - Cabasse Stream 1 v17.01.5.
    - La Box Numericable.
    - FreeBox Révolution.
 
 Software servers

    - Windows Media Player v12.015063.608 (Windows).
    - Foobar 2000 v1.3.16 with UPnP/DLNA Renderer, Server, Control Point plugin v0.99.49(Windows).
    - StriimServer Awox v1.2.0.0 (Windows).
    - MediaMonkey Windows v4.1.19 (Windows).
    - MediaTomb v0.12.2 (Linux).	

 Harware renderers

    Cabasse Stream 1 v17.01.5.
    La Box Numericable.
    Some televisions Samsung and Sony.

 Software renderers

    Renderer Android Awox v2.3.0 (Android).
    BubbleUPnP v2.8.14 (Android).
    Foobar 2000 v1.3.16 with UPnP/DLNA Renderer, Server, Control Point plugin v0.99.49 (Windows).
    Windows Media Player v12.015063.608 (Windows)

If you have some free time to test or use AIVCtrl, please use the issues and pull requests tabs for your remarks, malfunctions
and also to report if your servers and renderers are working or not working properly.

Good luck to all





