# Asset Customization

This document describes how to customize asset by dropping metadata, sprites and texture files next to the original game files.

For the metadata itself, its fields and every resource type it can describe, see [Metadata Format](Metadata.md).  
For the binary layout of the formats mentioned here (`.opi`/`.opa`, `.ojs` family, `ControlList_*.txt`, `ItemData.dat`, …),
see [File Format Documentation](../file-format/FileFormat.md).

## Overview

The client implements layered IO mechanism to reads the game assets, and it never modifies them.

- All assets, including the master archives (`Interface(1).opi`, `Playing(1).opi` and `Avatar.opa`) are treated as read-only.
- Patch packages are **overlay assets** on top of existing assets, not merged.
    - This is a different semantic than the original client; it never applies a patch into `Interface(1).opi`, `Playing(1).opi`, or `Avatar.opa`.
- Overrides are extra files. A customization is always a *new* file placed next to the originals, so repacking or patching an existing file is never needed.

This has two benefits that are the whole point of the design:

- **Original files stay pristine**  
  There is nothing to back up. The master archives retain their original content before and after any amount of modding.

- **Customization removal is trivial**  
  Removing a mod means deleting the extra files you added. There is no backup involved since nothing was replaced.

> [!IMPORTANT]
> Traditional customization by repacking the master archives is supported but highly discouraged.
>
> Everything in this document can be done additively. If a customization traditionally appears to require repacking `Interface(1).opi`, `Playing(1).opi`, or `Avatar.opa`,
> it can almost certainly be expressed as an override file or a patch package instead.
>
> The overall architecture is designed to encourage you to preserve the pristine state of your original O2Jam files.

---

## Asset Pipeline

Every file the game reads is looked up through a chain of sources.
The first source that holds the file answers the request, and the order is fixed at start-up:

| Priority | Source            | Contents                                                                                           |
|----------|-------------------|----------------------------------------------------------------------------------------------------|
| 1        | Local File System | Files on local disk. It comes before everything else, so **a local file always takes priority**.   |
| 2        | Patch packages    | `Interface*`, `Playing*` and `Avatar*` patch archives, ordered by patch number, highest first.     |
| 3        | Master archives   | `Interface(1).opi`, `Playing(1).opi`, `Avatar.opa`.                                                |
| 4        | Music banks       | `BGM.ojm`, `Event.ojm`, `bgEffect.ojm`, `Planet.ojm`, `O2PlanetNPC.ojm`.                           |
| 5        | Embedded Files    | In-memory assets, mostly the `ControlList/**.json` scene composition compiled into the executable. |

Because local files come first, **overriding any asset is a matter of placing a file with the matching name in one of the asset roots**.

### Asset Roots

A relative file name is resolved against each asset root, in the following order:

| Order | Root           | Notes                                                                                                  |
|-------|----------------|--------------------------------------------------------------------------------------------------------|
| 1     | `./`           | The working directory, i.e. the game folder that holds the executable.                                 |
| 2     | `./assets`     | Mainly used for debugging and development purposes.                                                    |
| 3     | `./Image`      | The original client's image folder.                                                                    |
| 4     | `./Image/TEMP` | The original client's patch staging folder. See [Patch Packages](#patch-packages-and-the-temp-folder). |
| 5     | `./Music`      | The original client's music folder.                                                                    |

Any of these roots is a valid place for an override.

### Path Prefixes

Each archive is mounted under a prefix that acts as its namespace. 
When a request matches the prefix of one of the archive, the prefix is stripped before the archive is searched.
Local files are looked up with the full name as written.

| Prefix         | Archive                            |
|----------------|------------------------------------|
| `Interface/`   | `Interface(1).opi` and its patches |
| `Playing/`     | `Playing(1).opi` and its patches   |
| `Avatar/`      | `Avatar.opa` and its patches       |

For example, when the game requests `Interface/State/Avi.json`, it tries the following in order:

```
./Interface/State/Avi.json                 <- local file (working directory)
./assets/Interface/State/Avi.json          <- local file (asset root)
./Image/Interface/State/Avi.json           <- local file (image root)
./Image/TEMP/Interface/State/Avi.json      <- local file (staging image root)
Interface_NNN : State/Avi.json             <- patch package (prefix stripped)
Interface.opi : State/Avi.json             <- master archive (prefix stripped)
Internal      : Interface/State/Avi.json   <- embedded resources
```

> [!NOTE]
> Name matching for local files follows the host filesystem: case-insensitive on Windows, case-sensitive on Linux and macOS.
> Matching the archive's spelling exactly is the best practice to keep a mod portable.

---

## Interop Mode

The game enters "Interop mode" when it is dropped into an original O2Jam installation.
Three markers are probed, and each scope is decided on its own:

| Scope     | Condition                          | Interop (original assets, condition met)                                                                                   | Standalone (custom assets)                                     |
|-----------|------------------------------------|----------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------|
| Interface | `ControlList_Interface.txt` exists | Scene composition from `ControlList/**.json`, sprites from `Interface(1).opi`                                              | Everything from `Interface/**.json`                            |
| Playing   | `ControlList_Playing.txt` exists   | Scene composition from `ControlList/State/Loading.json`, `ControlList/State/Playing7K.json`, sprites from `Playing(1).opi` | `Interface/State/Loading.json`, `Playing/State/Playing7K.json` |
| Avatar    | An `Itemdata*.dat` exists          | Items built from `Avatar/ItemData.json`, or from the `Itemdata*.dat` from `Avatar.opa` when it is absent                   | Items built from `Avatar/ItemData.json`                        |

In interop mode, the game assumed to use the `ControlList/**.json` files that are embedded to the executable, which is a thin metadata layer that maps over the original `ControlList_*.txt` from the master archive.  

This mean the embedded metadata assume to use texture and hit-tests from `.ojs` sprite files, `.bnd` files and the likes from the master archives that follows composition from the `ControlList_*.txt`.

Loading the master archives is not mandatory. If they fail to load, the client keeps running on whatever local assets are present,
which is what makes a complete standalone asset set possible.

Regardless of whether the game runs in interop or standalone mode, every sprite and metadata file can be overridden on its own.

> [!NOTE]
> The three markers are probed once at start-up.
> Adding or removing `ControlList_Interface.txt`, `ControlList_Playing.txt` or an `Itemdata*.dat` changes which composition the client uses.
> 
> If the game is running, restart the game after such a change.

---

## Overriding the assets

The following section describe how you can implement the override.
There are three levels of customization available, in increasing level of customization and complexity.

### 1. Resource Override

Drop a file whose name matches an archive entry into any asset root. The local file takes the priority, and nothing else has to change:

```
<game>/<asset root>/Btn_Login.ojs
```

For example:
```
./Image/Btn_Login.ojs
```

The replacement must use the same format as the original resource it override. 
For example, the file has to be a valid [O2Jam sprite](../file-format/FileFormat.md#o2jam-sprites) file when overriding `.ojs` file.

### 2. Metadata Override

Every node in the scene composition tree could be expressed in one metadata file, addressed by the path in its parent's `objects` map.
Create a file at that same path in an asset root, and it will replaces the metadata that embedded inside the executable.

For example, `ControlList/State/Planet/Planet_Btn_Exit.json` is overridden by:

```
<game>/<asset root>/ControlList/State/Planet/Planet_Btn_Exit.json
```

A metadata that uses [interop references](Metadata.md#interop-references) typically looks like this:

```json
{
  "type": "Button",
  "require": {
    "refID": "0X19200007",
    "bndID": "0X19200007"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 640,
        "y": 12
      }
    }
  }
}
```

The three fields it carries, and every resource type you can put in `type`, are documented in [Metadata Format](Metadata.md).  
The keys used above belong to the [interop references](Metadata.md#interop-references), which read the original control list (to infer the [O2Jam sprite](../file-format/FileFormat.md#o2jam-sprites) file to use) and [boundary tables](../file-format/FileFormat.md#o2jam-boundary) (for frame positions and hit-tests).

### 3. Metadata + Resource Override

Swapping the resource file is accomplished with an edit of the same metadata: replace the interop reference with a [custom reference](Metadata.md#custom-references).
Drop the image anywhere in an asset root:

```json
{
  "type": "Button",
  "require": {
    "texture": "Interface/Custom/Btn_Exit.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 10,
        "y": 550
      }
    },
    "states": {
      "normal": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 60,
          "height": 24
        }
      },
      "hover": {
        "texCoords": {
          "x": 60,
          "y": 0,
          "width": 60,
          "height": 24
        }
      },
      "click": {
        "texCoords": {
          "x": 120,
          "y": 0,
          "width": 60,
          "height": 24
        }
      }
    }
  }
}
```

The above example assume the texture file located at `<game>/Image/Interface/Custom/Btn_Exit.png`.

Notes when using custom textures:

- An image file has no frame table, so you have to define the frame layout. See [Source Precedence](Metadata.md#source-precedence) for which reference takes priority when several are present.
- When `bndID` specified, it supplies the position and the hit-test size from the original boundary table. 
  - Alternatively, provides the object with an explicit `transform.position`, and `bounds` / `texCoords` where the type supports it as shown above.
- Keep the `refID` only while you rely on the archived sprite. It is ignored once `texture` resolves as it takes priority.
  - Alternatively, you can keep using `refID` without `bndID` and provides custom position, texCoords and/or bounds as mentioned and shown above.

### Fonts, Cursors and Sounds

Fonts are resolved from the system font set at start-up and published under `Interface/Common/Font.ttf`, `Font.Bold.ttf` and `Fallback-Font1..3.ttf`.
Since embedded files come last, dropping `<asset root>/Interface/Common/Font.ttf` replaces the game font everywhere without touching anything else.

The cursor is a regular metadata file, `ControlList/Window_Cursor.json` in interop mode and `Interface/Common/Window_Cursor.json` otherwise.
It is overridden the same way; see [Cursor](Metadata.md#cursor) for its fields.

---

## Overriding Avatars

### Avatar asset pipeline

An avatar item is assembled from an item table and a set of sprite references:

1. The item table is located at start-up. `Avatar/ItemData.json` is gets the priority when it exists, otherwise the first `Itemdata*.dat` found across every source, asset roots before archives.
   The item sets follow the same rule: `Avatar/SetInfoData.json` first, then `setinfodata.*`.
2. Each entry carries the item identity, its pricing, gender, planet of origin, equipment type, thumbnails, and one sprite reference per gender, render part and instrument.
3. Thumbnails become [Sprite](Metadata.md#sprite) resources and the renderable parts become looping [Animation](Metadata.md#animation) resources,
   so both accept the same `require` keys as the interface.

When the table is the binary `ItemData.dat`, every reference in it names an O2Jam sprite.
If that name is not found anywhere, its extension is normalized to `.ojs` before the lookup is retried.

### Sprite-Level Swap

The table references entries by their name inside `Avatar.opa`, so an avatar sprite is replaced exactly like an interface sprite:
a local file of the same name in `Image/`, or any other asset root, gets the priority first.
The replacement has to be an O2Jam sprite, because the binary table has no place to describe an image file.

### Custom Item Table

For full control over new items, image art and custom thumbnails, supply the item table either using `Itemdata*.dat` or `Avatar/ItemData.json`,
resolved through the same asset roots as everything else, e.g. `<game>/Image/Avatar/ItemData.json` or `<game>/Image/ItemData_TICT.dat`.

The `SetInfoData*` behaves in the same way. However, please keep in mind that the metadata json files are prioritized over the native binary table.

For metadata in json format, the table is keyed by item ID, and each entry is an [Item](Metadata.md#item) metadata.
See [ItemData](Metadata.md#itemdata) for the table itself and [SetInfoData](Metadata.md#setinfodata) for the shop sets that bundle those items.

```json
{
  "type": "ItemData",
  "require": {
    "items": {
      "1001": {
        "type": "Item",
        "require": {
          "thumbnail": {
            "small": "Avatar/Custom/Hair_Thumb_S.json",
            "large": "Avatar/Custom/Hair_Thumb_L.json"
          },
          "female": {
            "body": {
              "default": "Avatar/Custom/Hair_Idle.json"
            }
          }
        },
        "attributes": {
          "id": 1001,
          "isNew": true,
          "name": "Custom Hair",
          "description": "Example item",
          "gender": "Female",
          "origin": "O2Planet",
          "type": "Hair",
          "price": {
            "gem": 1200
          }
        }
      }
    }
  },
  "attributes": {
    "version": "3.10"
  }
}
```

Every reference inside an item names a metadata of its own, a [Sprite](Metadata.md#sprite) for the thumbnails and an [Animation](Metadata.md#animation) for the renderable parts.
That is where the texture is chosen, and the tables can be decoupled from such details (i.e, they are not inlined); an item made of image files and an item made of O2Jam sprites differ only in those files, and of course, they can be mixed.

### Default Avatar (Standalone Mode)

When no `Itemdata*.dat` is present, the starting avatar is composed from fixed paths, each of them optional:

```
Avatar/default/Body.json
Avatar/default/LeftArm.json
Avatar/default/LeftHand.json
Avatar/default/RightArm.json
Avatar/default/RightHand.json
Avatar/default/{male|female}/{Face,Hair,Top,Pants,Shoes}.json
```

> [!IMPORTANT]
> When an `Itemdata*.dat` exists, the starting avatar is built from item IDs `30`–`34` plus `35` for male or `36` for female. 
> Custom `Avatar/ItemData.json` that replaces the original table MUST define the default files above.
> 
> This because the above default item json metadata are NOT part of the default embedded resources.

---

## Patch Packages

### Original semantics

In the original client, `O2JamPatchClient.exe` downloads `.opi`/`.opa` patch packages into `Image/TEMP`, and the main client (`OTwo.exe`) merges each one into the target master archive and removes it.
The master archive is rewritten in place. See [Asset Patching](../file-format/FileFormat.md#asset-patching).

### New semantics

This client merges nothing. `Image/` and `Image/TEMP/` are ordinary asset roots, and at start-up the client scans them for patch packages,
then **mounts each patch above the master archive** in the same namespace.

The result:

- Patches that were downloaded but never applied by the original patcher still take effect.
- The master archive is never touched, so it stays pristine no matter how many patches accumulate.
- Deleting a patch file is a complete rollback of that patch.

### Naming Rules

A file is treated as a patch package only if it matches the original patch convention:

```
<Target>[Archive]_<Patch Number>.<opi|opa>
```

| Part           | Rule                                                                                        |
|----------------|---------------------------------------------------------------------------------------------|
| `Target`       | `Interface`, `Playing`, or `Avatar`.                                                        |
| `Archive`      | Optional, digits only. It is the numeral suffix some distributions use, as in `Interface1`. |
| `_`            | Required separator.                                                                         |
| `Patch Number` | Required, digits only, at most 9 digits. Higher number gets prioritized.                    |
| Extension      | `.opi` or `.opa`; both are accepted for every target.                                       |

Matching is case-insensitive and considers the file name only.

| Example               | Result                                    |
|-----------------------|-------------------------------------------|
| `Interface1_300.opi`  | Mounted over `Interface(1).opi`           |
| `Playing_412.opi`     | Mounted over `Playing(1).opi`             |
| `Avatar1_7.opa`       | Mounted over `Avatar.opa`                 |
| `Interface1.opi`      | Not a patch. This is a master archive.    |
| `InterfaceMod_1.opi`  | Ignored. The archive part is not numeric. |
| `Interface1_300b.opi` | Ignored. The patch number is not numeric. |
| `Interface1_.opi`     | Ignored. The patch number is empty.       |

> [!NOTE]
> Patch discovery follow the same registered asset roots and not limited to the `TEMP` folder.
> However, the `Image/` and `Image/TEMP/` are the conventional locations used by the original client.

### Ordering

Patches for one target are mounted in descending patch number, so the **highest patch number is prioritized** and all patches outrank the master archive.
Local files still override every patch.

Effective priority for a single target:

```
local file  >  patch 412  >  patch 300  >  master archive
```

### Shipping custom assets as a Patch Package

A patch is an archive mounted above the master, so packaging a mod as `Interface1_1000.opi` is a valid way to distribute it.
It keeps many overrides in one file, installs by copying, and uninstalls by deleting.

Using a high patch number is recommended to stays above the official patches.
Loose metadata and image files remain the more flexible option, since they can also *add* files the archive never had.

---

## Layout example

```
O2Jam/                                  <- working directory
├─ Image/
│  ├─ ControlList/
│  │  └─ State/Planet/Planet_Btn_Exit.json    <- metadata override
│  ├─ Interface/
│  │  ├─ Common/Font.ttf                      <- font override
│  │  └─ Custom/Btn_Exit.png                  <- texture asset
│  ├─ Avatar/
│  │  ├─ ItemData.json                        <- custom item table, replaces Itemdata*.dat when present
│  │  └─ Custom/Hair_Idle.png
│  ├─ TEMP/
│  │  └─ Interface1_300.opi                   <- patch package, mounted, never merged
│  ├─ Interface1.opi                          <- pristine
│  ├─ Playing1.opi                            <- pristine
│  ├─ Avatar.opa                              <- pristine
│  └─ Btn_Login.ojs                           <- sprite override (same name as archive entry)
├─ Music/
└─ OTwo.exe                             <- CXO2, in place of the original binary (using different name is perfectly fine)
```

## Removing a Customization

As previously mentioned, removing customizations is as simple as deleting the files you added. 
That is the entire procedure. There is no archive to restore, no patch to roll back, and no state left behind:

| Customization           | Removal                                                                                                                         |
|-------------------------|---------------------------------------------------------------------------------------------------------------------------------|
| Sprite/texture override | Delete the image and/or sprite file                                                                                             |
| Metadata override       | Delete the metadata file; the embedded original takes over again                                                                |
| Custom item table       | Delete `Avatar/ItemData.json` and/or `Avatar/SetInfoData.json`; the binary `Itemdata*.dat` / `SetInfoData.ojs` takes over again |
| Patch package           | Delete the `.opi`/`.opa` patch file                                                                                             |
