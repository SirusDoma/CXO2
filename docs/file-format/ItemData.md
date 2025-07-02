# O2Jam `ItemData.dat` File Format Documentation

This document describes the binary structure of the O2Jam `ItemData.dat` file, which stores information about in-game items.

## File Header

| Offset | Data Type | Description                           |
|--------|-----------|---------------------------------------|
| 0      | `int32`   | Total number of items in ItemData.dat |

## Item Entry Structure

Each item entry follows this structure:

| Offset | Data Type | Description                                                                               |
|--------|-----------|-------------------------------------------------------------------------------------------|
| 0      | `uint32`  | Item ID                                                                                   |
| 4      | `byte`    | Item Kind (see [Item Kind](#item-kind))                                                   |
| 5      | `byte`    | Planet Origin (see [Planet](#planet))                                                     |
| 6      | `uint16`  | Gender (Bitflag, see [Gender](#gender))                                                   |
| 8      | `uint16`  | Quantity (for Attributive Items, see [Attributive](#attributive-skills))                  |
| 10     | `byte`    | Modifier Effect (for Attributive Items, see [Attributive - Modifier](#modifier))          |
| 11     | `byte`    | Attribute Category (for Attributive Items, see [Attributive - Category](#skill-category)) |
| 12     | `byte`    | Price Kind (see [Price Kind](#price-kind))                                                |
| 13     | `uint32`  | Price in GEM                                                                              |
| 17     | `uint32`  | Price in MCash                                                                            |
| 21     | `byte`    | Render Part (see [Render Part](#render-part))                                             |

### Korean Version (New)
| Offset | Data Type | Description                                              |
|--------|-----------|----------------------------------------------------------|
| 22     | `uint32`  | Special Item Flag (Male) (Value: `0` or `10` (`0x0A`))   |
| 26     | `uint32`  | Special Item Flag (Female) (Value: `0` or `10` (`0x0A`)) |

### e-Games Version (Old)
| Offset         | Data Type | Length Field       | Description                                                         |
|----------------|-----------|--------------------|---------------------------------------------------------------------|
| 22             | `uint32`  | -                  | Name Length (in bytes)                                              |
| 26             | `char[]`  | Name Length        | Item Name (UTF-8 string, may be `EUC-KR` for Korean version)        |
| 26+Name Length | `uint32`  | -                  | Description Length (in bytes)                                       |
| 30+Name Length | `char[]`  | Description Length | Item Description (UTF-8 string, may be `EUC-KR` for Korean version) |

## Item Files Header

Each item can have up to 42 file slots with the following structure (repeated 42 times):

| Offset | Data Type | Length Field     | Description                                  |
|--------|-----------|------------------|----------------------------------------------|
| i      | `bool`    | -                | Slot status (active/unused)                  |
| i+1    | `int32`   | -                | File Name Length (in bytes)                  |
| i+5    | `char[]`  | File Name Length | OJS File Name (UTF-8 null-terminated string) |

## Enumerations

### Item Kind
| Value | Description                 |
|-------|-----------------------------|
| 0     | Body                        |
| 1     | Left Arm                    |
| 2     | Right Arm                   |
| 3     | Left Hand                   |
| 4     | Right Hand                  |
| 5     | Face                        |
| 6     | Hair                        |
| 7     | Glasses                     |
| 8     | Earring                     |
| 9     | Necklace                    |
| 10    | Armlet                      |
| 11    | Accessories                 |
| 12    | Grove                       |
| 13    | Pants                       |
| 14    | Shoes                       |
| 15    | Musical Instrument - Piano  |
| 16    | Musical Instrument - Bass   |
| 17    | Musical Instrument - Drum   |
| 18    | Musical Instrument - Guitar |
| 19    | Shirts                      |
| 20    | Wings                       |
| 21    | Musical Accessories         |
| 22    | Pet                         |
| 23    | Hair Accessories            |
| 24    | Attributive Item (Skill)    |

### Planet
| Value | Description |
|-------|-------------|
| 0     | All         |
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

### Gender
Gender is stored as a bitflag in the `uint16` value:

#### Extracting Gender Flag
To extract the gender from the `uint16` value, use the following bitshift operation:
```
gender = (value >> 7) & 15
```

| Gender Value | Description |
|--------------|-------------|
| `0`          | Female      |
| `1`          | Male        |
| `2`          | Unisex      |

#### Extracting New Item Flag
To extract the "New" flag from the `uint16` value, use the following bitshift operation:
```
isNew = (value >> 11) & 1
```

| New Flag Value | Description |
|----------------|-------------|
| `0`            | Not New     |
| `1`            | New         |

#### Raw Values Examples
| Raw Value (Hex) | Raw Value (Dec)  | Gender     | New Flag | Description          |
|-----------------|------------------|------------|----------|----------------------|
| `0x0000`        | `0`              | Female     | Not New  | Female Item          |
| `0x0080`        | `128`            | Male       | Not New  | Male Item            |
| `0x0100`        | `256`            | Unisex     | Not New  | Unisex Item          |
| `0x0800`        | `2048`           | Female     | New      | New Female Item      |
| `0x0880`        | `2176`           | Male       | New      | New Male Item        |
| `0x0900`        | `2304`           | Unisex     | New      | New Unisex Item      |

### Attributive (Skills)
Describe Attributive (also known as Skills) properties. These properties are only valid if the Item Kind equals to `24`.

#### Modifier
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

#### Skill Category
| Value | Description |
|-------|-------------|
| 0     | None        |
| 1     | Power       |
| 2     | Arrangement |
| 3     | Visibility  |

### Price Kind
| Value | Description             |
|-------|-------------------------|
| 0     | Not for Sale            |
| 1     | Gem                     |
| 2     | ePoint / O2Cash / MCash |

### Render Part
| Value | Description         | Item Kind Code |
|-------|---------------------|----------------|
| 0     | Musical Instrument  | 15, 16, 17, 18 |
| 1     | Hair                | 6              |
| 2     | Accessories         | 11             |
| 3     | Glove               | 12             |
| 4     | Necklace            | 9              |
| 5     | Shirts              | 19             |
| 6     | Pants               | 13             |
| 7     | Glasses             | 7              |
| 8     | Earring             | 8              |
| 9     | Armlet              | 10             |
| 10    | Shoes               | 14             |
| 11    | Face                | 5              |
| 12    | Wing                | 20             |
| 13    | Musical Accessories | 21             |
| 14    | Pet                 | 22             |
| 15    | Hair Accessories    | 23             |
| 255   | Body                | 0              |
| 255   | Left Arm            | 1              |
| 255   | Right Arm           | 2              |
| 255   | Left Hand           | 3              |
| 255   | Right Hand          | 4              |
| 255   | Attributive Item    | 24             |

## Item File Slots

There are 42 file slots and the order of file slots is important as it determines which part of the body will be rendered in game:

| Index | Description                        |
|-------|------------------------------------|
| 0     | Preview / Thumbnail (Small)        |
| 1     | Preview / Thumbnail (Big)          |
| 2     | Body - No Instrument (Male)        |
| 3     | Body - No Instrument (Female)      |
| 4     | Body - Bass (Male)                 |
| 5     | Body - Bass (Female)               |
| 6     | Body - Guitar (Male)               |
| 7     | Body - Guitar (Female)             |
| 8     | Body - Keyboard (Male)             |
| 9     | Body - Keyboard (Female)           |
| 10    | Body - Drum (Male)                 |
| 11    | Body - Drum (Female)               |
| 12    | Right Arm - No Instrument (Male)   |
| 13    | Left Arm - No Instrument (Female)  |
| 14    | Left Arm - Bass (Male)             |
| 15    | Left Arm - Bass (Female)           |
| 16    | Left Arm - Guitar (Male)           |
| 17    | Left Arm - Guitar (Female)         |
| 18    | Left Arm - Keyboard (Male)         |
| 19    | Left Arm - Keyboard (Female)       |
| 20    | Left Arm - Drum (Male)             |
| 21    | Left Arm - Drum (Female)           |
| 22    | Right Arm - No Instrument (Male)   |
| 23    | Right Arm - No Instrument (Female) |
| 24    | Right Arm - Bass (Male)            |
| 25    | Right Arm - Bass (Female)          |
| 26    | Right Arm - Guitar (Male)          |
| 27    | Right Arm - Guitar (Female)        |
| 28    | Right Arm - Keyboard (Male)        |
| 29    | Right Arm - Keyboard (Female)      |
| 30    | Right Arm - Drum (Male)            |
| 31    | Right Arm - Drum (Female)          |
| 32    | Cape - No Instrument (Male)        |
| 33    | Cape - No Instrument (Female)      |
| 34    | Cape - Bass (Male)                 |
| 35    | Cape - Bass (Female)               |
| 36    | Cape - Guitar (Male)               |
| 37    | Cape - Guitar (Female)             |
| 38    | Cape - Keyboard (Male)             |
| 39    | Cape - Keyboard (Female)           |
| 40    | Cape - Drum (Male)                 |
| 41    | Cape - Drum (Female)               |