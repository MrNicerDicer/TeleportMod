# TeleportMod - DEV DON'T USE

A simple teleport mod for DayZ that can reliably connect locations such as bunkers placed outside the map — or any other custom area. Originally created for use on a private server, the mod includes optional item-based activation and the ability to consume the item upon use (e.g. keycards).

## Planned Features
- Teleport delay
- Fade-in/fade-out transitions
- Sound/Alarm effects during teleportation
- Better visible notification

---

## Features

- Interactive teleport points attachable to any object
- Item requirements (e.g. crowbar, shovel, keycards)
- Item damage or consumption system
- Temporary teleport activation (time-limited use)
- Sound effects (work in progress – not fully implemented)

---

## Example Configuration

```json
{
    "TeleportEntries": [
        {
            "EnableTeleport": 1,
            "TeleportName": "Barn Door",
            "ObjectType": "Land_Door_germa_1",
            "ObjectCoordinates": [7758.640137, 16.184601, 14116.900391],
            "ObjectOrientation": [0.0, 0.0, 0.0],
            "TeleportPosition": [10.299805, 10.765999, 10.270020],
            "CheckRadius": 5.0,
            "RequiredItem": "Crowbar",
            "RequiredItemDamagePercent": 50.0,
            "ConsumeRequiredItem": 0,
            "TeleportActiveTimeSeconds": 30,
            "MissingItemMessage": "You need a Crowbar!",
            "EnableTeleportSound": 1,
            "TeleportSoundFile": "TeleportMod\\Data\\teleport_sound.ogg",
            "TeleportSoundRadius": 50.0,
            "TeleportSoundVolume": 1.0
        }
    ]
}
```

## Configuration Parameters

| Parameter                    | Description                                                  |
|------------------------------|--------------------------------------------------------------|
| `EnableTeleport`            | Enables or disables the teleport (1 = true, 0 = false)     |
| `TeleportName`              | Name of the teleport for better organization                |
| `ObjectType`                | Object type that triggers the teleport                      |
| `ObjectCoordinates`         | Coordinates of the teleport trigger                         |
| `ObjectOrientation`         | Orientation (yaw, pitch, roll) of the trigger object        |
| `TeleportPosition`          | Destination coordinates of the teleport                     |
| `CheckRadius`               | Radius within which the teleport can be activated           |
| `RequiredItem`              | Item class name needed for activation                       |
| `RequiredItemDamagePercent` | Damage applied to the item on use (0.0–100.0)              |
| `ConsumeRequiredItem`       | Whether the item is destroyed on use (1 = true, 0 = false) |
| `TeleportActiveTimeSeconds` | Duration (in seconds) the teleport remains active           |
| `MissingItemMessage`        | Message displayed when required item is missing             |
| `EnableTeleportSound`       | Enable teleport sound effects (1 = true, 0 = false)        |
| `TeleportSoundFile`         | Path to the sound file                                      |
| `TeleportSoundRadius`       | Range of the teleport sound                                 |
| `TeleportSoundVolume`       | Volume of the teleport sound                                |

---

## Usage

1. Place objects in your map that will serve as teleport triggers
2. Configure their coordinates and settings in the `config.json`
3. Players can interact with these objects to teleport (if they have the required item)
4. Items can be damaged or consumed based on your configuration

---

## Credits & Repack

Based on the [ActionTeleport mod](https://steamcommunity.com/sharedfiles/filedetails/?id=3405070168) by FreddyCruger and kenik.  
This version includes an improved configuration format and extended features.

---

## License & Usage

**This mod is 100% free** – just like the original ActionTeleport mod:

- ✅ Free to use on any server  
- ✅ Free to modify and repack  
- ✅ Free to redistribute  

---

## Please Consider

- Leaving a like or comment if you find the mod useful  
- Supporting the original [ActionTeleport mod](https://steamcommunity.com/sharedfiles/filedetails/?id=3405070168)  
- Giving proper credit if you modify or repack the mod

---

## Changelog

### Version 1.0.0
- Initial release based on ActionTeleport
- Improved configuration format
- Extended parameter options
- Enhanced documentation
