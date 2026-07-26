# O2Jam Music List — Client v5.89 (O2JamO2 Final)

Music list format for the O2JamO2 **Final** client — the non-beta build that has 3K mode. See [File Format Documentation](../FileFormat.md) for shared conventions and for the [OJN](../FileFormat.md#o2jam-note) header layout referenced here.

O2Solista v0.3, v0.5 and v0.6 use this layout as well.

> [!NOTE]
> The O2JamO2 **Beta** client reports the same version number but uses a different layout; 
> it has no Free Music or Key Mode section, a narrower discount entry, and a different field order in section 4. 
> 
> See [v5.89 Beta](../5.89-beta/OJNList.md).

## Structure

| Order | Section                                    | Entry Size | Required |
|-------|--------------------------------------------|------------|----------|
| —     | [File Header](#file-header)                | 4          | Yes      |
| —     | [Music Header Array](#music-header-array)  | 300        | Yes      |
| 1     | [New](#section-1--new)                     | 16         | No       |
| 2     | [Planet](#section-2--planet)               | 20         | No       |
| 3     | [Premium](#section-3--premium)             | 16         | No       |
| 4     | [VIP Exclusion](#section-4--vip-exclusion) | 12         | No       |
| 5     | [Music Label](#section-5--music-label)     | 8          | No       |
| 6     | [Discount](#section-6--discount)           | 12         | No       |
| 7     | [Free Music](#section-7--free-music)       | 8          | No       |
| 8     | [Key Mode](#section-8--key-mode)           | 8          | No       |
| 9     | [Release Date](#section-9--release-date)   | 28         | No       |

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
| +16    | `int32` | Fallback availability<sup>4</sup>         |

<sup>1. Any planet that is not SuperEasy, Easy, or Beginner/Practice.</sup><br/>
<sup>2. The SuperEasy planet targets players aged 10 and under.</sup><br/>
<sup>3. The Easy planet targets adult beginners.</sup><br/>
<sup>4. Overrides availability when the server is not a SuperEasy, Easy, or 3K planet.</sup>

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

<sup>\* Observed value is the constant `2010252407` (`0x77D20477`).</sup>

---

### Section 4 — VIP Exclusion

Overrides premium behavior. Music listed here is excluded from the VIP package.

| Offset | Type    | Description         |
|--------|---------|---------------------|
| +0     | `int32` | Music ID            |
| +4     | `int32` | Availability        |
| +8     | `int32` | Unused<sup>\*</sup> |

When Availability is non-zero, the music is free and playable on SuperEasy and is not part of the VIP package.

<sup>\* Observed value is `0` or `1`.</sup>

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

Defines the discount applied to premium music, per currency.

| Offset | Type    | Description                |
|--------|---------|----------------------------|
| +0     | `int32` | Music ID                   |
| +4     | `int32` | O2Cash discount percentage |
| +8     | `int32` | GEM discount percentage    |

Values are whole percentages — `50` means 50% off.

<sup>The [Beta](../5.89-beta/OJNList.md#section-6--discount) client uses an 8-byte entry with a single discount field.</sup>

---

### Section 7 — Free Music

Music listed here becomes free and playable without purchasing it or owning a VIP pass.

| Offset | Type    | Description |
|--------|---------|-------------|
| +0     | `int32` | Music ID    |
| +4     | `int16` | Free flag   |
| +6     | `int16` | Unused      |

When the Free flag is non-zero, the music is free and playable.

---

### Section 8 — Key Mode

Defines the key mode of each music entry. Only one key mode is active per entry.

| Offset | Type    | Description                                        |
|--------|---------|----------------------------------------------------|
| +0     | `int32` | Music ID                                           |
| +4     | `byte`  | Key Mode (see [Key Mode Values](#key-mode-values)) |
| +5     | `byte`  | Unused<sup>\*</sup>                                |
| +6     | `int16` | Unused<sup>\*</sup>                                |

<sup>\* Observed as always `244` (`0xF4`) and `67` (`0x43`) respectively.</sup>

#### Key Mode Values

| Value | Description |
|-------|-------------|
| 3     | 3K          |
| 5     | 5K          |
| 7     | 7K          |

---

### Section 9 — Release Date

Carries the release date of each music entry.

| Offset | Type       | Description                                          |
|--------|------------|------------------------------------------------------|
| +0     | `int32`    | Music ID                                             |
| +4     | `char[10]` | Release Date (Format: `yyyy-MM-dd`)                  |
| +14    | `int16`    | Unknown<sup>1</sup>                                  |
| +16    | `int32`    | Unknown<sup>2</sup>                                  |
| +20    | `int32`    | Unknown<sup>3</sup>                                  |
| +24    | `int32`    | Unknown<sup>1</sup>                                  |

The date string is parsed into year/month/day by splitting on `-`, then used to sort the music list by year → month → day → music ID.

<sup>1. Observed values are `30464` (`0x7700`) at `+14` and `48401` (`0xBD11`) at `+24`.</sup><br/>
<sup>2. Observed value is always `1242928` (`0x12F730`).</sup><br/>
<sup>3. Observed value is always `4474939` (`0x44483B`).</sup>
