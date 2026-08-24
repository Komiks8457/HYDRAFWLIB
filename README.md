***Just another project for my c++ training.***

Nothing special on this repo, just basic hookings for starters :P

[Download](https://drive.google.com/file/d/1u_irZD2GZZBeZ1onSZRWE2HyKRSMAoSA/view?usp=sharing) this patched modules that loads hydrafwlib.dll.

```text
Follow this structure when using vanguard and you dont have to inject dll's individually.
HydraFW load's vanguard dll's automatically for each proper modules.
 
C:/silkroad/
├── Vanguard/
│   ├── plugins/
│   ├── GameServer.dll
│   ├── GameServer.ini
│   ├── GlobalManager.dll
│   ├── GlobalManager.ini
│   └── ...
├── GlobalManager.exe
├── hydrafwlib.dll
├── SR_GameServer.exe
└── ...
```

```text
HyrdaFW config file (hydrafwlib.ini)

[GlobalManager]
;True = will not show the mainview window, default false.
HideMainView=true

;Default to false
ShowConsole=true

;Output's the module log message to the console if available, default false.
OutputLogs=true

;Loads another DLL right after hydrafwlib.dll is loaded.
PluginDLL=path\to\your.dll

[SR_GameServer]
...
[SR_ShardManager]
...
same keys with from above example.

;
[DbUserInfo]
UID=userid use to login for database
PWD=userpw use to login for database
```
