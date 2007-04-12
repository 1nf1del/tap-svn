; This examples demonstrates how libusb's drivers
; can be installed automatically along with your application using an installer.
;
; Requirements: Inno Setup (http://www.jrsoftware.org/isdl.php)
;
; To use this script, do the following:
; - copy libusb's driver (libusb0.sys, libusb0.dll) to this folder
; - create an .inf and .cab file using libusb's 'inf-wiward.exe'
;   and save the generated files in this folder.
; - in this script replace <your_inf_file.inf> with the name of your .inf file
; - customize other settings (strings)
; - open this script with Inno Setup
; - compile and run

[Setup]
AppName = Wuppy
AppVerName = Wuppy 1.0
AppPublisher = Simon Capewell
AppPublisherURL = http://www.toppy.org.uk/~simonc
AppVersion = 1.0.
DefaultDirName = {pf}\Wuppy
DefaultGroupName = Wuppy
Compression = lzma
SolidCompression = yes
; Win2000 or higher
MinVersion = 5,5
PrivilegesRequired = admin
OutputBaseFilename=WuppySetup

[Files]
; copy the file to the App folder
Source: "..\Release\*.exe"; DestDir: "{app}"
Source: "*.sys"; DestDir: "{app}"
Source: "*.cat"; DestDir: "{app}"
Source: "*.dll"; DestDir: "{app}"
Source: "*.inf"; DestDir: "{app}"

; also copy the DLL to the system folder so that rundll32.exe will find it
Source: "*.dll"; DestDir: "{win}\system32"; Flags: replacesameversion restartreplace uninsneveruninstall

[Icons]
Name: "{group}\Wuppy Command Prompt"; Filename: "{sys}\cmd.exe"; Parameters:"/k path=%path%;{app}"; WorkingDir:"{userdocs}"
Name: "{group}\Uninstall Wuppy"; Filename: "{uninstallexe}"

[Run]
; invoke libusb's DLL to install the .inf file
Filename: "rundll32"; Parameters: "libusb0.dll,usb_install_driver_np_rundll {app}\TopfieldLibUSB.inf"; StatusMsg: "Installing driver (this may take a few seconds) ..."

