# O2Jam Item Data — Client v3.10 (O2Jam Original)

Avatar item metadata for the O2Jam v3.10 client. See [File Format Documentation](../FileFormat.md) for shared conventions.

> [!NOTE]
> This layout is exclusive to v3.10. It is identical to [v3.82](../3.82/ItemData.md) except that **Quantity is a single byte rather than an `int16`**, which shifts every field after it by one.

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
| +8     | `byte`   | Quantity<sup>\*</sup>                                   |
| +9     | `byte`   | Attributive Effect (see [Attributive](#attributive))    |
| +10    | `byte`   | Attributive Category (see [Attributive](#attributive))  |
| +11    | `byte`   | Payment Method (see [Payment Method](#payment-method))  |
| +12    | `int32`  | Price in GEM                                            |
| +16    | `int32`  | Price in ePoint                                         |
| +20    | `byte`   | Item Part (see [Item Part](#item-part))                 |
| +21    | `int32`  | Name Length (in bytes)                                  |
| +25    | `char[]` | Item Name                                               |
| +25+n  | `int32`  | Description Length (in bytes)                           |
| +29+n  | `char[]` | Item Description                                        |

The name and description are followed by exactly **42** [item sprites](#item-sprites).

<sup>\* Quantity applies to Attributive Items.</sup><br/>
<sup>Name and description use the distribution's string encoding; standard `ASCII` for e-Games clients.</sup>

## Item Sprites

Each sprite reference is **variable-length**:

| Offset | Type     | Description                                              |
|--------|----------|----------------------------------------------------------|
| +0     | `byte`   | Status: `0` = unused, `1` = present                      |
| +1     | `int32`  | Filename Length (in bytes) — **only when status is `1`** |
| +5     | `char[]` | OJS/OJI Filename — **only when status is `1`**           |

> [!IMPORTANT]
> When the status is `0` (unused), the reference ends there and the next one begins immediately. The length and filename fields are skipped, not zero-filled.
> A reference may carry status `1` (present) with a zero-length filename.

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

| Value | Description |
|-------|-------------|
| 0     | None        |
| 1     | Power       |
| 2     | Mirror      |
| 3     | Random      |
| 4     | Panic       |
| 5     | Hidden      |
| 6     | Sudden      |
| 7     | Dark        |

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
| 2     | ePoint       |

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

A value of `255` means the part is derived from the Item Type instead: Body, Left Arm, Right Arm, Left Hand, Right Hand and Attributive Item map to their matching part, and anything else falls back to Body.
