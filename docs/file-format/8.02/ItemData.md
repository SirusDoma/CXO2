# O2Jam Item Data — Client v8.02 (O2Jam Classic)

Avatar item metadata for the O2Jam Classic client. See [File Format Documentation](../FileFormat.md) for shared conventions.

> [!NOTE]
> This layout is identical to [v3.82](../3.82/ItemData.md) except that two **special item flags** are inserted between the Item Part byte and the name, shifting the name and everything after it by 8 bytes. 
> It also uses four additional [Item Type](#item-type) values and a fourth [Payment Method](#payment-method).

## Structure

| Offset | Type    | Description                          |
|--------|---------|--------------------------------------|
| 0      | `int32` | Number of items                      |
| 4      | —       | Item entries, repeated once per item |

Item entries are variable-length. They must be read sequentially; there is no offset table.

## Item Entry

| Offset | Type     | Description                                             |
|--------|----------|---------------------------------------------------------|
| +0     | `int32`  | Item ID                                                 |
| +4     | `byte`   | Item Type (see [Item Type](#item-type))                 |
| +5     | `byte`   | Planet Origin (see [Planet](#planet))                   |
| +6     | `int16`  | Bitflag: Gender and New label (see [Bitflag](#bitflag)) |
| +8     | `int16`  | Quantity<sup>1</sup>                                    |
| +10    | `byte`   | Attributive Effect (see [Attributive](#attributive))    |
| +11    | `byte`   | Attributive Category (see [Attributive](#attributive))  |
| +12    | `byte`   | Payment Method (see [Payment Method](#payment-method))  |
| +13    | `int32`  | Price in GEM                                            |
| +17    | `int32`  | Price in O2Cash                                         |
| +21    | `byte`   | Item Part (see [Item Part](#item-part))                 |
| +22    | `int32`  | Special Item Flag (Male)<sup>2</sup>                    |
| +26    | `int32`  | Special Item Flag (Female)<sup>2</sup>                  |
| +30    | `int32`  | Name Length (in bytes)                                  |
| +34    | `char[]` | Item Name                                               |
| +34+n  | `int32`  | Description Length (in bytes)                           |
| +38+n  | `char[]` | Item Description                                        |

The name and description are followed by exactly **42** [item sprites](#item-sprites).

<sup>1. Quantity applies to Attributive Items.</sup><br/>
<sup>2. Each flag is either `0` or `10` (`0x0A`). A non-zero value marks the item as a special animated item for that gender; when both are `10` the item is treated as unisex. These flags are absent in every earlier client.</sup>

<sup>Name and description are encoded in `EUC-KR`.</sup>

## Item Sprites

Each sprite reference is **variable-length**:

| Offset | Type     | Description                                              |
|--------|----------|----------------------------------------------------------|
| +0     | `byte`   | Status: `0` = unused, `1` = present                      |
| +1     | `int32`  | Filename Length (in bytes) — **only when status is `1`** |
| +5     | `char[]` | OJS/OJI Filename — **only when status is `1`**           |

> [!IMPORTANT]
> When the status is `0` (unused), the entry ends there and the next slot begins immediately. The length and filename fields are skipped, not zero-filled.
> A slot may carry status `1` (present) with a zero-length filename.

There are 42 sprite references per item. The order determines which body part and instrument animation the sprite will be used for:

| Index | Description                        |
|-------|------------------------------------|
| 0     | Preview / Thumbnail (Big)          |
| 1     | Preview / Thumbnail (Small)        |
| 2     | Body – No Instrument (Male)        |
| 3     | Body – No Instrument (Female)      |
| 4     | Body – Bass (Male)                 |
| 5     | Body – Bass (Female)               |
| 6     | Body – Guitar (Male)               |
| 7     | Body – Guitar (Female)             |
| 8     | Body – Keyboard (Male)             |
| 9     | Body – Keyboard (Female)           |
| 10    | Body – Drum (Male)                 |
| 11    | Body – Drum (Female)               |
| 12    | Left Arm – No Instrument (Male)    |
| 13    | Left Arm – No Instrument (Female)  |
| 14    | Left Arm – Bass (Male)             |
| 15    | Left Arm – Bass (Female)           |
| 16    | Left Arm – Guitar (Male)           |
| 17    | Left Arm – Guitar (Female)         |
| 18    | Left Arm – Keyboard (Male)         |
| 19    | Left Arm – Keyboard (Female)       |
| 20    | Left Arm – Drum (Male)             |
| 21    | Left Arm – Drum (Female)           |
| 22    | Right Arm – No Instrument (Male)   |
| 23    | Right Arm – No Instrument (Female) |
| 24    | Right Arm – Bass (Male)            |
| 25    | Right Arm – Bass (Female)          |
| 26    | Right Arm – Guitar (Male)          |
| 27    | Right Arm – Guitar (Female)        |
| 28    | Right Arm – Keyboard (Male)        |
| 29    | Right Arm – Keyboard (Female)      |
| 30    | Right Arm – Drum (Male)            |
| 31    | Right Arm – Drum (Female)          |
| 32    | Back – No Instrument (Male)        |
| 33    | Back – No Instrument (Female)      |
| 34    | Back – Bass (Male)                 |
| 35    | Back – Bass (Female)               |
| 36    | Back – Guitar (Male)               |
| 37    | Back – Guitar (Female)             |
| 38    | Back – Keyboard (Male)             |
| 39    | Back – Keyboard (Female)           |
| 40    | Back – Drum (Male)                 |
| 41    | Back – Drum (Female)               |

## Enumerations

### Item Type

| Value | Description         |
|-------|---------------------|
| 0     | Body                |
| 1     | Left Arm            |
| 2     | Right Arm           |
| 3     | Left Hand           |
| 4     | Right Hand          |
| 5     | Face                |
| 6     | Hair                |
| 7     | Glasses             |
| 8     | Earrings            |
| 9     | Necklace            |
| 10    | Armlet              |
| 11    | Accessories         |
| 12    | Glove               |
| 13    | Pants               |
| 14    | Shoes               |
| 15    | Instrument – Piano  |
| 16    | Instrument – Bass   |
| 17    | Instrument – Drum   |
| 18    | Instrument – Guitar |
| 19    | Jacket              |
| 20    | Wings               |
| 21    | Musical Accessories |
| 22    | Pet                 |
| 23    | Hair Accessories    |
| 24    | Attributive Item    |
| 25    | Name Changer        |
| 26    | Penalty Reset       |
| 27    | Bag Expansion       |
| 28    | Costume             |

### Planet

| Value | Description |
|-------|-------------|
| 0     | Any         |
| 1     | O2Planet    |
| 2     | Aqua        |
| 3     | Eliten      |
| 4     | Graffiti    |
| 5     | Bikini      |
| 6     | Crush       |
| 7     | Wonderland  |
| 8     | Meganut     |
| 9     | Crystal     |
| 10    | Draconic    |
| 11    | Event       |

### Bitflag

The `int16` at `+6` packs two fields:

- **Gender**: `(value >> 7) & 0xF`
- **New label**: `value >> 11`

**Gender values**

| Value | Description |
|-------|-------------|
| 0     | Female      |
| 1     | Male        |
| 2     | Unisex      |

### Attributive

Describe Attributive Items (skills) and are meaningful only when Item Type is `24`.

**Attributive Effect**

| Value | Description | Effect                                                                   |
|-------|-------------|--------------------------------------------------------------------------|
| 0     | None        | —                                                                        |
| 1     | Power       | Unlock access to all music                                               |
| 2     | Mirror      | Flips the chart horizontally                                             |
| 3     | Random      | Shuffles the note order across the whole chart                           |
| 4     | Panic       | Applies a random effect to every measure of the chart                    |
| 5     | Hidden      | Covers the bottom 20% of the play area                                   |
| 6     | Sudden      | Shows only the bottom 20% of the play area                               |
| 7     | Dark        | Shows only the middle 20% of the play area                               |
| 8     | EndlessJam  | Prevents the player from being pushed out of the room during Live Battle |
| 9     | ReverseDark | Covers the middle 20% of the play area                                   |
| 10    | BlackMist   | ReverseDark, with the covered band moving up and down                    |
| 11    | AngelsLight | Dark, with the visible band moving up and down                           |

**Attributive Category**

| Value | Description |
|-------|-------------|
| 0     | None        |
| 1     | Power       |
| 2     | Arrangement |
| 3     | Visibility  |

### Payment Method

| Value | Description  |
|-------|--------------|
| 0     | Not for Sale |
| 1     | GEM          |
| 2     | O2Cash       |
| 3     | Any          |

### Item Part

Determines which avatar slot the item occupies.

| Value | Description         |
|-------|---------------------|
| 0     | Instrument          |
| 1     | Hair                |
| 2     | Accessories         |
| 3     | Glove               |
| 4     | Necklace            |
| 5     | Jacket              |
| 6     | Pants               |
| 7     | Glasses             |
| 8     | Earrings            |
| 9     | Armlet              |
| 10    | Shoes               |
| 11    | Face                |
| 12    | Wings               |
| 13    | Musical Accessories |
| 14    | Pet                 |
| 15    | Hair Accessories    |
| 16    | Body                |

A value of `255` means the part is derived from the Item Type instead: Body, Left Arm, Right Arm, Left Hand, Right Hand and Attributive Item map to their matching part, and anything else falls back to Body.
