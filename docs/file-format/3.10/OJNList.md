# O2Jam Music List — Client v3.10 (O2Jam Original)

Music list format for the O2Jam v3.10 client. See [File Format Documentation](../FileFormat.md) for shared conventions and for the [OJN](../FileFormat.md#o2jam-note) header layout referenced here.

## File Location

Unlike every later client, v3.10 does **not** ship an `OJNList.dat` in the `Image/` directory. 
The file is stored inside `Playing(1).opi` as a regular archive entry named `OJNList.dat`. See [Package Interface & Avatar](../FileFormat.md#o2jam-package-interface--avatar) for the container format.

> [!NOTE]
> The entry remains present in `Playing(1).opi` in later clients, but is no longer used once a standalone `Image/OJNList.dat` exists.

## Structure

| Order | Section                                   | Entry Size | Required |
|-------|-------------------------------------------|------------|----------|
| —     | [File Header](#file-header)               | 4          | Yes      |
| —     | [Music Header Array](#music-header-array) | 300        | Yes      |
| 1     | [New](#section-1--new)                    | 16         | No       |
| 2     | [Premium](#section-2--premium)            | 16         | No       |
| 3     | [Release Date](#section-3--release-date)  | 28         | No       |

Every extra section is optional, but the ordering is fixed. 
A section that is present but carries no data is written as a count of `0`.

The client reads section 1 and section 2, then stops. Section 3 is present in the file but is never read by v3.10.

## File Header

| Offset | Type    | Description             |
|--------|---------|-------------------------|
| 0      | `int32` | Number of music entries |

## Music Header Array

Immediately follows the file header, repeated once per music entry. 
Each entry is exactly **300 bytes** — the first 300 bytes of an [OJN](../FileFormat.md#o2jam-note) file. Refer to that section for the field layout.

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

| Offset | Type    | Description |
|--------|---------|-------------|
| +0     | `int32` | Music ID    |
| +4     | `int32` | New flag    |
| +8     | `int32` | Unused      |
| +12    | `int32` | Unused      |

A non-zero New flag applies the label. The flag is also used as a sort key, falling back to music ID when two entries share the same value.

<sup>The two trailing fields are stored but never read by the client.</sup>

---

### Section 2 — Premium

Defines music that must be purchased in the music shop before it becomes playable.

| Offset | Type    | Description |
|--------|---------|-------------|
| +0     | `int32` | Music ID    |
| +4     | `int32` | Price       |
| +8     | `int32` | Unused      |
| +12    | `int32` | Unused      |

Price is also the ownership gate: a music with a price of `0` is always available, while a non-zero price requires the music to be owned before it becomes playable. The price is displayed in the music shop and is also used as a sort key, falling back to music ID when two entries share the same value.

<sup>The two trailing fields are stored but never read by the client.</sup>

---

### Section 3 — Release Date

Carries the release date of each music entry. **v3.10 does not read this section**; it stops after section 2.

| Offset | Type       | Description                                          |
|--------|------------|------------------------------------------------------|
| +0     | `int32`    | Music ID                                             |
| +4     | `char[11]` | Release Date (Format: `yyyy-MM-dd`, null-terminated) |
| +15    | `byte[13]` | Unused                                               |

The date string is null-terminated within its 11-byte field.
