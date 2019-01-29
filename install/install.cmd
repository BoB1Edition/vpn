@echo off
echo off

start /wait msiexec.exe /i "%CD%\anyconnect-win-4.5.04029-core-vpn-predeploy-k9.msi" /qn

start /wait avira_antivirus_ru-ru.exe

start /wait TeamViewer_Setup.exe /S
start /wait vc_redist.x64.exe /q /norestart
start /wait vc_redist.x86.exe /q /norestart
del "C:\Program Files (x86)\Cisco\Cisco AnyConnect Secure Mobility Client\vpnui.exe"

copy accisco*.dll c:\windows\
mkdir "C:\Program Files (x86)\ATH"
cacls "C:\Program Files (x86)\ATH" /g everyone:f
copy ath.vpn.ui.exe C:\Users\Public\Desktop
copy ATHfwconfig.config "C:\Program Files (x86)\ATH"
copy ath.vpn.service.exe "C:\Program Files (x86)\ATH"
cacls "C:\Program Files (x86)\ATH" /t /g everyone:f  /e /c
cacls "C:\Program Files (x86)\ATH" /t /g Все:f /e /c
"C:\Program Files (x86)\ATH\ath.vpn.service.exe" install
shutdown -r -t 60 -f
echo on
@echo on