MIME plugin for Notepad++ implements several main functionalities defined in MIME (Multipurpose Internet Mail Extensions) :
1. Base64 Encoding/Decoding
2. Quoted-printable Encoding/Decoding
3. URL Encoding/Decoding
4. SAML Decoding (though it's not part of MIME)

This plugin is under GPL.
Don Ho <don.h@free.fr>

Modified on March 12, 2024 by ExSlam.

To compile, download by clicking on the green Code button at the top, download zip, extract and navigate to the vs.proj folder. Open the .vxproj file with visual studio and select Release x64 at the top if it says Debug instead. Then click on Local Windows Debugger and it will compile the code into mimeTools.dll in the directory listed in the visual studio console (at the bottom). Navigate to that directory and move the mimeTools.dll into a directory named mimeTools in your Notepad++ plugins folder, which is usally located at C:\Program Files\Notepad++\plugins\ . Replace the existing mimeTools.dll if prompted. Done.
