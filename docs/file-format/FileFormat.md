# O2Jam File Format Documentation

This document provides a comprehensive reference for every file format used by the O2Jam game client. It describes how the game read and use every assets in the game client.

Existing documentation that has already been published elsewhere is linked rather than duplicated.

## Conventions

Unless stated otherwise, the following conventions apply throughout this document:

- All formats use **little-endian** byte order.
- All strings are **null-terminated**.
- **String encoding varies by client distribution.** For example, e-Games clients commonly use standard `ASCII`, while mgame and NOWCOM (Korea) clients use `EUC-KR`.

## O2Jam Content Structure

A typical O2Jam game client directory is structured as follows:

```
O2Jam/
├─ Image/
│  ├─ (TEMP/)
│  ├─ Interface(1).opi
│  ├─ Playing(1).opi
│  ├─ Avatar.opa
│  ├─ OJNList.dat (or X2OJNList.dat for X2 client)
├─ Music/
│  ├─ (TEMP/)
│  ├─ BGM.ojm
│  ├─ BgEffect.ojm
│  ├─ Planet.ojm
│  ├─ O2PlanetNPC.ojm
│  ├─ (BGMWinter.ojm)
│  ├─ Tutorial.ojn
│  ├─ Tutorial.ojm
│  ├─ o2ma100.ojn
│  ├─ o2ma100.ojm
│  ├─ o2ma101.ojn
│  ├─ o2ma101.ojm
│  ├─ o2ma...
├─ OTwo.exe
├─ O2Jam.exe
├─ O2JamPatchClient.exe
├─ (O2JamLauncher.exe)
```

<sup>Parenthesis denotes optional.</sup>

### Asset Patching

Temporary folders (`TEMP/`) inside both `Image` and `Music` hold patch files that have yet to be applied by the game client.

#### Image Patch

Image patch files typically use `.opi` or `.opa` format depending on the target archive. See [Package Interface and Avatar](#o2jam-package-interface--avatar) for the format specification. Patch files are downloaded by `O2JamPatchClient.exe`, not the game client itself.

The patch package file contains sprite or metadata files that will be merged into the target package. If a file already exists in the target package, the patch overrides it. The game will **not** process [sprite files](#o2jam-sprites) and metadata files directly; the patch file must be a package file.

Patch filenames use the following format:

```
<Target>(1)_<Patch Number>.<Extension>
```

For example: `Playing1_300.opi`

- `Target`: Valid targets are `Interface`, `Playing`, and `Avatar`.
- `Patch Number`: Used to differentiate multiple patch files. By convention, this is the patch build version.
- `Extension`: Either `opi` or `opa`.

#### Music Patch

Unlike image patches, music patches are downloaded by the main client (`OTwo.exe`) rather than the game patcher. The `TEMP/` folder holds in-progress download files for both [OJN](#o2jam-note) and [OJM](#o2jam-music) files, typically via Music Shop. Once a download completes, the files are moved out of `TEMP/`.

In addition to the [game music files](#music), the client persists download state via [INI file](https://en.wikipedia.org/wiki/INI_file) until the requested files are fully downloaded (or the download fails). The filename follows the pattern `o2ma<music_id>.ini`:

```ini
[FileInfo]
FileNum=2
FileName1=o2ma100.ojn
FileStatus1=0
FileName2=o2ma100.ojm
FileStatus2=0
```

---

## Image

The `Image` directory holds all visual assets used by the game client: UI elements, avatar sprites, and music list metadata.

### O2Jam Package Interface & Avatar

OPI (O2Jam Package Interface) and OPA (O2Jam Package Avatar) are archive container formats that hold multiple sprite resources and metadata files. Both use the same structure; the only difference is the archive signature value: `01` for OPA and `02` for OPI.

Typically, the client ships with `Interface.opi` (or `Interface1.opi`), `Playing.opi` (or `Playing1.opi`), and `Avatar.opa`. Some distributions (notably e-Games and NOWCOM) use names with a numeral suffix (`Interface1.opi`, `Playing1.opi`), while others (such as GAMANIA or 9you) use names without it. The files are identical in structure; the naming is a distribution-level difference.

- The **Interface** archive contains all non-gameplay UI sprites (login screen, server/channel select, music select, etc.).
- The **Playing** archive contains gameplay-related sprites (note skins, judgement indicators, combo numbers, gauge, etc.).
- The **Avatar** archive contains character avatar sprites and related item visuals.

In addition to visual assets, each archive contains one or two metadata files. The game may reference these by index in the file header list rather than by filename, though the client may still verify the filename before parsing. See [Control List](#o2jam-control-list) and [Item Data](#o2jam-item-data-table) for details.

Certain archives contain legacy files that are no longer used:

- `OJNList.dat` in `Playing(1).opi`: May have been used by the v3.10 client, as `OJNList.dat` is absent from the `Image/` directory in that version.
- `JamTable.dat` in `Playing(1).opi`: Purpose unknown; the client version that used it is unclear.
- `O2-x001.ojs` in `Avatar.opa`: Contains an item data table with the same structure as `ItemData.dat`.

#### Archive File Format

**Archive header**

| Offset | Type | Description |
|--------|------|-------------|
| 0 | `int32` | Archive Signature |
| 4 | `int32` | File count |
| 8 | `int32` | (Unused) |
| 12 | `int32` | (Unused) |
| 16 | `bytes[N]` | File contents |
| 16 + N | `bytes[152 × File count]` | File headers |

**File header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | File Signature |
| +4 | `char[128]` | Filename |
| +132 | `int32` | Data offset |
| +136 | `int32` | File Size 1\* |
| +140 | `int32` | File Size 2\* |
| +144 | `int32` | (Unused) |

<sup>\* Prefer File Size 1, or use whichever is greater between File Size 1 and File Size 2.</sup>

**Archive signature values**

| Value | Description |
|-------|-------------|
| 1 | OPA |
| 2 | OPI |

**File signature values**

| Value | Description |
|-------|-------------|
| 0 | Unused |
| 1 | Valid |

---

### O2Jam Sprites

O2Jam sprite files use one of these extensions: `.ojs`, `.oji`, `.ojt`, and `.oja`, with `.ojs` being the most common. A sprite file is a container holding multiple frame headers and raw bitmap data for each corresponding sprite frame. Bitmaps are stored as individual frames, not as a combined texture atlas.

All extensions share the same file format structure, but the extension may hint at how the game should use the sprite (e.g., custom blending mode, or rendering via Direct3D instead of DirectDraw). This is not always the case; the game can override this behavior.

#### Sprite File Format

**Sprite header**

| Offset | Type | Description |
|--------|------|-------------|
| 0 | `int16` | Signature |
| 2 | `int16` | Format |
| 4 | `int16` | Total frames |
| 6 | `int16` | Transparency Color Key |

**Frame header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int16` | X position of the sprite frame |
| +2 | `int16` | Y position of the sprite frame |
| +4 | `int16` | Width of the sprite frame |
| +6 | `int16` | Height of the sprite frame |
| +8 | `int32` | Bitmap data offset |
| +12 | `int32` | Bitmap data size |
| +16 | `int32` | (Unused) |

**Sprite signature values**

| Value | Description |
|-------|-------------|
| 1 | Valid |

**Format values**

| Value | Description |
|-------|-------------|
| `0x0555` | 16-bit per pixel (RGB555) |
| `0x555A` | [Custom 16-bit per pixel](https://github.com/SirusDoma/CXO2/blob/4d5471a360009ed85332106f0ddddddc38203860/src/OTwo/IO/Loaders/Adaptor/O2JamSpriteLoader.cpp#L186-L226) |

#### Important Notes

- The frame header defines the position of the sprite relative to world space, as well as the frame dimensions.
- **Bitmap data is raw / headerless.** It does **not** include a typical bitmap header.
- The pixel format is always 16-bit per pixel RGB555, but one format variant encodes RGB color with [custom logic](https://github.com/SirusDoma/CXO2/blob/4d5471a360009ed85332106f0ddddddc38203860/src/OTwo/IO/Loaders/Adaptor/O2JamSpriteLoader.cpp#L186-L226).
- If the sprite frame width is Not Power of Two (NPOT), the bitmap data will be uneven on every other scanline. Refer to this [code sample](https://stackoverflow.com/questions/37140941/extracting-bitmap-data-to-an-array-of-byte) for scanline handling when decoding or encoding bitmap data.
- The position in the frame header is a world-space coordinate used only for rendering. For input hit-testing, see [O2Jam Boundary](#o2jam-boundary).

---

### O2Jam Boundary

A boundary file defines the input hit-test regions — the coordinates and dimensions of each clickable area — for objects in a scene. The file extension is `.bnd`.

Each bound does not contain a reference to the scene object it describes. The mapping between scene objects (defined in the [Control List](#o2jam-control-list)) and boundary data is typically index-based, but this is not always the case, particularly for control list entries or scene objects that are hardcoded in the client.

Typically, one boundary frame belongs to one sprite object. Even if a sprite has multiple frames with unique sizes or positions, the bound for the object remains unchanged.

**Boundary list header**

| Offset | Type | Description |
|--------|------|-------------|
| 0 | `int32` | Signature |
| 4 | `int16` | Total bounds |

**Boundary frame header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Left |
| +4 | `int32` | Top |
| +8 | `int32` | Right |
| +12 | `int32` | Bottom |

<sup>All positions in a boundary frame use absolute world-space coordinates.</sup>

---

### O2Jam Control List

The control list is a text-based metadata file that defines the composition of objects in every scene of the game. Files are typically named `ControlList_Interface.txt` or `ControlList_Playing.txt`, depending on the archive. The structure is similar to [VDF](https://developer.valvesoftware.com/wiki/VDF).

The ordering of objects in this table does not affect z-index render order, but it may affect mapping with the [boundary](#o2jam-boundary) table. The game sometimes refers to a sprite by its OJS filename directly rather than looking it up from this table. Many sprites may be missing from the list; in such cases, the boundary is either resolved using hardcoded indices from the boundary file or uses values hardcoded in the game.

#### File Structure

The control list uses a hierarchical structure of states (scenes), objects, and nested sets.

- Comments begin with double slashes (`//`) and continue to end of line.
- Parameters are separated by whitespace (spaces, tabs, or a combination).
- Parameters are either hexadecimal numbers or strings enclosed in double quotes (`"file_name.ext"`).
- Each object definition (name and parameters) must appear on a single line.
- Curly braces (`{}`) represent a container object that can contain other objects.

#### Top-Level Definitions

##### `NUMBER_OF_STATES`

Defines the number of `STATE_*` objects.

```
NUMBER_OF_STATES    0x01
```

Parameters:
- Number of states

##### `NUMBER_OF_DIALOGS`

Defines the number of `DIALOG_*` objects.

```
NUMBER_OF_DIALOGS    0x01
```

Parameters:
- Number of dialogs

##### `STATE`

Represents a state (also known as a scene). Each state must have a unique name and typically starts with the `STATE_` prefix.

```
STATE_LOGO    0x03    0x00
{
    // State objects
}
```

Parameters:
- Number of state objects
- Number of `SET` objects within state objects

##### `DIALOG`

Represents a modal dialog. Each dialog must have a unique name and typically starts with the `DIALOG_` prefix.

```
DIALOG_INFORMATION    0x03
{
    // Dialog objects
}
```

Parameters:
- Number of dialog objects

#### Object Types

##### `BOUND` Declaration

Each `STATE` must contain exactly one `BOUND` declaration, which points to a [boundary](#o2jam-boundary) file describing the input hit-test regions of the state's or dialog's objects.

```
BOUND    "file.bnd"
```

Parameter:
- Boundary filename

##### `IDC` Objects

`IDC` (short for `ID Control`) is a regular object that acts as a basic building block of the control list. These typically follow the naming convention `IDC_<Type>_<Name>`.

```
IDC_IMAGE_BACKGROUND    0x11020304    "background.ojs"
```

Parameters:
- **Object ID**: Hexadecimal format, typically composed of four parts (1 byte each):
  - Byte 1: State ID (typically state index + 11)
  - Byte 2: Object type
  - Byte 3: Group/layer ID
  - Byte 4: Object ID
- **Asset filename**: Typically refers to a [sprite file](#o2jam-sprites)

> **Note:** The four-part convention for the Object ID is informal. The game does not parse it into four parts; it stores the value as a single integer and uses it as an identifier to associate with the corresponding sprite object.

##### Object Type Codes

The second byte of the Object ID maps to one of the following object types:

| Code | Object Type |
|------|-------------|
| 00 | `STATE` |
| 01 | Toggle/Radio/Checkbox |
| 02 | Scroll Bar |
| 10 | Image/Sprite |
| 20 | Button |
| 30 | Input Text Box |
| 50 | Interactable/Clickable |
| 60 | `SET` |
| 70 | Avatar |

> **Note:** The assets often use `Interactable` interchangeably with `Button` or `Toggle`.

##### `SET` Objects

`SET` objects are containers that group multiple objects, typically used for repeated elements such as row items in a list or table.

```
SET 0x02    0x11600200
{
    // Nested objects
}
```

Parameters:
- Number of objects inside the SET
- Object ID (same format as regular objects)

#### Example

```
// Define composition with one state
NUMBER_OF_STATES    0x01

// Logo screen state with 3 objects
STATE_LOGO    0x03    0x00
{
    // Boundary definition
    BOUND    "logo.bnd"
    
    // Background sprite
    IDC_IMAGE_BACKGROUND    0x11100100    "background.ojs"
    
    // Button set containing 2 buttons
    SET 0x02    0x11600200
    {
        IDC_BUTTON_START    0x11200201    "button_start.ojs"
        IDC_BUTTON_EXIT     0x11200202    "button_exit.ojs"
    }
}
```

---

### O2Jam Item Data Table

Item data is a collection of avatar item metadata available in the game. The filename is `ItemData.dat` (or `ItemData_TICT.dat`, `ItemData_Korea.dat`, `ItemData_China.dat`, etc., depending on the distribution). A legacy filename `O2-x001.ojs` uses the same structure. It is unclear at which point the client used this legacy file.

There are two known layout variants: the **New version** includes two extra `int32` special item flags before the name field, while the **Old version** does not.

#### File Header

| Offset | Type | Description |
|--------|------|-------------|
| 0 | `int32` | Number of items |

#### Item Entry — Common Fields

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Item ID |
| +4 | `byte` | Item Type (see [Item Type](#item-type-planets-and-payment-method)) |
| +5 | `byte` | Planet Origin (see [Planet](#item-type-planets-and-payment-method)) |
| +6 | `int16` | Gender (bitflag, see [Gender](#gender)) |
| +8 | `int16` | Quantity (for Attributive Items) |
| +10 | `byte` | Attributive Effect (see [Attributive Effect](#attributive-skills)) |
| +11 | `byte` | Attributive Category (see [Attributive Category](#attributive-skills)) |
| +12 | `byte` | Payment Method (see [Payment Method](#item-type-planets-and-payment-method)) |
| +13 | `int32` | Price in GEM |
| +17 | `int32` | Price in ePoint / O2Cash / Gash / MCash |
| +21 | `byte` | Item Slot (see [Item Slot](#item-slot)) |

The remaining fields differ by version:

**Old version (e-Games)**

| Offset | Type | Description |
|--------|------|-------------|
| +22 | `int32` | Name Length (in bytes) |
| +26 | `char[]` | Item Name |
| +26+n | `int32` | Description Length (in bytes) |
| +30+n | `char[]` | Item Description |

<sup>String encoding is typically standard `ASCII`.</sup>

**New version (NOWCOM)**

| Offset | Type | Description |
|--------|------|-------------|
| +22 | `int32` | Special Item Flag (Male)\* |
| +26 | `int32` | Special Item Flag (Female)\* |
| +30 | `int32` | Name Length (in bytes) |
| +34 | `char[]` | Item Name |
| +34+n | `int32` | Description Length (in bytes) |
| +38+n | `char[]` | Item Description |

<sup>\* Value is `0` or `10` (`0x0A`). String encoding is typically `EUC-KR`.</sup>

After the name and description, each item entry contains exactly **42 file header references**, each referencing an OJS sprite file. The order determines which body part and instrument animation combination the sprite represents. See [Item File References](#item-file-references).

#### Item File Header Slot

Repeated 42 times per item:

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `bool` | Slot status (active / unused) |
| +1 | `int32` | Filename Length (in bytes) |
| +5 | `char[]` | OJS Filename (null-terminated string) |

#### Enumerations

##### Item Type, Planets and Payment Method

**Item Type**

| Value | Description |
|-------|-------------|
| 0 | Body |
| 1 | Left Arm |
| 2 | Right Arm |
| 3 | Left Hand |
| 4 | Right Hand |
| 5 | Face |
| 6 | Hair |
| 7 | Glasses |
| 8 | Earring |
| 9 | Necklace |
| 10 | Armlet |
| 11 | Accessories |
| 12 | Glove |
| 13 | Pants |
| 14 | Shoes |
| 15 | Musical Instrument – Piano |
| 16 | Musical Instrument – Bass |
| 17 | Musical Instrument – Drum |
| 18 | Musical Instrument – Guitar |
| 19 | Shirts |
| 20 | Wings |
| 21 | Musical Accessories |
| 22 | Pet |
| 23 | Hair Accessories |
| 24 | Attributive Item (Skill) |

**Planet**

| Value | Description |
|-------|-------------|
| 0 | All |
| 1 | O2Planet |
| 2 | Aqua |
| 3 | Eliten |
| 4 | Graffiti |
| 5 | Bikini |
| 6 | Crush |
| 7 | Wonderland |
| 8 | Meganut |
| 9 | Crystal |
| 10 | Draconic |
| 11 | Event |

**Payment Method**

| Value | Description |
|-------|-------------|
| 0 | Not for Sale |
| 1 | Gem |
| 2 | ePoint / O2Cash / Gash / MCash |
| 3 | Any (NOWCOM version only) |

##### Gender

Gender is stored as a bitflag in the `int16` value. The field encodes two pieces of information:

- **Gender**: Bits 7–10: `(value >> 7) & 0xF`
- **`New` label**: Bit 11: `(value >> 11) & 0x1`

**Gender values**

| Value | Description |
|-------|-------------|
| 0 | Female |
| 1 | Male |
| 2 | Unisex |

**New label values**

| Value | Description |
|-------|-------------|
| 0 | Not New |
| 1 | New |

**Raw value examples**

| Raw (Hex) | Raw (Dec) | Gender | New Label | Description |
|-----------|-----------|--------|-----------|-------------|
| `0x0000` | 0 | Female | Not New | Female Item |
| `0x0080` | 128 | Male | Not New | Male Item |
| `0x0100` | 256 | Unisex | Not New | Unisex Item |
| `0x0800` | 2048 | Female | New | New Female Item |
| `0x0880` | 2176 | Male | New | New Male Item |
| `0x0900` | 2304 | Unisex | New | New Unisex Item |

##### Attributive (Skills)

These properties describe Attributive Items (also known as Skills) and are only valid when the Item Type is `24`.

**Attributive Effect**

| Value | Description |
|-------|-------------|
| 0 | None |
| 1 | Power |
| 2 | Mirror |
| 3 | Random |
| 4 | Panic |
| 5 | Hidden |
| 6 | Sudden |
| 7 | Dark |

**Attributive Category**

| Value | Description |
|-------|-------------|
| 0 | None |
| 1 | Power |
| 2 | Arrangement |
| 3 | Visibility |

##### Item Slot

| Value | Description | Item Type Code |
|-------|-------------|----------------|
| 0 | Musical Instrument | 15, 16, 17, 18 |
| 1 | Hair | 6 |
| 2 | Accessories | 11 |
| 3 | Glove | 12 |
| 4 | Necklace | 9 |
| 5 | Shirts | 19 |
| 6 | Pants | 13 |
| 7 | Glasses | 7 |
| 8 | Earring | 8 |
| 9 | Armlet | 10 |
| 10 | Shoes | 14 |
| 11 | Face | 5 |
| 12 | Wing | 20 |
| 13 | Musical Accessories | 21 |
| 14 | Pet | 22 |
| 15 | Hair Accessories | 23 |
| 255 | Body / Arms / Hands | 0–4 |
| 255 | Attributive Item | 24 |

##### Item File References

There are 42 file references per item. The order determines which body part and instrument animation the sprite will be used for:

| Index | Description |
|-------|-------------|
| 0 | Preview / Thumbnail (Small) |
| 1 | Preview / Thumbnail (Big) |
| 2 | Body – No Instrument (Male) |
| 3 | Body – No Instrument (Female) |
| 4 | Body – Bass (Male) |
| 5 | Body – Bass (Female) |
| 6 | Body – Guitar (Male) |
| 7 | Body – Guitar (Female) |
| 8 | Body – Keyboard (Male) |
| 9 | Body – Keyboard (Female) |
| 10 | Body – Drum (Male) |
| 11 | Body – Drum (Female) |
| 12 | Right Arm – No Instrument (Male) |
| 13 | Left Arm – No Instrument (Female) |
| 14 | Left Arm – Bass (Male) |
| 15 | Left Arm – Bass (Female) |
| 16 | Left Arm – Guitar (Male) |
| 17 | Left Arm – Guitar (Female) |
| 18 | Left Arm – Keyboard (Male) |
| 19 | Left Arm – Keyboard (Female) |
| 20 | Left Arm – Drum (Male) |
| 21 | Left Arm – Drum (Female) |
| 22 | Right Arm – No Instrument (Male) |
| 23 | Right Arm – No Instrument (Female) |
| 24 | Right Arm – Bass (Male) |
| 25 | Right Arm – Bass (Female) |
| 26 | Right Arm – Guitar (Male) |
| 27 | Right Arm – Guitar (Female) |
| 28 | Right Arm – Keyboard (Male) |
| 29 | Right Arm – Keyboard (Female) |
| 30 | Right Arm – Drum (Male) |
| 31 | Right Arm – Drum (Female) |
| 32 | Cape – No Instrument (Male) |
| 33 | Cape – No Instrument (Female) |
| 34 | Cape – Bass (Male) |
| 35 | Cape – Bass (Female) |
| 36 | Cape – Guitar (Male) |
| 37 | Cape – Guitar (Female) |
| 38 | Cape – Keyboard (Male) |
| 39 | Cape – Keyboard (Female) |
| 40 | Cape – Drum (Male) |
| 41 | Cape – Drum (Female) |

---

### O2Jam Set Info Table

The set info data table defines item set packages available in the item shop. The filename is typically `SetInfoData.ojs`. Item sets bundle up to 5 items at a time, typically at a discounted price.

#### File Header

| Offset | Type | Description |
|--------|------|-------------|
| 0 | `int32` | Number of sets |

#### Set Entry Header

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Set ID |
| +4 | `byte` | First Bitflag: New label and Planet Origin (see [Set Bitflag](#set-bitflag) & [Planet](#item-type-planets-and-payment-method)) |
| +5 | `byte` | Second Bitflag: Gender (see [Set Bitflag](#set-bitflag)) |
| +6 | `bool` | Use discounted price<sup>\*</sup> |
| +7 | `int16` | Number of items included in the set |
| +9 | `int32` | Item ID 1 |
| +13 | `int32` | Item ID 2 |
| +17 | `int32` | Item ID 3 |
| +21 | `int32` | Item ID 4 |
| +25 | `int32` | Item ID 5 |
| +29 | `int32` | Item Price 1 |
| +33 | `int32` | Item Price 2 |
| +37 | `int32` | Item Price 3 |
| +41 | `int32` | Item Price 4 |
| +45 | `int32` | Item Price 5 |
| +49 | `int32` | Item Sale Price 1 |
| +53 | `int32` | Item Sale Price 2 |
| +57 | `int32` | Item Sale Price 3 |
| +61 | `int32` | Item Sale Price 4 |
| +65 | `int32` | Item Sale Price 5 |
| +69 | `int32` | Name Length (in bytes) |
| +73 | `char[]` | Set Name |
| +73+n | `int32` | Description Length (in bytes) |
| +77+n | `char[]` | Item Description |

<sup>Item ID, price, and discount fields are ignored beyond the specified number of items (e.g., Item ID 4, Price 4, and Discount 4 are ignored if the set contains only 3 items).</sup>

<sup>\* The actual numeric value may fall outside the range 0–1. The value often correlates with `floor((total_price − final_price) / total_price * 100)` (discount rate) for many entries, but is not used numerically by the client — only the zero/non-zero check matters.</sup>

##### Set Bitflag

New label, Gender, and Planet are stored as bitflags. The fields are encoded as follows:

- **New label**: `first & 0x7F`
- **Planet**: `(first >> 7) & 1`
- **Gender**: `(second >> 6)`

---

### O2Jam Music List

O2Jam stores the entire music list that the game recognizes in a file called `OJNList.dat` (or `X2OJNList.dat` in the X2 version). Each entry contains music metadata, which is the entire header (the first 300 bytes) of an [OJN](#o2jam-note) file.

The music list defines what songs the game knows about, not necessarily what is playable. Actual playability depends on whether the corresponding [OJN](#o2jam-note) and [OJM](#o2jam-music) files are present locally.

In O2Jam v3.10, `OJNList.dat` is embedded within `Playing(1).opi`. A remnant still exists inside `Playing(1).opi` in newer client versions, but it is no longer used.

#### File Header

| Offset | Type | Description |
|--------|------|-------------|
| 0 | `int32` | Number of music entries |

#### Music Header

Each music header is equal to the first 300 bytes of an [OJN](#o2jam-note) file. Refer to that section for the field layout.

### Music List Extra Sections

In e-Games and NOWCOM distributions, `OJNList.dat` (and `X2OJNList.dat`) typically contains extra sections appended after the music header array. These sections may be interpreted differently across client versions (e.g., NX vs. X2).

Most fields in these sections are unknown, but their primary purposes are documented below.

#### Firt Extra Section: New Label

Defines music IDs with the "new" label.

**Section header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Number of music entries |

**Entry header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Music ID |
| +4 | `int32` | Unknown¹ |
| +8 | `int32` | Unknown² |
| +12 | `int32` | Unknown³ |

<sup>The client never read the (Unused) values. This list is merely used to mark songs with new label.</sup><br/>
<sup>1. Always `0`</sup><br>
<sup>2. Sometimes `1`, but otherwise `0`</sup><br>
<sup>3. Sometimes `0`, but otherwise `1`</sup>

#### Second Extra Section: Premium (Standard Client)

Defines premium music IDs that must be purchased in the music shop before becoming playable.

**Section header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Number of music entries |

**Entry header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Music ID |
| +4 | `int32` | Price in ePoint / O2Cash / Gash / MCash |
| +8 | `int32` | Price in GEM |
| +12 | `int32` | (Unused)<sup>\*</sup> |

<sup>* Likely price in second premium currency (O2Cash, MusicCash)</sup><br>

#### Second Extra Section (Alt): Mission (X2)

Defines a list of mission metadata that maps to actual music IDs.

**Section header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Number of missions |

**Entry header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Music ID |
| +4 | `int32` | Music Difficulty |
| +8 | `int32` | (Unused) |
| +12 | `int32` | Mission Level |

**Music Difficulty values**

| Value | Code | Description |
|-------|------|-------------|
| 1 | `EX` | Easy |
| 2 | `NX` | Normal |
| 3 | `HX` | Hard |

#### Third Extra Section: Extra Metadata

A section that contains extra metadata, such as release date information. Note that O2Jam NX (up to v3.82) does not read this section at all.

**Section header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Number of music entries |

**Entry header**

| Offset | Type | Description |
|--------|------|-------------|
| +0 | `int32` | Music ID |
| +4 | `char[10]` | Release Date (Format: `yyyy-MM-dd`) |
| +14 | `int16` | Unknown |
| +16 | `int32` | Unknown |
| +20 | `int32` | Unknown |
| +24 | `int32` | Unknown |

<sup><strong>The game does not read this section</strong>.</sup><br/>
<sup>Unknown fields are not verified. It might be possible that the bandwith size is not correct (it is not int16 or int32). </sup>

---

## Music

The `Music` directory holds all music assets used by the game client, consisting of two file formats: `.ojn` and `.ojm`.

All playable music filenames (except the tutorial) begin with `o2ma` followed by a music ID. Officially, music IDs start from `100`, but there is no enforcement preventing IDs with lower values. In addition to playable music, the game includes several OJM files that serve as background music or UI sound effects.

### O2Jam Note

The OJN file holds music metadata, cover and thumbnail art, and note information.

Refer to the Open2Jam documentation for the file format:
- [OJN Documentation](https://open2jam.wordpress.com/the-ojn-documentation/)
- [The Notes Section](https://open2jam.wordpress.com/2010/10/05/the-notes-section/)

#### Encrypted OJN

Starting from late O2Jam NX, the OJN file may be encrypted. The game client can process both encrypted and unencrypted OJN files.

**Encrypted OJN header**

| Offset | Type | Description |
|--------|------|-------------|
| 0 | `char[3]` | Signature (`new`) |
| 3 | `byte` | XOR key block size |
| 4 | `byte` | Primary XOR Key |
| 5 | `byte` | Middle XOR Key |
| 6 | `byte` | Initial XOR Key |

Decryption requires two steps:

**Step 1: Construct the XOR Key**

Create a byte array with length equal to the XOR key block size. Fill the entire array with the Primary XOR Key, then replace two positions: set the first byte (index `0`) to the Initial XOR Key, and set the byte at index `floor(xor_key_block_size / 2)` to the Middle XOR Key.

**Step 2: Decrypt the data**

Read the entire payload (everything after the 7-byte header) in reverse byte order. Walk through the reversed data in chunks equal to the XOR key block size, XOR-ing each chunk against the key block. The result is the original, unencrypted OJN data.

Refer to this [code sample](https://github.com/SirusDoma/CXO2/blob/bd845a2239315ec91d6bfe5fe6d4e94142343f25/src/OTwo/IO/Loaders/Chart/O2JamChartLoader.cpp#L314-L397) for the decryption algorithm.

### O2Jam Music

OJM is a sound bank container that holds either `.wav` or `.ogg` files.

Refer to the Open2Jam documentation for the file format:
- [OJM Documentation](https://open2jam.wordpress.com/the-ojm-documentation/)
