QtUPnP framework is a C++ framework, based on QT5, to build easily an UPnP control point.
It focuses on the UPnP/AV standards.
For this, a set of classes are created to handle easily media servers and media renderers and to hide the UPnP protocol.
Of course UPnP protocol (UPnP/AV use UPnP) is implemented and your can handle every compatible devices.

For a better description, unzip /upnp/docs/html.zip and choose index.html.

Build this framework and the test program need to install QT5. QtUPnP has been coded from:
  - Windows: The build has been tested from and Qt 5.12.3 (mingw73_32 and mingw73_64).
    Do not forget to add to the path (global or QtCreator) the access to qtupnp.dll or change qtupnp library by a static library.

  - Linux: The build has been tested Qt 5.12.3.
    A 64 bits is built. It has been tested on Ubuntu 18.04 under VirtualBox and Ubuntu 18.04 native.
	To build QtUPnP without QtCreator, see linux-build folder.
	
  - MacOS Mojave 10.14: The build has been tested from Qt 5.12.3 with Clang compiler.
  
  - Raspbian QtUPnP can be built using QT5.7 available in standard packages.
    To build QtUPnP install QT5.7 and QtCreator on your Raspberry PI3.
	  * sudo apt-get install qt5-default
      * sudo apt-get install qtcreator
	  
	For me it worked but another solution is possible.
	
	  * sudo apt-get update
      * sudo apt-get upgrade
      * sudo apt-get install qtcreator gcc qtbase5-dev
	  
    It has been tested under Raspbian Stretch with desktop on a Raspberry PI3.

**Remarks**
- The framework use only QT API.
- The folder upnp contains the framework to encapsulate the UPnP protocol. By default, it is a dll for Windows and a static library for all other systems.
- The folder googledrive is optional. With this plugin, you can play files stored in your googledrive space.
  You can remove it to build aivctrl. If you want to keep this folder, on Linux systems, you must enter in a terminal **chmod +x dep.sh** in the folder .../QtUPnP/googledrive.
  Similarly on MacOS enter **chmod +x depmac.sh**.


*************** chupnp *******************
![](readme-images/chupnp.png)

chupnp is an application for developers to test the framework. You can see the documentation at /QtUPnP/chupnp/docs/Test QtUPnP with chupnp.pdf
The main functionalities are:
  - Discover devices
  - Choose devices.
  - Invoke the actions.
  - and see the detailed results.
  - ...
  
  
*************** AIVCtrl *******************
![](readme-images/aivctrl.png)

aivctrl is a control point for users to drive media servers and media renderers. The user documentation it in /QtUPnP/aivctrl/doc
The main fonctionnalities are:
  - A remote control format (small size on the screen).
  - Discover and show servers and renderers.
  - Browse the servers content.
  - Manage renderers like a player (volume, seek, mute, suffle, repeat, cover, previous, next...).
  - Manage audio, image, video playlists and favorites.
  - Verify playlist elements.
  - And a lot of another things.
  
To test easily AIVCtrl, some installers are available at: https://drive.google.com/drive/folders/1_MOe60dsZ-chqqjHpwyHcySzrp0ZyX55?usp=sharing

  - Windows 7 to 10: Download install-AIVCtrl-32-1.1.4.exe and double click on it to install. This version run on Windows 32 and 64 bits.
    You can use install-AIVCtrl-64-1.1.4.exe to install the 64 bits version.
    Tested with Windows 10.
	
  - Linux x86 Debian base: Download aivctrl-1.1.4.deb double click on it to install. It is a 64 bits version.
    Tested Ubuntu 18.04.
	
  - All Linux x86: Download aivctrl-1.1.4.zip for a manual installation. It is a 64 bits version.
    Not tested (for this, use preferably /user/share/aivctrl/aivctrl.run.desktop to launch AIVCtrl).
	
  - MacOS: Download aivctrl-1.1.4.dmg double click on it and drag the aivctrl icon in Applications folder. It is a 64 bits version.
	
  - Raspbian: Download aivctrl-pi3-1.1.4.deb. It is a 32 bits version.
      Tested on Raspberry PI3 with Raspbian Stretch.
    	
  - Documentation in English and French.
  
  - Remark: If you system run under VirtualBox, choose mode Bridge for the network.
  
Some hardware or software devices are tested :
  
 Hardware servers

    - NAS Synology DS214 (DSM 6).
	- NAS Synology DS215J. (1)
	- NAS Synology DS213. (1)
	- NAS Synology DS120. (1)
    - NAS Western Digital My Cloud. (1)
    - Cabasse Stream 1.
    - La Box Numericable.
    - FreeBox Revolution. (1)
	- CocktailAudio X40. (1)
 
 Software servers

    - Windows Media Player (Windows).
    - Foobar 2000 with UPnP/DLNA Renderer, Server, Control Point plugin (Windows).
    - StriimServer Awox (Windows).
    - MediaMonkey Windows (Windows).
    - MediaTomb(Linux).
	- MiniDLNA on Raspberry Pi 2, LEDE/OpenWRT, Linux (1).
	- Plex Media Server (detected). (1)
	- MinimServer (detected). (1)

 Harware renderers

   - Cabasse Stream 1.
   - La Box Numericable.
   - Some televisions Samsung, Sony, LG. (1)
   - Denon AVR-X2100W Amplifier. (1)
   - GGMM E5 (es-100). (1)
   - Naim NAC-N 272. (1)
   - Denon DRA-100. (1)
   - LG TV webOS based 55B7V. (1)
   - CocktailAudio X40. (1)
   - Pioneer N-50. (1)

 Software renderers

    BubbleUPnP (Android).
    Foobar 2000 with UPnP/DLNA Renderer, Server, Control Point plugin (Windows).
    Windows Media Player (Windows)
	
 Router	detected
 
 	TPLink N600 Router. (1)
	Box Numericable.
	SuperHub 3 router from Virgin ISP. (1)
	
 Over devices detected

    Wemo Wi-Fi Smart Light Switch F7C030fc. (1)
	Wemo Switch Smart Plug F7C027fc. (1)
	
  (1) Tested by users and contributors. Many thanks for your contribution and your help.

If you have some free time to test or use AIVCtrl and chupnp, please use the issues tab for your remarks, malfunctions
and also to report if your servers, renderers and other devices are working or not working properly.





