vc_redist.x64.exe /q /norestart
vc_redist.x86.exe /q /norestart
"%CD%\kav\startup.exe" /s /pAGREETOEULA=1 /pSKIPPRODUCTCHECK=1 /pJOINKSN=1 /recommended
msiexec /i "%CD%\anyconnect-win-4.5.04029-core-vpn-predeploy-k9.msi" /qn
TeamViewer_Setup.exe /S
"C:\Program Files (x86)\Kaspersky Lab\Kaspersky Free 19.0.0\avp.com" UPDATE
del "C:\Program Files (x86)\Cisco\Cisco AnyConnect Secure Mobility Client\vpnui.exe"
net user /ADD ATHUser Ath2018
net user /ADD ATHAdmin u-5iqNO47JfC
net LOCALGROUP Администраторы athadmin /add
net LOCALGROUP Administrators athadmin /add
copy accisco*.dll c:\windows\
mkdir "C:\Program Files (x86)\ATH"
cacls "C:\Program Files (x86)\ATH" /g everyone:f
copy ath.vpn.ui.exe C:\Users\Public\Desktop
copy ATHfwconfig.config "C:\Program Files (x86)\ATH"
copy ath.vpn.service.exe "C:\Program Files (x86)\ATH"
cacls "C:\Program Files (x86)\ATH" /t /g everyone:f  /e /c
cacls "C:\Program Files (x86)\ATH" /t /g Все:f /e /c
"C:\Program Files (x86)\ATH\ath.vpn.service.exe" install
shutdown -r -t 30 -f
