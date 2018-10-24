;#ifndef EVENT_LOG_FILE_H_
;#define EVENT_LOG_FILE_H_

MessageIdTypeDef=DWORD


SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
               )

LanguageNames=(
    Neutral=0x0000:MSG00000
               )

MessageId=0x100   
SymbolicName=MSG_INFO_1
Severity=Informational
Facility=Application
Language=Neutral
%1
.


;#endif