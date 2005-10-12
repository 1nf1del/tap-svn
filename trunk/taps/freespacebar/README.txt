Freespacebar 1.1n, draws a bargraph of freespace on the filelist screen, and calculates the current bitrate being used.
Copyright (C) 2005 Tony H

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


Features of the tap: This TAP calculates the bitrate of the channel you are watching by calculating how much disk space is lost over a 6 second interval.   The results are a little out (slightly jumpy value) but a pretty good indication of the transmitted bitrate (certainly good enough to determine how much space is left, and allowing the bitrate over 6 secs to be approx calculated).
If the bitrate is <0.5Mb/s or >30Mb/s (indicating a glitch in the maths that happens sometimes when the HDD first spins up, also indicating the HDD isn't recording anything at all) then the value shown= 7Mb/s* (*see release info for 1.1j), and the calculation for hours:minutes is done for 7Megabit/s in this situation.

So for newbies to the "TAP" environment, all you need to do is copy this .tap file to your Topfield via USB with the ALTAIR proggy avail on the Topfield Australia site, and put the tap file into the "ProgramFiles" folder where you can manually launch it when you wish, or put it into "\ProgramFiles\Auto Start"  so that you don't have to manually choose it to start it.

To manually launch once you have uploaded it: hit the middle filelist button on the bottom section of the remote, then either >| or |< once or twice to see the TAP applications that you have uploaded (window will be titled "Program Files").  Select the TAP file, hit OK, and away you go! 
To remove the TAP from memory, you will need TSRcommander also available on my website (you can obviously delete it from "auto start" and reboot the Toppy, but with TSRcommander, you have the option of quitting the tap without rebooting).

Cheers
Tonymy01

Freespacebar 1.1n
Now uses TSR commander to exit, rather than a menu system (saves daisy chaining to multiple taps)
21/01/05

Freespacebar 1.1m
Fixes overlapping text with newer larger keyboard size with new f/w.  Fixed German text display a little.
4/12/04

Freespacebar 1.1k
Now if timeshift is off, will not poll the HDD free space during idle time, thus will let the HDD spin down.  Also accommodated german language mods.   Can only be used with newer than April 2004 firmware.
25/09/04

Release info:
Freespacebar 1.1j
If rate is 0, now will show 7Mb/s* with the "*" to indicate this is the value chosen to calculate the time remaining.
12/07/04

Freespacebar 1.1i
Date 27/06/2004
Fixed the "bitrate" calculation that Lucky missed in his version ;-)


Freespacebar 1.1h
Date: 10/06/2004
Problems with newer firmware fixed (This fixes the "corrupt screen" that 1.1g gives with newer firmwares (From April 20 and newer)).
Author: Lucky B


Freespacebar 1.1g
Date: 29/04/2004
I made the program average two adjacent readings as I found that the Toppy seems to "pump" high and low rates to the HDD, and the average is almost exactly what the recorded bitrate should be.   This results in far less fluctuations of the reading.   Since I am averaging over 2 periods now, I made the measurement period slightly smaller at 5.5secs (the smaller the period, the more severe the fluctuations are).
I also removed a couple of variables I setup for the prev version but never used.
Enjoy!


Freespacebar 1.1f
Date: 29/4/2004
Modded Koops version a bit, to calculate the used HDD rate.



Freespacebar 1.1e
Date: 26/4/2004

I've just edited the marcs original freespace bar and replaced all the text with english and recomplied ( a 2 second job).
It was mainly just a test of my TAP compile environment.

It *should* work. ;)

1.1d - Changed - German to english text
1.1e - Changed - 5Mbit to 7Mbit remaining time calculation. SHould provide a more realistic estimate of recording time remaining. 
Averaged according to the rates on http://users.bigpond.net.au/tharper/DTVBitrates.html

Koops

All original work as documented below!


Original freespacebar 1.1b
*****************************************************************************************


FreeSpaceBar ist eine Modifikation von Judy's FreeSpacePie-TAP.
Während FreeSpacePie eine tolle Optik bietet, hat mich gestört, daß die Legende
im Festplattenmenü überlagert wird und der Informationsgehalt der Anzeige für
meine Vorstellungen nicht umfangreich genug war. Daher habe ich diese Modifikation
gebastelt.

Modifikation von Marc:

FreeSpaceBar zeigt den freien Festplatten-Platz in Form eines Balkens als Ersatz
für die normale Textanzeige an. Hierdurch wird die Legende nicht überlagert.

Die Anzeige gibt Aufschluß über den prozentualen Füllstand, den freien Platz sowie
die Gesamtgröße und die zu erwartende Restaufnahmezeit bei 5MBit Datenrate.

Homepage von Marc: http://www.marcwinter.net

Versions-Historie:

05.11.03 Version 1.0
Erste "offiziell" funktionsfähige Version 1.0

07.11.03 Version 1.1
Verbesserung der Lesbarkeit der Texte innerhalb des Balkens

08.11.03 Version 1.1a
Die Restaufnahmedauer wird nun genauer (Stunden+Minuten) angezeigt

08.11.03 Version 1.1b
Weitere optische Verbesserungen. Die Anzeige der Restzeit wurde nach oben verschoben
um den Balken übersichtlicher zu gestalten.

------------- Original FreeSpacePie - Readme ------------------

FreeSpacePie.tap ist eine Applikation (TAP) für den Topfield TF5000PVR.

Es zeigt den freien Festplatten-Platz grafisch als Tortendiagramm im File-Menü.
FreeSpacePie kann mit der Exit-Taste der FB beendet werden.
Es bietet sich an, das TAP in den Auto Start-Ordner zu legen.

Viel Spaß damit, Judy

Versions-Historie:

16.10.03
FreeSpacePie erstellt und veröffentlicht.

Bekannte Bugs:
BUG#1: Beim Verzeichnis-Wechsel wird die Grafik nicht erneut angezeigt.
BUG#2: Beim Sprung vom File-Menü direkt ins Haupt-Menü wird dieses nicht angezeigt.

17.10.03
Neuer Bug von Fummleroni gefunden:
BUG#3: Farben des Diagramms vertauscht.

Neuer Bug:
BUG#4: Kein update des Diagramms beim löschen von Dateien.

18.10.03
Neuer Bug wie bei DontSpinDown von rosenbusch gefunden:
BUG#5: Exit-Taste bei angezeigter INFOBOX führt zum anzeigen des Exit-Menüs im TAP, statt die INFOBOX zu schließen.

BUG#3 beseitigt.
BUG#2 beseitigt.
BUG#1 beseitigt.
Diagrammanzeige beschleunigt.
Abbildungsqualität verbessert.
BUG#4 beseitigt.
Source-Code besser dokumentiert.
BUG#5 beseitigt.

Version 1.1 veröffentlicht.
