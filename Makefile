all: sms

sms.obj: sms.cpp
    cl /c /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D WIN32 /D NDEBUG /D _CONSOLE /EHsc /MT /Gy /Gd /TP sms.cpp

sms: sms.obj
    link kernel32.lib user32.lib shell32.lib /out:sms.exe /LTCG sms.obj

clean:
    @del /q sms.obj
    @del /q sms.exe