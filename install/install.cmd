@echo off
ath.vpn.service.exe install
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
copy ath.vpn.ui.exe C:\Users\Default\Desktop
copy ATHfwconfig.config c:\windows\system32\

rem shutdown -r -t 0 -f
@echo on
