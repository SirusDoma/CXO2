# O2Jam Music List — Client v2.33 (O2Jam X2)

Music list format for the O2Jam X2 client. See [File Format Documentation](../FileFormat.md) for shared conventions and for the [OJN](../FileFormat.md#o2jam-note) header layout referenced here.

## File Location

`Image/X2OJNList.dat`. X2 is the only client that uses the `X2` filename prefix; every other version ships `OJNList.dat`.

## Structure

| Order | Section                                   | Entry Size | Required |
|-------|-------------------------------------------|------------|----------|
| —     | [File Header](#file-header)               | 4          | Yes      |
| —     | [Music Header Array](#music-header-array) | 300        | Yes      |
| 1     | [New](#section-1--new)                    | 16         | No       |
| 2     | [Mission](#section-2--mission)            | 16         | No       |
| 3     | [Release Date](#section-3--release-date)  | 28         | No       |

Every extra section is optional, but the ordering is fixed. The client reads sections sequentially and stops at end of file, so a file may terminate after any section. A section that is present but carries no data is written as a count of `0`.

The section layout is byte-compatible with [v3.82](../3.82/OJNList.md); only the meaning of section 2 differs — X2 uses it for mission definitions rather than music shop pricing.

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
| +8     | `int32` | Unknown<sup>1</sup> |
| +12    | `int32` | Unknown<sup>2</sup> |

Presence of a music ID in this section is what applies the label; the remaining fields are not required to be non-zero.

<sup>1. Always `0`.</sup><br/>
<sup>2. Always `1`.</sup>

---

### Section 2 — Mission

Defines the mission list. Each entry binds a mission level to a music entry and the difficulty it must be played on. This section is exclusive to X2.

| Offset | Type    | Description                                                |
|--------|---------|------------------------------------------------------------|
| +0     | `int32` | Music ID                                                   |
| +4     | `int32` | Difficulty (see [Mission Difficulty](#mission-difficulty)) |
| +8     | `int32` | Unknown<sup>\*</sup>                                       |
| +12    | `int32` | Mission Level                                              |

The mission level is independent of the music's own difficulty level stored in the [OJN](../FileFormat.md#o2jam-note) header.

<sup>\* Always `0`.</sup>

#### Mission Difficulty

| Value | Code | Description |
|-------|------|-------------|
| 1     | `EX` | Easy        |
| 2     | `NX` | Normal      |
| 3     | `HX` | Hard        |

<sup>Difficulty is 1-based in this section, unlike the 0-based difficulty index used elsewhere in the client.</sup>

---

### Section 3 — Release Date

Carries the release date of each music entry.

| Offset | Type       | Description                                          |
|--------|------------|------------------------------------------------------|
| +0     | `int32`    | Music ID                                             |
| +4     | `char[11]` | Release Date (Format: `yyyy-MM-dd`, null-terminated) |
| +15    | `byte`     | Unknown<sup>1</sup>                                  |
| +16    | `int32`    | Unknown<sup>2</sup>                                  |
| +20    | `int32`    | Unknown<sup>3</sup>                                  |
| +24    | `int32`    | Unknown<sup>4</sup>                                  |

<sup>1. Always `0`.</sup><br/>
<sup>2. Observed value is always `2753084` (`0x2A023C`).</sup><br/>
<sup>3. Observed value is always `274` (`0x112`).</sup><br/>
<sup>4. Observed value is always `61589` (`0xF095`).</sup>
