# sys-clk

Switch sysmodule allowing you to set cpu/gpu/mem clocks according to the running application and docked state.

## Installation

The following instructions assumes you have a Nintendo Switch running Atmosphère, updated to at least the latest stable version.
Copy the `config` and `atmosphere` folder at the root of your sdcard, overwriting files if prompted.

## Relevant files

* Config file: allows one to set custom clocks per docked state and title id, described below

	`/config/sys-clk/config.ini`

* Log file: where the log are written if enabled

	`/config/sys-clk/log.txt`

* Log flag file: enables log writing if file exists

	`/config/sys-clk/log.flag`

## Config

Presets can be customized by adding them to the ini config file located at `/config/sys-clk/config.ini`, using the following template for each app:

```
[Application Title ID]
docked_cpu=
docked_gpu=
docked_mem=
handheld_charging_cpu=
handheld_charging_gpu=
handheld_charging_mem=
handheld_charging_usb_cpu=
handheld_charging_usb_gpu=
handheld_charging_usb_mem=
handheld_charging_official_cpu=
handheld_charging_official_gpu=
handheld_charging_official_mem=
handheld_cpu=
handheld_gpu=
handheld_mem=
```

* Replace `Application Title ID` with the title id of the game/application you're interested in customizing.
A list of games title id can be found in [Switchbrew wiki](https://switchbrew.org/wiki/Title_list/Games).
* Frequencies are expressed in mhz, and will be scaled to the nearest possible values, described in the clock table below.
* If any key is omitted, value is empty or set to 0, it will be ignored, and stock clocks will apply.
* If charging, sys-clk will look for the frequencies in that order, picking the first found:
	- Charger specific config (USB or Official): `handheld_charging_usb_X` or `handheld_charging_official_X`
	- Non-specific charging config: `handheld_charging_X`
	- Handheld config: `handheld_X`

### Example 1: Zelda BOTW

* Overclock CPU when docked or charging
* Overclock MEM to docked clocks when handheld

Leads to a smoother framerate overall (ex: in the korok forest)

```
[01007EF00011E000]
docked_cpu=1224
handheld_charging_cpu=1224
handheld_mem=1600
```

### Example 2: Picross

* Underclocks on handheld to save battery

```
[0100BA0003EEA000]
handheld_cpu=816
handheld_gpu=153
handheld_mem=800
```

## Clock table (MHz)

### RAM clocks
* 1600 -> official docked, max clock
* 1331 -> official handheld
* 1065
* 800
* 665

### CPU clocks
* 1785 → max clock, will cause instability in certain configurations
* 1683
* 1581
* 1428
* 1326
* 1224 → sdev oc
* 1122
* 1020 → official docked & handheld
* 918
* 816
* 714
* 612

### GPU clocks
**Notes:**  
-GPU overclock is capped at 460 if handheld  
-Clocks higher than 768MHz need the official charger
* 921 → max clock
* 844
* 768 → official docked
* 691
* 614
* 537
* 460 → max handheld
* 384 → official handheld
* 307 → official handheld
* 230
* 153
* 76