;--------------------------------
;Include Modern UI
 
!include "MUI2.nsh"
 
;--------------------------------
; General
Name "Little Disk Cleaner"
 
; Define the version (must have 2 decimals)
!define VERSION "1.0.0"
 
; Define the date for the output file
!define /date DATE "%m_%d_%Y"
 
; The file to write
OutFile "Little_Disk_Cleaner_${DATE}.exe"
 
; The default installation directory
InstallDir "$PROGRAMFILES\Little Disk Cleaner"
 
; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Little Disk Cleaner" "Install_Dir"
 
; Request application privileges for Windows Vista
RequestExecutionLevel admin
 
; GPL License
LicenseData gpl.txt
 
; Start Menu Folder
Var StartMenuFolder
 
; Compressor
SetCompressor /SOLID lzma
 
; File Info
VIAddVersionKey "ProductName" "Little Disk Cleaner"
VIAddVersionKey "CompanyName" "Little Apps"
VIAddVersionKey "LegalTrademarks" "Little Apps are licensed under the GNU General Public License v3"
VIAddVersionKey "LegalCopyright" "Copyright © Little Apps 2008"
VIAddVersionKey "FileDescription" "Open source disk cleaner"
VIAddVersionKey "FileVersion" "${VERSION}"
VIProductVersion "${VERSION}.0"
 
;--------------------------------
; Pages
 
  !define MUI_FINISHPAGE_RUN "$INSTDIR\Little Disk Cleaner.exe"
 
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Little Disk Cleaner"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
 
  !define MUI_FINISHPAGE_NOAUTOCLOSE
  !define MUI_UNFINISHPAGE_NOAUTOCLOSE
 
  /***********************************************************************************************************/
  /* THE CODE BELOW IS LICENSED UNDER THE CREATIVE COMMONS ATTRIBUTION NON-COMMERCIAL NO DERIVATIVES LICENSE */
  !define MUI_PAGE_CUSTOMFUNCTION_SHOW WarnMsgBox
  /* THE CODE ABOVE IS LICENSED UNDER THE CREATIVE COMMONS ATTRIBUTION NON-COMMERCIAL NO DERIVATIVES LICENSE */
  /***********************************************************************************************************/
  !insertmacro MUI_PAGE_LICENSE gpl.txt
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
 
  ; theese 2 lines are inserting LittleApp Suggestor upsell page (need NSIS CustomLicense plugin)
  !include "LittleAppSuggestor.nsdinc"
  Page custom fnc_LittleAppSuggestor_Show fnc_LittleAppSuggestor_Leave
 
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
 
  !insertmacro MUI_UNPAGE_CONFIRM
  ; !insertmacro MUI_UNPAGE_COMPONENTS
  !insertmacro MUI_UNPAGE_INSTFILES
 
   !define MUI_CUSTOMFUNCTION_GUIINIT MyGuiInit
 
;--------------------------------
;Languages
 
  !define MUI_LANGDLL_ALLLANGUAGES
 
  !insertmacro MUI_LANGUAGE "English"
;--------------------------------
; Language Text
 
LangString DESC_SecFramework ${LANG_ENGLISH} "Required to run Little Disk Cleaner"
 
LangString DESC_SecMain ${LANG_ENGLISH} "The core files of Little Disk Cleaner"
 
LangString DESC_SecUnMain ${LANG_ENGLISH} "Removes the core files"
 
LangString DESC_SecUnBackups ${LANG_ENGLISH} "The logs and backup files"
 
LangString DESC_CreateShortcuts ${LANG_ENGLISH} "Would you like to create a shortcut on the desktop?"
 
LangString DESC_AlreadyInstalled ${LANG_ENGLISH} "Little Disk Cleaner is already installed. $\n$\nClick `OK` to remove the previous version or `Cancel` to cancel this upgrade."
 
LangString DESC_VisitWebsite ${LANG_ENGLISH} "Visit Website"
 
LangString DESC_Help ${LANG_ENGLISH} "Help"
 
LangString DESC_Uninstall ${LANG_ENGLISH} "Uninstall"
 
LangString DESC_StartSysRestore ${LANG_ENGLISH} "Creating system restore point"
 
LangString DESC_ErrorSysRestore1 ${LANG_ENGLISH} "Error creating system restore point. Error code: $0"
 
LangString DESC_ErrorSysRestore2 ${LANG_ENGLISH} "Error closing system restore point. Error code: $0"
 
LangString DESC_FreeWareProgram ${LANG_ENGLISH} "This program is freeware. If you paid for a copy of this program, get a refund!"
 
;--------------------------------
; Installer
 
Function .onInit
 
  ;Language selection dialog
  ;!insertmacro MUI_LANGDLL_DISPLAY
 
FunctionEnd
 
Function MyGuiInit
 
  ReadRegStr $R0 HKLM \
  "Software\Microsoft\Windows\CurrentVersion\Uninstall\Little Disk Cleaner" \
  "UninstallString"
  StrCmp $R0 "" done
 
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  "$(DESC_AlreadyInstalled)" \
  IDOK uninst
  Abort
 
  ;Run the uninstaller
  uninst:
  ExecWait '$R0 _?=$INSTDIR' ;Do not copy the uninstaller to a temp file
 
done:
 
FunctionEnd
 
/***********************************************************************************************************/
/* THE CODE BELOW IS LICENSED UNDER THE CREATIVE COMMONS ATTRIBUTION NON-COMMERCIAL NO DERIVATIVES LICENSE */
Function WarnMsgBox
   MessageBox MB_OK|MB_ICONINFORMATION|MB_SETFOREGROUND "$(DESC_FreeWareProgram)"
FunctionEnd
/* THE CODE ABOVE IS LICENSED UNDER THE CREATIVE COMMONS ATTRIBUTION NON-COMMERCIAL NO DERIVATIVES LICENSE */
/***********************************************************************************************************/
 
; A prerequisite
!define NETVersion "3.5" ; Version to install
!define NETCheckVersion "3.5" ; Version to check for
!define NETInstaller "dotNetFx35setup.exe"
Section "Microsoft .NET Framework v${NETVersion}" SecFramework
  IfFileExists "$WINDIR\Microsoft.NET\Framework\v${NETCheckVersion}" NETFrameworkInstalled 0
  File /oname=$TEMP\${NETInstaller} ${NETInstaller}
 
  DetailPrint "Starting Microsoft .NET Framework v${NETVersion} Setup..."
  ExecWait "$TEMP\${NETInstaller}"
  Return
 
  NETFrameworkInstalled:
  DetailPrint "Microsoft .NET Framework v${NETVersion} is already installed!"
 
SectionEnd
 
; The stuff to install
Section "Little Disk Cleaner (required)" SecMain
  ; Section is read-only
  SectionIn RO
 
  ; Send data to Little Software Stats
  ; You could also use: NsisLittleSoftwareStats::TrackInstallation "http://stats.yourwebsite.com/api.xml" "xml" "f64e57760e1b49e93c1a1165a905575f" "1.0"
  NsisLittleSoftwareStats::TrackInstallation "http://stats.yourwebsite.com/api.xml" "xml" "f64e57760e1b49e93c1a1165a905575f" "${VERSION}"
  Pop $0
 
  ; Start system restore point
  DetailPrint "$(DESC_StartSysRestore)"
  SysRestore::StartRestorePoint /NOUNLOAD "Installed Little Disk Cleaner"
  Pop $0
  StrCmp $0 0 +2
  DetailPrint "$(DESC_ErrorSysRestore1)"
 
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
 
  ; Put file there
  File "gpl.txt"
  File "Little Disk Cleaner.exe"
  File "Little Disk Cleaner.exe.config"
  File "ChangeLog.txt"
 
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\Little Disk Cleaner" "Install_Dir" "$INSTDIR"
 
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Little Disk Cleaner" "DisplayName" "Little Disk Cleaner"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Little Disk Cleaner" "DisplayIcon" "$INSTDIR\Little Disk Cleaner.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Little Disk Cleaner" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Little Disk Cleaner" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Little Disk Cleaner" "NoRepair" 1
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Little Disk Cleaner" "InstallLocation" $INSTDIR
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Little Disk Cleaner" "Publisher" "Little Apps"
  WriteUninstaller "uninstall.exe"
 
  MessageBox MB_YESNO $(DESC_CreateShortcuts) IDNO noshortcut
     CreateShortcut "$DESKTOP\Little Disk Cleaner.lnk" "$INSTDIR\Little Disk Cleaner.exe" "" "$INSTDIR\Little Disk Cleaner.exe" 0
  noshortcut:
 
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
 
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(DESC_Uninstall).lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Little Disk Cleaner.lnk" "$INSTDIR\Little Disk Cleaner.exe" "" "$INSTDIR\Little Disk Cleaner.exe" 0
    WriteIniStr "$SMPROGRAMS\$StartMenuFolder\$(DESC_VisitWebsite).url" "InternetShortcut" "URL" "http://lildiskcleaner.sourceforge.net"
 
  !insertmacro MUI_STARTMENU_WRITE_END
 
  SysRestore::FinishRestorePoint
  Pop $0
  StrCmp $0 0 +2
  DetailPrint "$(DESC_ErrorSysRestore2)"
 
SectionEnd
 
;--------------------------------
; Uninstaller
 
Section "Uninstall"
  ; Section is read-only
  SectionIn RO
 
  ; Send data to Little Software Stats
  ; You could also use: NsisLittleSoftwareStats::TrackUninstallation "http://stats.yourwebsite.com/api.xml" "xml" "f64e57760e1b49e93c1a1165a905575f" "1.0"
  NsisLittleSoftwareStats::TrackUninstallation "http://stats.yourwebsite.com/api.xml" "xml" "f64e57760e1b49e93c1a1165a905575f" "${VERSION}"
  Pop $0
 
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Little Disk Cleaner"
  DeleteRegKey HKLM "SOFTWARE\Little Disk Cleaner"
 
  ; Remove files and uninstaller
  Delete "$INSTDIR\gpl.txt"
  Delete "$INSTDIR\Little Disk Cleaner.exe"
  Delete "$INSTDIR\Little Disk Cleaner.exe.config"
  Delete "$INSTDIR\ChangeLog.txt"
  Delete "$INSTDIR\uninstall.exe"
 
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
 
  ; Remove shortcuts, if any
  Delete "$DESKTOP\Little Disk Cleaner.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\*.*"
 
  ; Remove directories used
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  RMDir "$INSTDIR"
 
SectionEnd
 
;--------------------------------
; Section Descriptions
 
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecFramework} $(DESC_SecFramework)
!insertmacro MUI_FUNCTION_DESCRIPTION_END