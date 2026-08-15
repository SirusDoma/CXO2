# O2Jam Music List — Client v5.89 (O2JamO2 Beta)

Music list format for the O2JamO2 **Beta** client — the build that has the Easy and SuperEasy planets but no 3K mode. See [File Format Documentation](../FileFormat.md) for shared conventions and for the [OJN](../FileFormat.md#o2jam-note) header layout referenced here.

## Structure

| Order | Section                                   | Entry Size | Required |
|-------|-------------------------------------------|------------|----------|
| —     | [File Header](#file-header)               | 4          | Yes      |
| —     | [Music Header Array](#music-header-array) | 300        | Yes      |
| 1     | [New](#section-1--new)                    | 16         | No       |
| 2     | [Planet](#section-2--planet)              | 20         | No       |
| 3     | [Premium](#section-3--premium)            | 16         | No       |
| 4     | [SuperEasy](#section-4--supereasy)        | 12         | No       |
| 5     | [Music Label](#section-5--music-label)    | 8          | No       |
| 6     | [Discount](#section-6--discount)          | 8          | No       |
| 7     | [Release Date](#section-7--release-date)  | 28         | No       |

Every extra section is optional, but the ordering is fixed. The client reads sections sequentially and stops at end of file, so a file may terminate after any section. A section that is present but carries no data is written as a count of `0`.

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

Defines which music carries the "new" label, and whether that music is restricted to a subscription tier.

| Offset | Type    | Description      |
|--------|---------|------------------|
| +0     | `int32` | Music ID         |
| +4     | `int32` | Premium New flag |
| +8     | `int32` | New Type         |
| +12    | `int32` | New Override     |

The client resolves the label as:

- **New** when `New Type < 2`, or when `New Override` is non-zero.
- **Premium New** when the Premium New flag is non-zero. Subscription type `0x04` unlocks every music in this section; types `0x01` and `0x40` unlock them as well, along with all other content.

<sup>New Type is observed as `1` or `2`, which suggests an enum or state value rather than a boolean. No usage beyond the `< 2` comparison has been observed.</sup>

---

### Section 2 — Planet

Controls music availability per planet tier.

| Offset | Type    | Description                               |
|--------|---------|-------------------------------------------|
| +0     | `int32` | Music ID                                  |
| +4     | `int32` | Available on standard planets<sup>1</sup> |
| +8     | `int32` | Available on SuperEasy planet<sup>2</sup> |
| +12    | `int32` | Available on Easy planet<sup>3</sup>      |
| +16    | `int32` | Unknown<sup>4</sup>                       |

<sup>1. Any planet that is not SuperEasy, Easy, or Beginner/Practice.</sup><br/>
<sup>2. The SuperEasy planet targets players aged 10 and under.</sup><br/>
<sup>3. The Easy planet targets adult beginners.</sup><br/>
<sup>4. Unused and always `0`. Presumably a flag for the Beginner/Practice planet.</sup>

---

### Section 3 — Premium

Defines pricing for music. Removing a music ID from this section removes its premium status.

| Offset | Type    | Description         |
|--------|---------|---------------------|
| +0     | `int32` | Music ID            |
| +4     | `int32` | Price in O2Cash     |
| +8     | `int32` | Price in GEM        |
| +12    | `int32` | Unused<sup>\*</sup> |

A price of `0` in O2Cash renders a "Free" label. The GEM price is stored but ignored by the UI, which always displays the O2Cash price.

<sup>\* Observed value is the constant `4379964` (`0x42D53C`).</sup>

---

### Section 4 — SuperEasy

Overrides premium behaviour for the SuperEasy planet.

| Offset | Type    | Description          |
|--------|---------|----------------------|
| +0     | `int32` | Music ID             |
| +4     | `int32` | Unknown<sup>\*</sup> |
| +8     | `int32` | Availability         |

When Availability is non-zero, the music is free and playable on SuperEasy regardless of any premium restriction defined in the [Premium](#section-3--premium) section.

<sup>\* Observed value is constant `0`.</sup>

---

### Section 5 — Music Label

Assigns a colored label to each music entry. Labels are used as filter tabs both in the music selection screen and the music shop.

| Offset | Type    | Description                                              |
|--------|---------|----------------------------------------------------------|
| +0     | `int32` | Music ID                                                 |
| +4     | `int32` | Label ID (see [Music Label Values](#music-label-values)) |

#### Music Label Values

| Value | Label  | Korean | Description                               |
|-------|--------|--------|-------------------------------------------|
| 0     | —      | —      | Unlabeled                                 |
| 1     | Gold   | 골드라벨   | Best curated O2Jam music                  |
| 2     | Black  | 블랙라벨   | Music for O2Jam enthusiasts               |
| 3     | Blue   | 블루라벨   | Best collection of popular music (대중가요)   |
| 4     | Red    | 레드라벨   | O2Jam self-produced K-pop music (가요풍 자작곡) |
| 5     | Silver | 실버라벨   | Various other genres                      |

---

### Section 6 — Discount

Defines the discount applied to premium music.

| Offset | Type    | Description         |
|--------|---------|---------------------|
| +0     | `int32` | Music ID            |
| +4     | `int32` | Discount percentage |

The value is a whole percentage; `50` means 50% off.

---

### Section 7 — Release Date

Carries the release date of each music entry.

| Offset | Type       | Description                          |
|--------|------------|--------------------------------------|
| +0     | `int32`    | Music ID                             |
| +4     | `char[10]` | Release Date (Format: `yyyy-MM-dd`)  |
| +14    | `int16`    | Unknown<sup>1</sup>                  |
| +16    | `int32`    | Unknown<sup>2</sup>                  |
| +20    | `int32`    | Unknown<sup>3</sup>                  |
| +24    | `int32`    | Unknown<sup>1</sup>                  |

The date string is parsed into year/month/day by splitting on `-`. The resulting fields sort the music list by year → month → day → music ID.

<sup>1. Observed value is constant `0`.</sup><br/>
<sup>2. Observed value is constant `1243708` (`0x12FA3C`).</sup><br/>
<sup>3. Observed value is constant `4655611` (`0x4709FB`).</sup>
