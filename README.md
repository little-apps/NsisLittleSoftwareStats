# NsisLittleSoftwareStats
This is a Dynamic Linking Library (DLL) for integrating the API for [Little Software Stats](https://github.com/little-apps/little-software-stats) with the [Nullsoft Scriptable Install System (NSIS)](http://nsis.sourceforge.net/Main_Page). It is a Little Apps project and is coded in C++.

## Installation ##
### 1) Download The Library ###
The first step is to download our library from http://getlittleapps.com/little-software-stats/libraries/nsis-latest.zip

### 2) Include The Library ###
You will first need to add the “NsisLittleSoftwareStats.dll” file (located in the DLLs folder) to NSIS by copying it to “%ProgramFiles(x86)%\NSIS\Plugins”. If it’s NSIS Unicode, then you would need to copy “NsisLittleSoftwareStatsUnicode.dll” (located in the DLLs folder) to “%ProgramFiles(x86)%\NSIS\Unicode\Plugins”.

### 3) Integrate with your software ###
Now you’ll want to integrate Little Software Stats by calling it using your NSIS code. We have included Example.nsi (from Little Disk Cleaner) which shows how to call the plugin using NSIS.

### License ###
This library is licensed under the [GNU Lesser General Public License v3](http://www.gnu.org/copyleft/lesser.html).

### Show Your Support ###
Little Apps relies on people like you to keep our software running. If you would like to show your support for Little System Cleaner, then you can [make a donation](https://www.little-apps.com/?donate) using PayPal, Payza or credit card (via Stripe). Please note that any amount helps (even just $1).