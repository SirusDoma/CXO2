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

Unlike image patches, music patches are downloaded by the main client (`OTwo.exe`) rather than the game patcher. The `TEMP/` folder holds in-progress download files for both [OJN](#o2jam-note) and [OJM](#o2jam-music) files, typically via Music Shop. 
While inside `TEMP/`, download files carry a trailing underscore (e.g., `o2ma100.ojn_`). Once every file of the request completes, the files are moved out of `TEMP/` with the underscore stripped; if the destination file already exists in `Music/`, the temporary copy is discarded instead.

In addition to the [game music files](#music), the client persists download state via [INI file](https://en.wikipedia.org/wiki/INI_file) until the requested files are fully downloaded (or the download fails). The filename follows the pattern `o2ma<music_id>.ini`, or `o2ma<music_id>_<version>.ini` when a chart revision is requested. The same `_<version>` suffix applies to the OJN filename, while the OJM filename never carries it:

```ini
[FileInfo]
FileNum=2
FileName1=o2ma100.ojn
FileStatus1=0
FileName2=o2ma100.ojm
FileStatus2=0
```

`FileStatus` tracks the download state of each file: `0` while pending or in progress, `4` once completed. When every entry reaches `4`, the files are moved out of `TEMP/` and the INI file is deleted.

Once a music download completes, the client also reports the downloaded music ID to the server via packet `5012`, unless the download was initiated from the Music Shop screen, where completion is handled locally instead.

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

- `OJNList.dat` in `Playing(1).opi`: Used by the v3.10 client, as `OJNList.dat` is absent from the `Image/` directory in that version. See [Music List — v3.10](3.10/OJNList.md).
- `JamTable.dat` in `Playing(1).opi`: Purpose unknown; the client version that used it is unclear.
- `O2-x001.ojs` in `Avatar.opa`: Contains an item data table with similar structure as `ItemData.dat`.

#### Archive File Format

**Archive header**

| Offset | Type                      | Description       |
|--------|---------------------------|-------------------|
| 0      | `int32`                   | Archive Signature |
| 4      | `int32`                   | File count        |
| 8      | `int32`                   | (Unused)          |
| 12     | `int32`                   | (Unused)          |
| 16     | `bytes[N]`                | File contents     |
| 16 + N | `bytes[152 × File count]` | File headers      |

**File header**

| Offset | Type        | Description     |
|--------|-------------|-----------------|
| +0     | `int32`     | File Signature  |
| +4     | `char[128]` | Filename        |
| +132   | `int32`     | Data offset     |
| +136   | `int32`     | File size\*     |
| +140   | `int32`     | Reserved size\* |
| +144   | `int32`     | (Unused)        |
| +148   | `int32`     | (Unused)        |

<sup>\* Read exactly File Size 1 bytes. See [File Size Fields](#file-size-fields).</sup>

**Archive signature values**

| Value | Description |
|-------|-------------|
| 1     | OPA         |
| 2     | OPI         |

**File signature values**

| Value | Description |
|-------|-------------|
| 0     | Unused      |
| 1     | Valid       |

##### File Size Fields

**Read exactly `File Size` bytes starting at the data offset.** `Reserved size` describes the allocated size of the entry occupies inside the content blob, which could be equal or greater than the `File Size`.  

The archive stores every payload back to back in a single blob, with the header table written after it. Growing or moving one entry would shift every payload that follows and invalidate their data offsets, so [image patching](#image-patch) can instead write a replacement payload into the region the entry already occupies.
When the replacement is smaller than what was there before, `File Size` shrinks to the new payload length while `Reserved size` retains the original allocation which prevents the patcher to rebuild the entire archive.

> [!NOTE] 
> The patching explanation is inferred from the archive layout and from which entries diverge. It has not been confirmed against `O2JamPatchClient.exe`.
> The read rule itself is independent of the explanation: `File Size` is the payload length.

---

### O2Jam Sprites

O2Jam sprite files use one of these extensions: `.ojs`, `.oji`, `.ojt`, and `.oja`, with `.ojs` being the most common. A sprite file is a container holding multiple frame headers and raw bitmap data for each corresponding sprite frame. Bitmaps are stored as individual frames, not as a combined texture atlas.

All extensions share the same file format structure, but the extension may hint at how the game should use the sprite (e.g., custom blending mode, or rendering via Direct3D instead of DirectDraw). This is not always the case; the game can override this behavior.

#### Sprite File Format

**Sprite header**

| Offset | Type    | Description            |
|--------|---------|------------------------|
| 0      | `int16` | Signature              |
| 2      | `int16` | Format                 |
| 4      | `int16` | Total frames           |
| 6      | `int16` | Transparency Color Key |

**Frame header**

| Offset | Type    | Description                    |
|--------|---------|--------------------------------|
| +0     | `int16` | X position of the sprite frame |
| +2     | `int16` | Y position of the sprite frame |
| +4     | `int16` | Width of the sprite frame      |
| +6     | `int16` | Height of the sprite frame     |
| +8     | `int32` | Bitmap data offset             |
| +12    | `int32` | Bitmap data size               |
| +16    | `int32` | (Unused)                       |

**Sprite signature values**

| Value | Description |
|-------|-------------|
| 1     | Valid       |

**Format values**

| Value    | Description                                                                                                                                                            |
|----------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `0x0555` | 16-bit per pixel (RGB555)                                                                                                                                              |
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

| Offset | Type    | Description  |
|--------|---------|--------------|
| 0      | `int32` | Signature    |
| 4      | `int16` | Total bounds |

**Boundary frame header**

| Offset | Type    | Description |
|--------|---------|-------------|
| +0     | `int32` | Left        |
| +4     | `int32` | Top         |
| +8     | `int32` | Right       |
| +12    | `int32` | Bottom      |

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

> [!NOTE] 
> The four-part convention for the Object ID is informal. 
> The game does not parse it into four parts; it stores the value as a single integer and uses it as an identifier to associate with the corresponding sprite object.

##### Object Type Codes

The second byte of the Object ID maps to one of the following object types:

| Code | Object Type            |
|------|------------------------|
| 00   | `STATE`                |
| 01   | Toggle/Radio/Checkbox  |
| 02   | Scroll Bar             |
| 10   | Image/Sprite           |
| 20   | Button                 |
| 30   | Input Text Box         |
| 50   | Interactable/Clickable |
| 60   | `SET`                  |
| 70   | Avatar                 |

> [!NOTE]
> The assets often use `Interactable` interchangeably with `Button` or `Toggle`.

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

Item data is a collection of avatar item metadata available in the game. Each entry holds the item's identity, pricing, and the sprite references used to render it on the avatar.

The table is stored inside `Avatar.opa` as a regular archive entry, typically appear as the last item in the archive after [SetInfoData.ojs](#o2jam-set-info-table). 
The filename carries a distribution suffix (`Itemdata.dat`, `ItemData_TICT.dat`, `ItemData_Korea.dat`, `ItemData_China.dat`, `ItemData_Phil.dat`, etc.) and the capitalization varies (`ItemData` or `Itemdata`). 
The suffix identifies the distribution and has no effect on the format.

#### File Header

| Offset | Type    | Description     |
|--------|---------|-----------------|
| 0      | `int32` | Number of items |

#### Item Entries

Item entries follow the file header, repeated once per item. They are variable-length and must be read sequentially; there is no offset table.

**The entry layout varies by client version.** Refer to the per-version documentation:

| Client Version | Distribution   | Documentation                   |
|----------------|----------------|---------------------------------|
| v3.10          | O2Jam Original | [ItemData.md](3.10/ItemData.md) |
| v3.82          | O2Jam NX       | [ItemData.md](3.82/ItemData.md) |
| v2.33          | O2Jam X2       | [ItemData.md](3.82/ItemData.md) |
| v5.89          | O2JamO2 Beta   | [ItemData.md](3.82/ItemData.md) |
| v6.65          | O2JamO2        | [ItemData.md](3.82/ItemData.md) |
| v8.02          | O2Jam Classic  | [ItemData.md](8.02/ItemData.md) |

---

### O2Jam Set Info Table

The set info data table defines item set packages available in the item shop. The filename is typically `SetInfoData.ojs`. Item sets bundle up to 5 items at a time, typically at a discounted price.

#### File Header

| Offset | Type    | Description    |
|--------|---------|----------------|
| 0      | `int32` | Number of sets |

#### Set Entry Header

| Offset | Type     | Description                                                                                                      |
|--------|----------|------------------------------------------------------------------------------------------------------------------|
| +0     | `int32`  | Set ID                                                                                                           |
| +4     | `byte`   | First Bitflag: New label and Planet Origin (see [Set Bitflag](#set-bitflag) & [Planet](3.82/ItemData.md#planet)) |
| +5     | `byte`   | Second Bitflag: Gender (see [Set Bitflag](#set-bitflag))                                                         |
| +6     | `bool`   | Use discounted price<sup>\*</sup>                                                                                |
| +7     | `byte`   | Number of items included in the set                                                                              |
| +8     | `byte`   | Currency (see [Set Currency](#set-currency))                                                                     |
| +9     | `int32`  | Item ID 1                                                                                                        |
| +13    | `int32`  | Item ID 2                                                                                                        |
| +17    | `int32`  | Item ID 3                                                                                                        |
| +21    | `int32`  | Item ID 4                                                                                                        |
| +25    | `int32`  | Item ID 5                                                                                                        |
| +29    | `int32`  | Item Price 1                                                                                                     |
| +33    | `int32`  | Item Price 2                                                                                                     |
| +37    | `int32`  | Item Price 3                                                                                                     |
| +41    | `int32`  | Item Price 4                                                                                                     |
| +45    | `int32`  | Item Price 5                                                                                                     |
| +49    | `int32`  | Item Sale Price 1                                                                                                |
| +53    | `int32`  | Item Sale Price 2                                                                                                |
| +57    | `int32`  | Item Sale Price 3                                                                                                |
| +61    | `int32`  | Item Sale Price 4                                                                                                |
| +65    | `int32`  | Item Sale Price 5                                                                                                |
| +69    | `int32`  | Name Length (in bytes)                                                                                           |
| +73    | `char[]` | Set Name                                                                                                         |
| +73+n  | `int32`  | Description Length (in bytes)                                                                                    |
| +77+n  | `char[]` | Item Description                                                                                                 |

<sup>Item ID, price, and discount fields are ignored beyond the specified number of items (e.g., Item ID 4, Price 4, and Discount 4 are ignored if the set contains only 3 items).</sup>

<sup>\* Matches `round((total_price − total_sale_price) / total_price * 100)` for the large majority of priced sets; the remainder may fall outside the range 0–1, or may have two or three points off. 
A value of `0` means the set is not discounted.</sup>

##### Set Bitflag

New label, Gender, and Planet are stored as bitflags. The fields are encoded as follows:

- **Planet**: `first & 0x7F`
- **New label**: `(first >> 7) & 1`
- **Gender**: `(second >> 6)`

##### Set Currency

Selects which currency the set's prices are denominated in.

| Value | Description             |
|-------|-------------------------|
| 0     | ePoint / O2Cash / MCash |
| 1     | GEM                     |

---

### O2Jam Music List

O2Jam stores the entire music list that the game recognizes in a file called `OJNList.dat` (or `X2OJNList.dat` in the X2 version). Each entry contains music metadata, which is the entire header (the first 300 bytes) of an [OJN](#o2jam-note) file.

The music list defines what songs the game knows about, not necessarily what is playable. Actual playability depends on whether the corresponding [OJN](#o2jam-note) and [OJM](#o2jam-music) files are present locally.

In O2Jam v3.10, `OJNList.dat` is embedded within `Playing(1).opi`. A remnant still exists inside `Playing(1).opi` in newer client versions, but it is no longer used.

#### File Header

| Offset | Type    | Description             |
|--------|---------|-------------------------|
| 0      | `int32` | Number of music entries |

#### Music Header

Each music header is equal to the first 300 bytes of an [OJN](#o2jam-note) file. Refer to that section for the field layout.

#### Music List Extra Sections

In e-Games and NOWCOM distributions, `OJNList.dat` (and `X2OJNList.dat`) typically contains extra sections appended after the music header array. 
These sections may be interpreted differently across client versions (e.g., NX vs. X2). Most fields in these sections are unknown and/or unused, but their primary purposes are documented.

Refer to the per-version documentation:

| Client Version | Distribution   | Filename                   | Documentation                      |
|----------------|----------------|----------------------------|------------------------------------|
| v3.10          | O2Jam Original | `OJNList.dat`<sup>\*</sup> | [OJNList.md](3.10/OJNList.md)      |
| v3.82          | O2Jam NX       | `OJNList.dat`              | [OJNList.md](3.82/OJNList.md)      |
| v2.33          | O2Jam X2       | `X2OJNList.dat`            | [X2OJNList.md](2.33/X2OJNList.md)  |
| v5.89          | O2JamO2 Beta   | `OJNList.dat`              | [OJNList.md](5.89/OJNList.md) |
| v6.65          | O2JamO2        | `OJNList.dat`              | [OJNList.md](6.65/OJNList.md)      |
| v8.02          | O2Jam Classic  | `OJNList.dat`              | [OJNList.md](8.02/OJNList.md)      |

<sup>\* Placed inside `Playing(1).opi`</sup>

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

| Offset | Type      | Description        |
|--------|-----------|--------------------|
| 0      | `char[3]` | Signature (`new`)  |
| 3      | `byte`    | XOR key block size |
| 4      | `byte`    | Primary XOR Key    |
| 5      | `byte`    | Middle XOR Key     |
| 6      | `byte`    | Initial XOR Key    |

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
