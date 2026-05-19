# PhantomKiller

weaponizing a signed lenovo kernel driver to terminate any process — including EDR/AV protected processes.

## overview

PhantomKiller abuses `BootRepair.sys`, a legitimate lenovo driver shipped with Lenovo PC Manager. the driver exposes a device object (`\\.\BootRepair`) with no DACL restrictions and a single IOCTL (`0x222014`) that takes a 4-byte PID and calls `ZwTerminateProcess`, no access checks, no caller validation, no protection.

**full writeup:** [Phantom Killer — Reverse Engineering and Weaponizing a Lenovo Driver to Terminate EDR Processes](https://medium.com/@jehadbudagga/phantom-killer-reverse-engineering-and-weaponizing-a-lenovo-driver-to-terminate-edr-processes-9191cd06374f)

## driver details

| field | value |
|-------|-------|
| file name | `BootRepair.sys` |
| sha256 | `5ab36c116767eaae53a466fbc2dae7cfd608ed77721f65e83312037fbd57c946` |
| signer | LENOVO (Symantec Class 3 SHA256 Code Signing CA) |
| compiled | 2018-01-03 |
| arch | x64 |
| VT detections | 0/71 at time of discovery |

## vulnerability summary

- device object created without secure DACL — any user can open a handle
- `IRP_MJ_CREATE` (MajorFunction[0]) has no access checks
- `IRP_MJ_DEVICE_CONTROL` (MajorFunction[14]) accepts IOCTL `0x222014`
- input: 4-byte `DWORD` (target PID)
- internally calls `PsLookupProcessByProcessId` → `ObOpenObjectByPointer` → `ZwTerminateProcess`
- kills any process including PPL-protected AV/EDR processes

## attack scenarios

**driver already loaded:** any low-privileged user can open the device and terminate any process on the system.

**BYOVD:** an attacker loads the signed driver via `sc.exe` or similar, then uses it to kill EDR processes before deploying post-exploitation tools.

## usage

```
sc.exe create PhantomKiller binPath="C:\Path\to\BootRepair.sys" type=kernel
sc.exe start PhantomKiller
```

```
PhantomKiller.exe <pid>
```



## disclaimer

this project is for **educational and authorized security research purposes only**. do not use this against systems you do not own or have explicit permission to test. the author is not responsible for any misuse.

## author

**j3h4ck** — [@j3h4ck](https://twitter.com/j3h4ck) | [linkedin](https://www.linkedin.com/in/jehadabudagga/) | [medium](https://medium.com/@j3h4ck)
