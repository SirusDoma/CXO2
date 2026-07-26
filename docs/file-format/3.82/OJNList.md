# O2Jam Music List — Client v3.82 (O2Jam NX)

Music list format for the O2Jam NX client. See [File Format Documentation](../FileFormat.md) for shared conventions and for the [OJN](../FileFormat.md#o2jam-note) header layout referenced here.

This layout is not exclusive to v3.82 — v3.15, v3.17, v3.24 and v3.73 use the same structure, as does the legacy list embedded in [v3.10](../3.10/OJNList.md).

## Structure

| Order | Section                                   | Entry Size | Required |
|-------|-------------------------------------------|------------|----------|
| —     | [File Header](#file-header)               | 4          | Yes      |
| —     | [Music Header Array](#music-header-array) | 300        | Yes      |
| 1     | [New](#section-1--new)                    | 16         | No       |
| 2     | [Premium](#section-2--premium)            | 16         | No       |
| 3     | [Release Date](#section-3--release-date)  | 28         | No       |

Every extra section is optional, but the ordering is fixed. The client reads sections sequentially and stops at end of file, so a file may terminate after any section. A section that is present but carries no data is written as a count of `0`.

Several distributions ship a list with no extra sections at all — only the file header and the music header array.

## File Header

| Offset | Type    | Description             |
|--------|---------|-------------------------|
| 0      | `int32` | Number of music entries |

## Music Header Array

Immediately follows the file header, repeated once per music entry. Each entry is exactly **300 bytes** — the first 300 bytes of an [OJN](../FileFormat.md#o2jam-note) file. Refer to that section for the field layout.

## Extra Sections

Each section begins with its own entry count.

**Section header**

| Offset | Type    | Description                       |
|--------|---------|-----------------------------------|
| +0     | `int32` | Number of entries in this section |

The count is independent of the music entry count and may reference music IDs in any order.

---

### Section 1 — New

Marks music with the "new" label.

| Offset | Type    | Description         |
|--------|---------|---------------------|
| +0     | `int32` | Music ID            |
| +4     | `int32` | Unknown<sup>1</sup> |
| +8     | `int32` | Unknown<sup>2</sup> |
| +12    | `int32` | Unknown<sup>3</sup> |

Presence of a music ID in this section is what applies the label, even when all three fields are `0`.

<sup>1. Always `0`.</sup><br/>
<sup>2. Sometimes `1`, otherwise `0`.</sup><br/>
<sup>3. Sometimes `0`, otherwise `1`.</sup>

---

### Section 2 — Premium

Defines music that must be purchased in the music shop before it becomes playable.

| Offset | Type    | Description                             |
|--------|---------|-----------------------------------------|
| +0     | `int32` | Music ID                                |
| +4     | `int32` | Price in ePoint                         |
| +8     | `int32` | Unknown<sup>1</sup>                     |
| +12    | `int32` | Unknown<sup>2</sup>                     |

<sup>1. No occurrence other than `0`. Presumably the price in a secondary premium currency (O2Cash, MusicCash).</sup><br/>
<sup>2. No occurrence other than `0`. Presumably the price in in-game currency (GEM).</sup>

---

### Section 3 — Release Date

Carries the release date of each music entry.

| Offset | Type       | Description                                          |
|--------|------------|------------------------------------------------------|
| +0     | `int32`    | Music ID                                             |
| +4     | `char[11]` | Release Date (Format: `yyyy-MM-dd`, null-terminated) |
| +15    | `byte`     | Unknown<sup>1</sup>                                  |
| +16    | `int32`    | Unknown<sup>1</sup>                                  |
| +20    | `int32`    | Unknown<sup>2</sup>                                  |
| +24    | `int32`    | Unknown<sup>3</sup>                                  |

<sup>1. Always `0`.</sup><br/>
<sup>2. Observed value is always `1243692` (`0x12FA2C`).</sup><br/>
<sup>3. Observed value is always `4540192` (`0x454720`).</sup>
