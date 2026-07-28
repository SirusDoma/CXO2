# Metadata Format

A metadata file describes **one resource**. A _resource_ could be a sprite, a UI control, an animation, a shape, a scene composition, an avatar and its item table, a cursor, and many others.
Currently it is expressed in JSON, but the game might support other formats in the future.

All metadata in JSON format uses the same envelope schema. 
The `type` field decides which `require` keys and `attributes` the metadata understands. 

For how metadata files are found, overridden, and packaged, see [Asset Customization](Asset.md).

## Envelope Schema

<table>
<tr><th>Schema</th><th>Example</th></tr>
<tr valign="top">
<td>

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "Metadata",
  "type": "object",
  "required": ["type"],
  "properties": {
    "type": {
      "type": "string"
    },
    "name": {
      "type": "string"
    },
    "require": {
      "type": "object",
      "additionalProperties": true
    },
    "attributes": {
      "type": "object",
      "properties": {
        "objects": {
          "type": "object",
          "additionalProperties": { 
            "$ref": "#/$defs/reference"
          }
        }
      },
      "additionalProperties": true
    }
  },
  "$defs": {
    "reference": {
      "anyOf": [
        {
          "type": "string"
        },
        {
          "$ref": "#"
        }
      ]
    }
  }
}
```

</td>
<td>

```json
{
  "type": "Animation",
  "name": "IDC_ANIMATION_TOWER",
  "require": {
    "sprite": "Planet_Tower.ojs"
  },
  "attributes": {
    "isLoop": true,
    "duration": 1300
  }
}
```

</td>
</tr>
</table>

| Field        | Type   | Required | Description                                                                                         |
|--------------|--------|----------|-----------------------------------------------------------------------------------------------------|
| `type`       | string | Yes      | The type of resource this metadata represents. Metadata with an unknown or missing type is ignored. |
| `name`       | string | No       | The resource name. Defaults to the ID the resource is loaded under.                                 |
| `require`    | object | No       | The external resources this metadata depends on: textures, sprites, bounds, fonts, sounds, etc.     |
| `attributes` | object | No       | The properties of the resource itself, including the `objects` map that nests other metadata.       |

Every field is optional except `type` (and `name`, for the scene types). Metadata with no `attributes` produces a resource with default values.

### File vs. Inline References

A reference to other metadata takes one of two interchangeable forms:

| Form   | Example                                        | Meaning                                                                                      |
|--------|------------------------------------------------|----------------------------------------------------------------------------------------------|
| String | `"ControlList/State/Planet/Planet_Tower.json"` | The path to a metadata file, resolved through the [asset pipeline](Asset.md#asset-pipeline). |
| Object | `{ "type": "Animation", "require": { … } }`    | The metadata, declared inline.                                                               |

The same applies to `attributes.objects` entries, list templates, dialog sub-objects, and avatar item references.

### Composition Order

Metadata is parsed as an *ordered* JSON document. 
Sibling entries in `objects` are composed in the order they appear, so the last entry is added last, and `frames` entries are numbered the same way.

### Failure Behavior

Metadata loading is resilient by design, so a partly modded asset set still boots:

- A child whose `type` is unknown, whose file is missing, or whose metadata fails to parse is skipped; its siblings still load.
- Metadata whose `attributes` cannot be read produces a resource with default values rather than aborting.
- A `require` entry that cannot be resolved is treated as absent, and the next source is used instead (see [Source Precedence](#source-precedence)).

Scene composition renames every node it places to `<parent name>/<key>`, 
so `name` matters mainly for resources loaded on their own.

## Common Fields

### `type`

Specifies the resource to build. Types that derive from a scene node can be nested in an `objects` map; the rest are standalone resources loaded by name.

#### Scene Types

- `State`
  - `StatePlaying7K`

#### Graphics Types

- `Animation`
- `Shape`
- `Sprite`

#### Common UI Types

- `BitmapNumber`
- `Button`
- `Dialog`
- `Gauge`
- `Image`
- `InputField`
- `Label`
- `List`
- `RadioButton`
- `ScrollBar`
- `ToggleButton`
- `ToolTip`
- `UiContainer`

#### Custom UI Types

- `AvatarInfo`
- `ChannelBoard`
- `ChannelButton`
- `ChatPanel`
- `ChatWindow`
- `CreateRoomDialog`
- `Equalizer`
- `InstrumentSelector`
- `MapSelector`
- `Marquee`
- `OptionDialog`
- `PlayMenu`
- `RoomButton`
- `RoomList`
- `SelectMusicDialog`
- `SpeedButton`
- `UserList`

#### Avatar Types

- `Avatar`
- `Item`
- `ItemData`
- `SetInfoData`

#### Audio Types

- `Music`
- `Sound`

#### Standalone Types

- `Cursor`

Custom UI types are game-specific subclasses of the common UI types. 
They are read as the type they derive from and accept exactly the same fields; see [Custom UI](#custom-ui).

Non-scene nodes such as `Cursor`, `ItemData`, `SetInfoData` and the audio types may **not** appear in an `objects` map.

> [!NOTE]
> `ControlList` is the legacy text format inside the original master archives, not a metadata type.  
> The metadata replaces it, and the values it holds are reached through `refID` and `bndID`.

### `require`

Lists of other resources required to construct the current resource. The keys below are the shared ones, and each type's own section lists which of them it reads; 
keys that belong to a single type (`template`, `thumbnail`, `items`, `sets`) are documented with that type.

The `require` is often grouped into two. **Custom references** read ordinary, portable files, while **interop references** read the original O2Jam formats and tables. 
The two can be mixed in one metadata: a resource may take its texture from a PNG and its position from the legacy boundary table or vice-versa.

#### Custom References

| Key             | Description                                                                                           |
|-----------------|-------------------------------------------------------------------------------------------------------|
| `texture`       | An image file (`.bmp`, `.png`, `.tga`, `.jpg`, `.gif`, `.psd`, `.hdr`, `.pic`, `.pnm`).               |
| `font`          | A font file (`.ttf`, `.ttc`, `.otf`, `.otc`, `.pfa`, `.pfb`, `.cff`, `.pfr`, `.bdf`, `.pcf`, `.fnt`). |
| `fallbackFonts` | Array of font files used for glyphs the main font lacks (same formats as `font`).                     |
| `sound`         | A sound file decoded into memory (`.wav`, `.ogg`, `.flac`, `.mp3`), read by [Sound](#sound).          |
| `music`         | A music file streamed while it plays (`.wav`, `.ogg`, `.flac`, `.mp3`), read by [Music](#music).      |

#### Interop References

| Key                    | Description                                                                                                                                                  |
|------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `sprite`               | An [O2Jam sprite](../file-format/FileFormat.md#o2jam-sprites) file (`.ojs`, `.oji`, `.ojt`, `.oja`).                                                         |
| `refID`                | A control ID in the legacy [control list](../file-format/FileFormat.md#o2jam-control-list); the O2Jam sprite file name is read from that entry.              |
| `bndID`, `globalBndID` | A bound ID in the legacy [boundary](../file-format/FileFormat.md#o2jam-boundary) table, in world space. Converted to a position local to the parent's bound. |
| `localBndID`           | The same lookup, used as-is without conversion.                                                                                                              |

The `refID` and bound keys read the tables cached from `ControlList_*.txt` and its `.bnd` files, which means they only work in [interop mode](Asset.md#interop-mode).
The `sprite` key names the file directly and works in either mode.

A graphical resource may only holds one texture at a time. `texture`, `sprite` and `refID` fill the same slot, so only the first one that resolves is used.

> [!IMPORTANT]
> Internally, the client will pack O2Jam sprite side by side into a single texture. Using multiple texture is NOT supported.  
> 
> This is important to keep in mind if you are modifying the scene composition while still using original sprite files since this mean you have to manipulate texCoords to change displayed texture rather than switching the texture itself.
> 
> However, most of resources support dealing this through various metadata attributes (such as `id` and `infer`) so you don't have to deal with the layouting of texture packing that is done by the client.
 
> [!NOTE]
> When using custom references, try to fit as many objects as you can in one texture.  
> Typically, the ideal size for one texture atlas in a modern hardware is 2048x2048 or 4096x4096 pixels.  
> 
> This is depends heavily on the target hardware performance, and memory limits.

#### Modifiers

| Key      | Description                                                                                                              |
|----------|--------------------------------------------------------------------------------------------------------------------------|
| `format` | The format string applied to a reference whose value begins with `_`. See [Formatted References](#formatted-references). |

### `attributes`

The following attributes are shared by most types. Type-specific attributes are listed with their type.

| Attribute   | Type   | Default     | Applies to           | Description                                                            |
|-------------|--------|-------------|----------------------|------------------------------------------------------------------------|
| `transform` | object | —           | Graphic resources    | The position, scale, rotation and origin. See [Transform](#transform). |
| `color`     | color  | White       | Graphic resources    | The tint colour. `Shape` and `ToolTip` default to black instead.       |
| `texCoords` | rect   | Whole frame | Graphic resources    | The sub-rectangle of the texture or O2Jam sprite frame to draw.        |
| `blend`     | string | `Auto`      | Sprite-derived types | The blend mode. See [Blend Mode](#blend-mode).                         |
| `id`        | number | —           | Sprite-derived types | The frame index inside the O2Jam sprite.                               |
| `objects`   | object | —           | Node types           | The child metadata, keyed by child name. Each value is a reference.    |

#### Transform

```json
"transform": {
  "position": {
    "x": 120,
    "y": 187
  },
  "origin": {
    "x": 0,
    "y": 0
  },
  "scale": {
    "x": 1,
    "y": 1
  },
  "rotation": 0
}
```

| Key        | Type   | Default  | Description                                                                                           |
|------------|--------|----------|-------------------------------------------------------------------------------------------------------|
| `position` | vector | Unset    | The explicit position.<br/>The value is inferred from the bound or the O2Jam sprite frame when unset. |
| `origin`   | vector | `{0, 0}` | The local origin used for rotation, scaling and placement.                                            |
| `scale`    | vector | `{1, 1}` | The scale factors.                                                                                    |
| `rotation` | number | `0`      | The rotation in degrees.                                                                              |

#### Value Structures

| Shape  | JSON                                            | Notes                                  |
|--------|-------------------------------------------------|----------------------------------------|
| vector | `{ "x": 0, "y": 0 }`                            | Both components required.              |
| rect   | `{ "x": 0, "y": 0, "width": 60, "height": 24 }` | All four components required.          |
| color  | `{ "a": 255, "r": 255, "g": 255, "b": 255 }`    | All four channels required, `0`–`255`. |
| time   | `1300`                                          | Milliseconds, as a plain number.       |

### Source Precedence

A texture source is resolved in a fixed order, and the first one that resolves is used:

1. `require.texture`, an image file.
2. `require.sprite`, an O2Jam sprite file.
3. `require.refID`, an O2Jam sprite reference in the corresponding entry of the legacy control list.

When none of them is specified, the resource is built without a texture.

Positioning follows its own order:

1. `attributes.transform.position`.
2. `require.bndID` / `globalBndID` / `localBndID`, the position stored in the legacy boundary table.
3. The frame position stored in the O2Jam sprite.
4. The origin, `x: 0` and `y: 0`.

A leftover `texture` key disables the sprite path entirely. 
A bound still positions a control whose texture comes from an image file.

### Formatted References

> [!NOTE]
> This is primarily used in the gameplay scene (`StatePlaying7K`)

When a resolved `require` value begins with `_`, the underscore is stripped and the remainder is expanded through the `format` string. 
This is how the legacy control list expresses per-map and per-effect variants of one sprite:

```json
"require": {
  "refID": "0X1B500066",
  "format": "{Name}{FxMapID}.{FxExtension}"
}
```

| Placeholder     | Value                                                            |
|-----------------|------------------------------------------------------------------|
| `{Name}`        | The reference name without its extension.                        |
| `{Extension}`   | The reference's own extension.                                   |
| `{MapID}`       | The current playing map ID.                                      |
| `{FxMapID}`     | Resolves to Map ID when 3D effects are enabled, empty otherwise. |
| `{FxExtension}` | Resolves to `oja` when 3D effects are disabled, empty otherwise. |
| `{EffectID}`    | The current effect ID.                                           |
| `{MapEffectID}` | Shorthand for `{MapID}_{EffectID}`.                              |

The map and effect placeholders are only populated while the playing scene is being composed; elsewhere they expand to an empty string.

## Scene

### State

Represents a scene. `name` is required.

| Field                | Description                                                                                                                                          |
|----------------------|------------------------------------------------------------------------------------------------------------------------------------------------------|
| `name`               | The state identifier, e.g. `STATE_PLANET`. Required.                                                                                                 |
| `require`            | String entries are loaded and registered in the state **without** being attached to the scene tree. Use it for resources the state looks up by name. |
| `attributes.objects` | The children, composed in the order they appear and attached to the scene tree.                                                                      |

```json
{
  "type": "State",
  "name": "STATE_PLANET",
  "require": {},
  "attributes": {
    "objects": {
      "IDC_IMAGE_STATE_PLANET": "ControlList/State/Planet/State_Planet.json",
      "IDC_ANIMATION_TOWER": "ControlList/State/Planet/Planet_Tower.json"
    }
  }
}
```

#### StatePlaying7K

Represents a specialized type for the 7-key gameplay scene. 
It is a variant of [State](#state) and takes the same structure, plus a viewport and a set of `require`d gameplay resources.

| Field                 | Description                                                                                                         |
|-----------------------|---------------------------------------------------------------------------------------------------------------------|
| `require`             | The named gameplay resources, resolved per selected map and per selected effect. Required entries are listed below. |
| `attributes.viewport` | The Y coordinate the judgement line sits at. Note positions are measured upward from it. Required.                  |
| `attributes.objects`  | The children, as for [State](#state). Three of them are mandatory, and the scene fails to load when one is missing. |

| Require key                                            | Notes                                                        |
|--------------------------------------------------------|--------------------------------------------------------------|
| `IDC_IMAGE_PLAYING_BG<map>`                            | The background per map ID. The map ID is picked at run time. |
| `IDC_IMAGE_NOTE_BG<map>`                               | The note field background per map ID.                        |
| `IDC_IMAGE_NOTE_MEASURE1`, `IDC_IMAGE_NOTE_MEASURE2`   | The measure lines.                                           |
| `IDC_ANIMATION_NOTE_NORMAL<n>_<s>`                     | The normal note per channel `1`–`7` and shape `0`–`1`.       |
| `IDC_ANIMATION_NOTE_LONG<n>_<s>`                       | The long note per channel `1`–`7` and shape `0`–`1`.         |
| `IDC_LIST_NOTE_CLICK`, `IDC_ANIMATION_NOTE_CLICK`      | The key hit effect, generated per channel.                   |
| `IDC_LIST_LONG_NOTE_EFFECT`                            | The long note effect list.                                   |
| `IDC_ANIMATION_NOTE_COMBO`, `IDC_NUMBER_NOTE_COMBO`    | The combo effect and counter.                                |
| `IDC_ANIMATION_NOTE_COOL`, `…_GOOD`, `…_BAD`, `…_MISS` | The judgement effects.                                       |
| `IDC_CONTAINER_NOTE_JAM`                               | The jam effect container.                                    |
| `IDC_ANIMATION_EFFECT_JAM`, `IDC_NUMBER_EFFECT_JAM`    | The optional jam effect and counter.                         |
| `IDC_IMAGE_KEY_EFFECT`, `IDC_IMAGE_GUIDE_KEY_EFFECT`   | The key effects, generated once per key.                     |

These objects are the mandatory ones, and each of them is filled from the `require` entries above:

| Object                      | Type          | Notes                                                                             |
|-----------------------------|---------------|-----------------------------------------------------------------------------------|
| `IDC_CONTAINER_KEY_EFFECT`  | `UiContainer` | Receives one `IDC_IMAGE_KEY_EFFECT` per key.                                      |
| `IDC_LIST_NOTE_CLICK`       | `List`        | Receives one `IDC_ANIMATION_NOTE_CLICK` per channel.                              |
| `IDC_LIST_LONG_NOTE_EFFECT` | `List`        | Holds the long note effects declared by `IDC_LIST_LONG_NOTE_EFFECT` in `require`. |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "StatePlaying7K",
  "name": "STATE_PLAYING_7K",
  "require": {
    "IDC_IMAGE_PLAYING_BG1": {
      "type": "Image",
      "require": {
        "refID": "0X1B100001"
      }
    },
    "IDC_ANIMATION_NOTE_NORMAL1_0": {
      "type": "Animation",
      "require": {
        "refID": "0X1B500010"
      }
    }
  },
  "attributes": {
    "viewport": 480,
    "objects": {
      "IDC_CONTAINER_KEY_EFFECT": "ControlList/State/Playing7K/KeyEffect.json"
    }
  }
}
```

</td>
<td>

```json
{
  "type": "StatePlaying7K",
  "name": "STATE_PLAYING_7K",
  "require": {
    "IDC_IMAGE_PLAYING_BG1": {
      "type": "Image",
      "require": {
        "texture": "Playing/Custom/Bg1.png"
      }
    },
    "IDC_ANIMATION_NOTE_NORMAL1_0": {
      "type": "Animation",
      "require": {
        "texture": "Playing/Custom/Note1.png"
      },
      "attributes": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 44,
          "height": 12
        }
      }
    }
  },
  "attributes": {
    "viewport": 480,
    "objects": {
      "IDC_CONTAINER_KEY_EFFECT": "Playing/State/Playing7K/KeyEffect.json"
    }
  }
}
```

</td>
</tr>
</table>

## Graphics

### Animation

Represents a frame sequence. Frames come from explicit `frames` entries, from the O2Jam sprite, or from the sprite's own frame table when `infer` is set.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute                                        | Type   | Default         | Description                                                                                  |
|--------------------------------------------------|--------|-----------------|----------------------------------------------------------------------------------------------|
| `frames`                                         | object | —               | The frame entries, in the order they appear. Each accepts `texCoords`, `id` and `transform`. |
| `infer`                                          | bool   | `false`         | Whether to infer the frames from the O2Jam sprite, keeping the sprite's own positions.       |
| `isLoop`                                         | bool   | `false`         | Whether the sequence restarts after the last frame.                                          |
| `repeatCount`                                    | number | `0`             | The number of repeats. `0` is unlimited when looping.                                        |
| `speed`                                          | number | `1.0`           | The playback rate multiplier.                                                                |
| `duration`                                       | time   | 60 ms per frame | The total duration of one cycle.                                                             |
| `alignment`                                      | string | `None`          | The automatic origin placement. See [Animation Alignment](#animation-alignment).             |
| `transform`, `color`, `blend`, `texCoords`, `id` |        |                 | As for [Sprite](#sprite).                                                                    |

Within a frame, `id` selects the O2Jam sprite frame to draw; without it, frames map to sprite frames by position. 
A frame that has no `transform.position` inherits the parent position, or the sprite frame's own position.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Animation",
  "require": {
    "refID": "0X1950000B",
    "bndID": "0X1950000B"
  },
  "attributes": {
    "isLoop": true,
    "duration": 1300
  }
}
```

</td>
<td>

```json
{
  "type": "Animation",
  "require": {
    "texture": "Interface/Custom/Tower.png"
  },
  "attributes": {
    "isLoop": true,
    "duration": 1300,
    "transform": {
      "position": {
        "x": 420,
        "y": 96
      }
    },
    "frames": {
      "0": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 128,
          "height": 128
        }
      },
      "1": {
        "texCoords": {
          "x": 128,
          "y": 0,
          "width": 128,
          "height": 128
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### Shape

Represents a vector primitive. The primitive is selected by `attributes.type`, which is separate from the metadata's own `type`.

`Shape` is the only visual type with no O2Jam sprite path. It reads `texture`, and takes its position from the boundary table or from `transform`.

**Require:**
- `texture`
- `bndID` / `globalBndID` / `localBndID`

| Attribute          | Type   | Default     | Description                                                       |
|--------------------|--------|-------------|-------------------------------------------------------------------|
| `type`             | string | —           | `Circle`, `Polygon`, `Rectangle` or `RoundedRectangle`. Required. |
| `color`            | color  | Black       | The fill colour.                                                  |
| `colorMap`         | object | —           | The per-vertex colours, applied in the order they appear.         |
| `outlineThickness` | number | `0`         | The outline width.                                                |
| `outlineColor`     | color  | Transparent | The outline colour.                                               |
| `texCoords`        | rect   | —           | The texture rectangle when a texture is supplied.                 |
| `transform`        | object | —           | See [Transform](#transform).                                      |

Per-primitive attributes:

| Primitive          | Attributes                                                                     |
|--------------------|--------------------------------------------------------------------------------|
| `Circle`           | `radius` (`0`), `pointCount` (`30`)                                            |
| `Polygon`          | `pointCount` (`30`), `points` (array of vectors)                               |
| `Rectangle`        | `width` (`0`), `height` (`0`)                                                  |
| `RoundedRectangle` | `width` (`0`), `height` (`0`), `cornerRadius` (`1`), `cornerPointCount` (`30`) |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Shape",
  "require": {
    "bndID": "0X29017012"
  },
  "attributes": {
    "type": "Rectangle",
    "width": 320,
    "height": 24,
    "color": {
      "a": 160,
      "r": 0,
      "g": 0,
      "b": 0
    }
  }
}
```

</td>
<td>

```json
{
  "type": "Shape",
  "require": {
    "texture": "Interface/Custom/Panel.png"
  },
  "attributes": {
    "type": "RoundedRectangle",
    "width": 320,
    "height": 24,
    "cornerRadius": 4,
    "transform": {
      "position": {
        "x": 240,
        "y": 180
      }
    }
  }
}
```

</td>
</tr>
</table>

### Sprite

Represents a single static image drawn from a texture, or from one frame of an O2Jam sprite.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute   | Type   | Default | Description                                                                     |
|-------------|--------|---------|---------------------------------------------------------------------------------|
| `transform` | object | —       | See [Transform](#transform).                                                    |
| `id`        | number | —       | The frame index in the O2Jam sprite. It also sets the position from that frame. |
| `texCoords` | rect   | —       | The rectangle to draw, overriding the frame rectangle.                          |
| `color`     | color  | White   | The tint colour.                                                                |
| `blend`     | string | `Auto`  | See [Blend Mode](#blend-mode).                                                  |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Sprite",
  "require": {
    "refID": "0X19100001"
  }
}
```

</td>
<td>

```json
{
  "type": "Sprite",
  "require": {
    "texture": "Interface/Custom/Planet_Bg.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 0,
        "y": 0
      }
    },
    "texCoords": {
      "x": 0,
      "y": 0,
      "width": 800,
      "height": 600
    }
  }
}
```

</td>
</tr>
</table>

## Common UI

### BitmapNumber

Represents a number whose digits are drawn from a texture or an O2Jam sprite, with optional per-digit animation.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute    | Type   | Default         | Description                                                                  |
|--------------|--------|-----------------|------------------------------------------------------------------------------|
| `digits`     | object | —               | Maps digit `0`–`9` to a rect, or to an array of rects for an animated digit. |
| `digitSize`  | object | —               | The `width` and `height` of one digit.                                       |
| `digitCount` | number | `1`             | The number of digit slots to render.                                         |
| `duration`   | time   | 60 ms per frame | The animation duration per digit.                                            |
| `value`      | number | `0`             | The initial value.                                                           |
| `kerning`    | number | `0`             | The spacing between digits.                                                  |
| `alignment`  | string | `None`          | `None`, `Left`, `Center` or `Right`.                                         |
| `color`      | color  | White           | The tint colour.                                                             |
| `blend`      | string | `Auto`          | See [Blend Mode](#blend-mode).                                               |
| `transform`  | object | —               | See [Transform](#transform).                                                 |

With an O2Jam sprite and no `digits`, exactly 10 frames map one per digit; more than 10 frames are split evenly into per-digit animations.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "BitmapNumber",
  "require": {
    "refID": "0X1B50006E"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 150,
        "y": 150
      }
    },
    "alignment": "Center",
    "duration": 480
  }
}
```

</td>
<td>

```json
{
  "type": "BitmapNumber",
  "require": {
    "texture": "Playing/Custom/Combo_Digits.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 150,
        "y": 150
      }
    },
    "digitSize": {
      "width": 16,
      "height": 24
    },
    "digitCount": 4,
    "alignment": "Center",
    "digits": {
      "0": {
        "x": 0,
        "y": 0,
        "width": 16,
        "height": 24
      },
      "1": {
        "x": 16,
        "y": 0,
        "width": 16,
        "height": 24
      }
    }
  }
}
```

</td>
</tr>
</table>

### Button

Represents a three-state clickable control.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute                                        | Type   | Default | Description                                                   |
|--------------------------------------------------|--------|---------|---------------------------------------------------------------|
| `states`                                         | object | —       | The frames per state. Keys are `normal`, `hover` and `click`. |
| `enabled`                                        | bool   | `true`  | Whether the button accepts input.                             |
| `visible`                                        | bool   | `true`  | Whether the button is drawn.                                  |
| `transform`, `color`, `blend`, `texCoords`, `id` |        |         | As for [Sprite](#sprite).                                     |

Each state entry accepts the sprite attributes, plus:

| Key         | Type   | Description                                                 |
|-------------|--------|-------------------------------------------------------------|
| `texCoords` | rect   | The texture rectangle for this state.                       |
| `bounds`    | rect   | The local bound for this state. `x` and `y` default to `0`. |
| `id`        | number | The O2Jam sprite frame index to use instead of `texCoords`. |

When no `states` are given and an O2Jam sprite is used, frames map by count: one frame drives `Active`, two drive `Hover`/`Active`, 
three drive `Normal`/`Hover`/`Active`, and more than three use the first frame for `Normal`/`Hover` and the last for `Active`.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Button",
  "require": {
    "refID": "0X19200007",
    "bndID": "0X19200007"
  }
}
```

</td>
<td>

```json
{
  "type": "Button",
  "require": {
    "texture": "Interface/Custom/Btn_Exit.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 640,
        "y": 12
      }
    },
    "states": {
      "normal": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 60,
          "height": 24
        }
      },
      "hover": {
        "texCoords": {
          "x": 60,
          "y": 0,
          "width": 60,
          "height": 24
        }
      },
      "click": {
        "texCoords": {
          "x": 120,
          "y": 0,
          "width": 60,
          "height": 24
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### Dialog

Represents a modal window with an optional prompt label and accept/cancel buttons. A dialog is composed but not attached to the scene tree; it is shown on demand.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute                                  | Type   | Description                                                    |
|--------------------------------------------|--------|----------------------------------------------------------------|
| `label`                                    | object | The metadata for the prompt label, in full envelope form.      |
| `buttons`                                  | object | `accept` and `cancel`, each a full [Button](#button) metadata. |
| `objects`                                  | object | Any additional children.                                       |
| `transform`, `color`, `texCoords`, `blend` |        | As for [Sprite](#sprite).                                      |

When neither a texture nor an O2Jam sprite resolves, the dialog's texture rectangle falls back to the bound from `bndID`.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Dialog",
  "require": {
    "refID": "0X29100001"
  },
  "attributes": {
    "buttons": {
      "accept": {
        "type": "Button",
        "require": {
          "refID": "0X29200002",
          "bndID": "0X29200002"
        }
      },
      "cancel": {
        "type": "Button",
        "require": {
          "refID": "0X29200003",
          "bndID": "0X29200003"
        }
      }
    }
  }
}
```

</td>
<td>

```json
{
  "type": "Dialog",
  "require": {
    "texture": "Interface/Custom/Dialog.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 200,
        "y": 150
      }
    },
    "texCoords": {
      "x": 0,
      "y": 0,
      "width": 400,
      "height": 220
    },
    "buttons": {
      "accept": {
        "type": "Button",
        "require": {
          "texture": "Interface/Custom/Btn_Ok.png"
        },
        "attributes": {
          "transform": {
            "position": {
              "x": 90,
              "y": 180
            }
          }
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### Gauge

Represents a bar whose fill tracks a value, with optional flicker and animation frames.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute                                  | Type   | Default         | Description                                             |
|--------------------------------------------|--------|-----------------|---------------------------------------------------------|
| `orientation`                              | string | `Horizontal`    | `Horizontal` or `Vertical`.                             |
| `maximum`                                  | number | `100`           | The value that corresponds to a full bar.               |
| `frame`                                    | number | —               | The O2Jam sprite frame index to use as the bar texture. |
| `frames`                                   | array  | —               | The animation frames, each accepting sprite attributes. |
| `duration`                                 | time   | 60 ms per frame | The animation duration.                                 |
| `flicker`                                  | bool   | `false`         | Whether the bar flickers.                               |
| `transform`, `color`, `texCoords`, `blend` |        |                 | As for [Sprite](#sprite).                               |

With an O2Jam sprite of more than one frame and no explicit `frame`, every sprite frame becomes an animation frame.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Gauge",
  "require": {
    "refID": "0X1B500029"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 90,
        "y": 65
      }
    },
    "orientation": "Horizontal",
    "frame": 1
  }
}
```

</td>
<td>

```json
{
  "type": "Gauge",
  "require": {
    "texture": "Playing/Custom/Life_Bar.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 90,
        "y": 65
      }
    },
    "texCoords": {
      "x": 0,
      "y": 0,
      "width": 64,
      "height": 8
    },
    "orientation": "Horizontal",
    "maximum": 100
  }
}
```

</td>
</tr>
</table>

### Image

Represents a sprite with named frames, sizing behaviour, and a local bound. Most static interface elements use this type.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute                                        | Type           | Default  | Description                                                                            |
|--------------------------------------------------|----------------|----------|----------------------------------------------------------------------------------------|
| `frames`                                         | object         | —        | The named frames. Each accepts `position`, `origin`, `rotation`, `texCoords` and `id`. |
| `sizeMode`                                       | string         | `Normal` | `Normal`, `Center` or `Stretch`.                                                       |
| `bounds`                                         | rect \| string | —        | The local bound as a rectangle, or a bound ID whose size is used.                      |
| `transform`, `color`, `blend`, `texCoords`, `id` |                |          | As for [Sprite](#sprite).                                                              |

When `frames` is omitted, a single frame named `default` is built from the metadata's own `texCoords` and transform.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Image",
  "require": {
    "refID": "0X1B500029"
  },
  "attributes": {
    "frames": {
      "default": {
        "id": 0,
        "position": {
          "x": 90,
          "y": 65
        }
      }
    }
  }
}
```

</td>
<td>

```json
{
  "type": "Image",
  "require": {
    "texture": "Interface/Custom/Life.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 90,
        "y": 65
      }
    },
    "frames": {
      "default": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 64,
          "height": 8
        }
      },
      "low": {
        "texCoords": {
          "x": 0,
          "y": 8,
          "width": 64,
          "height": 8
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### InputField

Represents an editable text field.

**Require:**
- `font`
- `fallbackFonts`
- `bndID` / `globalBndID` / `localBndID`

| Attribute            | Type   | Default | Description                                                               |
|----------------------|--------|---------|---------------------------------------------------------------------------|
| `bounds`             | rect   | —       | The local bound. It falls back to the bound from `bndID`, offset by 2 px. |
| `maximumLength`      | number | `0`     | The maximum number of characters. `0` is unlimited.                       |
| `highlightTextColor` | color  | Black   | The selected text colour.                                                 |
| `highlightBackColor` | color  | White   | The selection background colour.                                          |
| Label attributes     |        |         | All attributes of [Label](#label) apply.                                  |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "InputField",
  "require": {
    "font": "Interface/Common/Font.ttf",
    "bndID": "0X27300004"
  },
  "attributes": {
    "fontSize": 12,
    "maximumLength": 20
  }
}
```

</td>
<td>

```json
{
  "type": "InputField",
  "require": {
    "font": "Interface/Custom/Pixel.ttf"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 220,
        "y": 260
      }
    },
    "bounds": {
      "x": 0,
      "y": 0,
      "width": 180,
      "height": 16
    },
    "fontSize": 12,
    "maximumLength": 20,
    "highlightBackColor": {
      "a": 255,
      "r": 60,
      "g": 120,
      "b": 220
    }
  }
}
```

</td>
</tr>
</table>

### Label

Represents text rendered with a font.

**Require:**
- `font`
- `fallbackFonts`
- `bndID` / `globalBndID` / `localBndID`

| Attribute    | Type   | Default   | Description                                                      |
|--------------|--------|-----------|------------------------------------------------------------------|
| `string`     | string | Empty     | The text content, UTF-8.                                         |
| `fontSize`   | number | `30`      | The character size.                                              |
| `fontWidth`  | number | `0`       | The forced character width. `0` keeps the font's own metrics.    |
| `color`      | color  | White     | The text colour.                                                 |
| `bold`       | bool   | `false`   | Whether the text is bold.                                        |
| `italic`     | bool   | `false`   | Whether the text is italic.                                      |
| `underlined` | bool   | `false`   | Whether the text is underlined.                                  |
| `kerning`    | number | `0`       | The extra letter spacing. It applies only when greater than `0`. |
| `outline`    | object | —         | The outline `thickness` (number) and `color`.                    |
| `alignment`  | string | `Default` | `Default`, `Left`, `Center` or `Right`.                          |
| `transform`  | object | —         | See [Transform](#transform).                                     |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Label",
  "require": {
    "font": "Interface/Common/Font.ttf",
    "bndID": "0X19400012"
  },
  "attributes": {
    "fontSize": 10
  }
}
```

</td>
<td>

```json
{
  "type": "Label",
  "require": {
    "font": "Interface/Custom/Pixel.ttf",
    "fallbackFonts": [
      "Interface/Common/Fallback-Font1.ttf"
    ]
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 120,
        "y": 47
      }
    },
    "fontSize": 10,
    "outline": {
      "thickness": 1.25,
      "color": {
        "a": 255,
        "r": 25,
        "g": 25,
        "b": 25
      }
    }
  }
}
```

</td>
</tr>
</table>

### List

Represents a repeating container. Children are either declared individually in `objects`, or generated from a template.

**Require:**
- `template`
- `bndID` / `globalBndID` / `localBndID`

`require.template` generates the children:

| Key      | Type      | Description                                                                                                                               |
|----------|-----------|-------------------------------------------------------------------------------------------------------------------------------------------|
| `name`   | string    | The base name. Children are named `name1`, `name2`, …                                                                                     |
| `source` | reference | The metadata used for every item.                                                                                                         |
| `count`  | number    | The number of items to generate.                                                                                                          |
| `bounds` | array     | The bound IDs. One item is generated per bound, positioned relative to the list's own bound, and `count` is derived from the list length. |

| Attribute    | Type   | Default    | Description                                                                  |
|--------------|--------|------------|------------------------------------------------------------------------------|
| `layouts`    | array  | —          | The explicit per-item transforms. They override the repeat attributes below. |
| `vertical`   | object | —          | The `count` and `spacing` for vertical repetition.                           |
| `horizontal` | object | —          | The `count` and `spacing` for horizontal repetition.                         |
| `order`      | string | `Vertical` | The fill order, `Vertical` or `Horizontal`.                                  |
| `transform`  | object | —          | See [Transform](#transform).                                                 |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "List",
  "require": {
    "bndID": "0X29017012",
    "template": {
      "name": "IDC_RADIO_MUSIC",
      "source": "ControlList/Dialog/SelectMusic/SelectMusic_Btn_Select.json",
      "bounds": [
        "0X29017012",
        "0X29017013",
        "0X29017014"
      ]
    }
  }
}
```

</td>
<td>

```json
{
  "type": "List",
  "require": {
    "template": {
      "name": "IDC_RADIO_MUSIC",
      "source": "Interface/Custom/MusicEntry.json",
      "count": 15
    }
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 40,
        "y": 120
      }
    },
    "vertical": {
      "count": 15,
      "spacing": 18
    },
    "order": "Vertical"
  }
}
```

</td>
</tr>
</table>

### RadioButton

Represents a button that stays active until another button in the same container is selected. 
It takes the same fields as [Button](#button), with one difference: the third state key is `active` instead of `click`.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "RadioButton",
  "require": {
    "refID": "0X19100015",
    "bndID": "0X19100015"
  }
}
```

</td>
<td>

```json
{
  "type": "RadioButton",
  "require": {
    "texture": "Interface/Custom/Btn_Channel.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 96,
        "y": 240
      }
    },
    "states": {
      "normal": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 48,
          "height": 20
        }
      },
      "hover": {
        "texCoords": {
          "x": 48,
          "y": 0,
          "width": 48,
          "height": 20
        }
      },
      "active": {
        "texCoords": {
          "x": 96,
          "y": 0,
          "width": 48,
          "height": 20
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### ScrollBar

Represents a draggable value bar.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute                                  | Type   | Default      | Description                                               |
|--------------------------------------------|--------|--------------|-----------------------------------------------------------|
| `orientation`                              | string | `Horizontal` | `Horizontal` or `Vertical`.                               |
| `maximum`                                  | number | `100`        | The maximum value.                                        |
| `step`                                     | number | `1`          | The value change per step.                                |
| `bounds`                                   | rect   | —            | The local bound. It falls back to the bound from `bndID`. |
| `transform`, `color`, `texCoords`, `blend` |        |              | As for [Sprite](#sprite).                                 |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "ScrollBar",
  "require": {
    "refID": "0X29102011",
    "bndID": "0X29102011"
  },
  "attributes": {
    "orientation": "Vertical",
    "maximum": 100
  }
}
```

</td>
<td>

```json
{
  "type": "ScrollBar",
  "require": {
    "texture": "Interface/Custom/ScrollBar.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 620,
        "y": 120
      }
    },
    "texCoords": {
      "x": 0,
      "y": 0,
      "width": 12,
      "height": 32
    },
    "bounds": {
      "x": 0,
      "y": 0,
      "width": 12,
      "height": 240
    },
    "orientation": "Vertical",
    "step": 5
  }
}
```

</td>
</tr>
</table>

### ToggleButton

Represents a button that switches between its normal and active state on its own, independently of its siblings. 
It takes the same fields as [Button](#button), with one difference: the third state key is `active` instead of `click`.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "ToggleButton",
  "require": {
    "refID": "0X29017005",
    "bndID": "0X29017005"
  }
}
```

</td>
<td>

```json
{
  "type": "ToggleButton",
  "require": {
    "texture": "Interface/Custom/Btn_Toggle.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 300,
        "y": 180
      }
    },
    "states": {
      "normal": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 24,
          "height": 24
        }
      },
      "active": {
        "texCoords": {
          "x": 24,
          "y": 0,
          "width": 24,
          "height": 24
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### ToolTip

Represents a label inside a floating container.

**Require:**
- `font`
- `bndID` / `globalBndID` / `localBndID`

| Attribute        | Type   | Default | Description                                                                                                                                             |
|------------------|--------|---------|---------------------------------------------------------------------------------------------------------------------------------------------------------|
| `duration`       | time   | `3000`  | The time the tooltip stays visible.                                                                                                                     |
| `delay`          | time   | `0`     | The delay before it appears.                                                                                                                            |
| `color`          | color  | Black   | The text colour.                                                                                                                                        |
| `container`      | object | —       | The container styling: `padding` (vector, defaults to 10% of the font size), `color` (white), and `outline` with `thickness` (`1`) and `color` (black). |
| Label attributes |        |         | All attributes of [Label](#label) apply.                                                                                                                |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "ToolTip",
  "require": {
    "font": "Interface/Common/Font.ttf",
    "bndID": "0X27200006"
  },
  "attributes": {
    "fontSize": 12,
    "duration": 3000
  }
}
```

</td>
<td>

```json
{
  "type": "ToolTip",
  "require": {
    "font": "Interface/Custom/Pixel.ttf"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 300,
        "y": 420
      }
    },
    "fontSize": 12,
    "delay": 250,
    "container": {
      "padding": {
        "x": 4,
        "y": 2
      },
      "color": {
        "a": 235,
        "r": 20,
        "g": 20,
        "b": 20
      },
      "outline": {
        "thickness": 1,
        "color": {
          "a": 255,
          "r": 90,
          "g": 90,
          "b": 90
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### UiContainer

Represents a grouping node with a local bound. It draws nothing itself.

**Require:**
- `bndID` / `globalBndID` / `localBndID`

| Attribute   | Type             | Description                                                                         |
|-------------|------------------|-------------------------------------------------------------------------------------|
| `bounds`    | object \| string | The local bound as `{ "width": …, "height": … }`, or a bound ID whose size is used. |
| `transform` | object           | See [Transform](#transform).                                                        |
| `objects`   | object           | The children.                                                                       |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "UiContainer",
  "require": {
    "bndID": "0X19600200"
  },
  "attributes": {
    "objects": {
      "IDC_RADIO_BEGINNER_01": "ControlList/State/Planet/Planet_Btn_Beginner_01.json"
    }
  }
}
```

</td>
<td>

```json
{
  "type": "UiContainer",
  "attributes": {
    "transform": {
      "position": {
        "x": 32,
        "y": 200
      }
    },
    "bounds": {
      "width": 240,
      "height": 120
    },
    "objects": {
      "IDC_RADIO_BEGINNER_01": "Interface/Custom/Btn_Beginner.json"
    }
  }
}
```

</td>
</tr>
</table>

## Custom UI

Custom UI types are game-specific types built on the common UI types. Each is read as the type it derives from, so every field of that type applies:

| Type                 | Parsed As     | Extra Fields                    |
|----------------------|---------------|---------------------------------|
| `AvatarInfo`         | `UiContainer` | [AvatarInfo](#avatarinfo)       |
| `ChannelBoard`       | `Image`       | [ChannelBoard](#channelboard)   |
| `ChannelButton`      | `RadioButton` | [ChannelButton](#channelbutton) |
| `ChatPanel`          | `UiContainer` | None                            |
| `ChatWindow`         | `List`        | [ChatWindow](#chatwindow)       |
| `CreateRoomDialog`   | `Dialog`      | None                            |
| `Equalizer`          | `UiContainer` | [Equalizer](#equalizer)         |
| `InstrumentSelector` | `UiContainer` | None                            |
| `MapSelector`        | `UiContainer` | None                            |
| `Marquee`            | `Label`       | [Marquee](#marquee)             |
| `OptionDialog`       | `Dialog`      | None                            |
| `PlayMenu`           | `Image`       | None                            |
| `RoomButton`         | `Image`       | None                            |
| `RoomList`           | `List`        | None                            |
| `SelectMusicDialog`  | `Dialog`      | None                            |
| `SpeedButton`        | `RadioButton` | [SpeedButton](#speedbutton)     |
| `UserList`           | `UiContainer` | None                            |

The types with no extra fields are declared exactly like the type they are parsed as; only the `type` value differs.

### AvatarInfo

Represents the per-player info panel shown next to an avatar.

**Require:**
- `bndID` / `globalBndID` / `localBndID`

| Attribute    | Type   | Description                                      |
|--------------|--------|--------------------------------------------------|
| `teamColors` | object | Maps a team letter (`A`–`H`) to a colour.        |
| `transform`  | object | See [Transform](#transform).                     |
| `objects`    | object | The children, typically labels and a life gauge. |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "AvatarInfo",
  "require": {
    "bndID": "0X1B500029"
  },
  "attributes": {
    "objects": {
      "IDC_GAUGE_AVATAR_INFO_LIFE": {
        "type": "Gauge",
        "require": {
          "refID": "0X1B500029"
        },
        "attributes": {
          "frame": 1
        }
      }
    }
  }
}
```

</td>
<td>

```json
{
  "type": "AvatarInfo",
  "attributes": {
    "transform": {
      "position": {
        "x": 90,
        "y": 40
      }
    },
    "teamColors": {
      "A": {
        "a": 255,
        "r": 220,
        "g": 60,
        "b": 60
      },
      "B": {
        "a": 255,
        "r": 60,
        "g": 120,
        "b": 220
      }
    },
    "objects": {
      "IDC_GAUGE_AVATAR_INFO_LIFE": {
        "type": "Gauge",
        "require": {
          "texture": "Playing/Custom/Life_Bar.png"
        },
        "attributes": {
          "texCoords": {
            "x": 0,
            "y": 0,
            "width": 64,
            "height": 8
          }
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### ChannelBoard

Represents the notice and channel list board. It uses two named frames, `Notice` and `ChannelList`.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute        | Type   | Description                              |
|------------------|--------|------------------------------------------|
| `channelCount`   | number | The number of channels listed per page.  |
| Image attributes |        | All attributes of [Image](#image) apply. |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "ChannelBoard",
  "require": {
    "refID": "0X19100040",
    "bndID": "0X19100040"
  },
  "attributes": {
    "channelCount": 8
  }
}
```

</td>
<td>

```json
{
  "type": "ChannelBoard",
  "require": {
    "texture": "Interface/Custom/Board.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 420,
        "y": 120
      }
    },
    "channelCount": 8,
    "frames": {
      "Notice": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 240,
          "height": 160
        }
      },
      "ChannelList": {
        "texCoords": {
          "x": 240,
          "y": 0,
          "width": 240,
          "height": 160
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### ChannelButton

Represents a channel selector button with one frame set per channel mode.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute                                  | Type   | Description                                                                                                                     |
|--------------------------------------------|--------|---------------------------------------------------------------------------------------------------------------------------------|
| `mode`                                     | object | The frame set per mode: `Beginner`, `Intermediate` or `High`, each with a `states` object keyed `normal`, `hover` and `active`. |
| `transform`, `color`, `texCoords`, `blend` |        | As for [Sprite](#sprite).                                                                                                       |

With an O2Jam sprite and no `mode` entries, the first two frames become `Normal` and `Hover`, and the mode index selects the `Active` frame from frame 2 onward.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "ChannelButton",
  "require": {
    "refID": "0X19100031",
    "bndID": "0X19100031"
  }
}
```

</td>
<td>

```json
{
  "type": "ChannelButton",
  "require": {
    "texture": "Interface/Custom/Channel.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 96,
        "y": 240
      }
    },
    "mode": {
      "Beginner": {
        "states": {
          "normal": {
            "texCoords": {
              "x": 0,
              "y": 0,
              "width": 48,
              "height": 20
            }
          },
          "hover": {
            "texCoords": {
              "x": 48,
              "y": 0,
              "width": 48,
              "height": 20
            }
          },
          "active": {
            "texCoords": {
              "x": 96,
              "y": 0,
              "width": 48,
              "height": 20
            }
          }
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### ChatWindow

Represents a scrolling chat log.

**Require:**
- `font`
- `fallbackFonts`
- `bndID` / `globalBndID` / `localBndID`

| Attribute           | Type   | Default | Description                                               |
|---------------------|--------|---------|-----------------------------------------------------------|
| `fontSize`          | number | `13`    | The character size.                                       |
| `color`             | color  | White   | The text colour.                                          |
| `maximumChatLength` | number | —       | The maximum number of retained messages.                  |
| `lineSpacing`       | number | `0`     | The extra spacing between lines.                          |
| `bounds`            | rect   | —       | The local bound. It falls back to the bound from `bndID`. |
| `transform`         | object | —       | See [Transform](#transform).                              |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "ChatWindow",
  "require": {
    "font": "Interface/Common/Font.ttf",
    "bndID": "0X21400008"
  },
  "attributes": {
    "fontSize": 13
  }
}
```

</td>
<td>

```json
{
  "type": "ChatWindow",
  "require": {
    "font": "Interface/Custom/Pixel.ttf",
    "fallbackFonts": [
      "Interface/Common/Fallback-Font1.ttf"
    ]
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 12,
        "y": 400
      }
    },
    "bounds": {
      "x": 0,
      "y": 0,
      "width": 360,
      "height": 120
    },
    "fontSize": 13,
    "lineSpacing": 2,
    "maximumChatLength": 60
  }
}
```

</td>
</tr>
</table>

### Equalizer

Represents the music visualizer display.

Every **string keyed** entry in this type's `require` map is composed as a child object, named after its key.

| Attribute   | Type   | Description                                       |
|-------------|--------|---------------------------------------------------|
| `bounds`    | object | The local bound as `{ "width": …, "height": … }`. |
| `transform` | object | See [Transform](#transform).                      |
| `objects`   | object | The children.                                     |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Equalizer",
  "require": {
    "IDC_IMAGE_EQUALIZER_BAR": "ControlList/State/Playing7K/Playing_Eq_Bar.json"
  },
  "attributes": {
    "bounds": {
      "width": 160,
      "height": 60
    }
  }
}
```

</td>
<td>

```json
{
  "type": "Equalizer",
  "require": {
    "IDC_IMAGE_EQUALIZER_BAR": "Playing/Custom/Eq_Bar.json"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 600,
        "y": 480
      }
    },
    "bounds": {
      "width": 160,
      "height": 60
    }
  }
}
```

</td>
</tr>
</table>

### Marquee

Represents scrolling text clipped to a bound.

**Require:**
- `font`
- `fallbackFonts`
- `bndID` / `globalBndID` / `localBndID`

| Attribute        | Type   | Default | Description                                              |
|------------------|--------|---------|----------------------------------------------------------|
| `speed`          | number | `30`    | The scroll speed.                                        |
| `bounds`         | rect   | —       | The clip bound. It falls back to the bound from `bndID`. |
| Label attributes |        |         | All attributes of [Label](#label) apply.                 |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Marquee",
  "require": {
    "font": "Interface/Common/Font.ttf",
    "bndID": "0X19400021"
  },
  "attributes": {
    "fontSize": 12,
    "speed": 30
  }
}
```

</td>
<td>

```json
{
  "type": "Marquee",
  "require": {
    "font": "Interface/Custom/Pixel.ttf"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 20,
        "y": 570
      }
    },
    "bounds": {
      "x": 0,
      "y": 0,
      "width": 760,
      "height": 16
    },
    "fontSize": 12,
    "speed": 45
  }
}
```

</td>
</tr>
</table>

### SpeedButton

Represents a radio button that carries a speed value.

**Require:**
- `texture` / `sprite` / `refID`
- `bndID` / `globalBndID` / `localBndID`

| Attribute         | Type   | Description                                                                      |
|-------------------|--------|----------------------------------------------------------------------------------|
| `speed`           | number | The speed value applied when selected.                                           |
| `mode`            | string | `HiSpeed`, `XrSpeed` or `TdSpeed`.                                               |
| Button attributes |        | All attributes of [Button](#button) apply, with `active` as the third state key. |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "SpeedButton",
  "require": {
    "refID": "0X21200030",
    "bndID": "0X21200030"
  },
  "attributes": {
    "speed": 2.0,
    "mode": "HiSpeed"
  }
}
```

</td>
<td>

```json
{
  "type": "SpeedButton",
  "require": {
    "texture": "Interface/Custom/Btn_Speed.png"
  },
  "attributes": {
    "transform": {
      "position": {
        "x": 480,
        "y": 300
      }
    },
    "speed": 2.0,
    "mode": "HiSpeed",
    "states": {
      "normal": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 32,
          "height": 16
        }
      },
      "active": {
        "texCoords": {
          "x": 32,
          "y": 0,
          "width": 32,
          "height": 16
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

## Avatar

### Avatar

Represents a rendered character. Default items for the gender are equipped first, then the listed item IDs.

**Require:**
- `bndID` / `globalBndID` / `localBndID`

| Attribute   | Type   | Description                                                     |
|-------------|--------|-----------------------------------------------------------------|
| `items`     | array  | The item IDs to equip, in order.                                |
| `gender`    | string | `Male`, `Female` or `Any`.                                      |
| `offset`    | vector | The offset applied to every equipped part.                      |
| `transform` | object | See [Transform](#transform).                                    |
| `objects`   | object | The children, e.g. effect animations and an `AvatarInfo` panel. |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Avatar",
  "require": {
    "bndID": "0X1B500009"
  },
  "attributes": {
    "items": [],
    "objects": {
      "IDC_ANIMATION_OHM": {
        "type": "Animation",
        "require": {
          "refID": "0X1B500086"
        },
        "attributes": {
          "isLoop": true
        }
      }
    }
  }
}
```

</td>
<td>

```json
{
  "type": "Avatar",
  "attributes": {
    "transform": {
      "position": {
        "x": 120,
        "y": 187
      }
    },
    "gender": "Female",
    "offset": {
      "x": 0,
      "y": -4
    },
    "items": [
      1001,
      1002
    ],
    "objects": {
      "IDC_ANIMATION_OHM": {
        "type": "Animation",
        "require": {
          "texture": "Avatar/Custom/Ohm.png"
        },
        "attributes": {
          "isLoop": true
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

### Item

Represents one avatar item: its shop identity, its thumbnails, and renderable node per gender, body part and instrument.

| Require key      | Description                                                                                                                                                                      |
|------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `thumbnail`      | The `small` and `large` thumbnails, each a reference to a `Sprite`.                                                                                                              |
| `male`, `female` | Maps a body part and an instrument to a reference to an `Animation`. Parts: `body`, `leftArm`, `rightArm`, `back`. Instruments: `default`, `guitar`, `bass`, `keyboard`, `drum`. |

| Attribute     | Type   | Description                                                            |
|---------------|--------|------------------------------------------------------------------------|
| `id`          | number | The item ID. Required.                                                 |
| `name`        | string | The display name, UTF-8. Required.                                     |
| `description` | string | The shop description, UTF-8. Required.                                 |
| `isNew`       | bool   | Whether the item shows the "new" label. Required.                      |
| `gender`      | string | `Male`, `Female` or `Any`. Required.                                   |
| `type`        | string | The equipment type. See [Equipment Types](#equipment-types). Required. |
| `origin`      | string | The planet of origin. See [Planets](#planets). Required.               |
| `price`       | object | Maps a currency (`gem`, `cash`) to an amount.                          |

A reference may name an O2Jam sprite directly, or point at metadata that resolves its own texture.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Item",
  "require": {
    "thumbnail": {
      "small": "Avatar/Items/Body_S.ojs",
      "large": "Avatar/Items/Body_L.ojs"
    },
    "female": {
      "body": {
        "default": "Avatar/Items/Body_F.ojs",
        "keyboard": "Avatar/Items/Body_F_Key.ojs"
      }
    }
  },
  "attributes": {
    "id": 30,
    "isNew": false,
    "name": "Basic Body",
    "description": "",
    "gender": "Female",
    "origin": "O2Planet",
    "type": "Body",
    "price": {
      "gem": 0
    }
  }
}
```

</td>
<td>

```json
{
  "type": "Item",
  "require": {
    "thumbnail": {
      "small": "Avatar/Custom/Hair_S.json",
      "large": "Avatar/Custom/Hair_L.json"
    },
    "female": {
      "body": {
        "default": "Avatar/Custom/Hair_Idle.json",
        "keyboard": "Avatar/Custom/Hair_Keyboard.json"
      }
    }
  },
  "attributes": {
    "id": 1001,
    "isNew": true,
    "name": "Custom Hair",
    "description": "Example item",
    "gender": "Female",
    "origin": "O2Planet",
    "type": "Hair",
    "price": {
      "gem": 1200
    }
  }
}
```

</td>
</tr>
</table>

### ItemData

Represents the item table. It is read from `Avatar/ItemData.json`, and the binary `Itemdata*.dat` is only consulted when that file is absent.

| Field                | Description                                   |
|----------------------|-----------------------------------------------|
| `require.items`      | Maps an item ID to an [Item](#item) metadata. |
| `attributes.version` | The free-form version string.                 |

Without this file the game falls back to `Itemdata*.dat` shipped inside `Avatar.opa`, where every item reference points at an O2Jam sprite. 
Authoring the metadata form replaces that binary table entirely, in [interop mode](Asset.md#interop-mode) as well, so it has to carry every item the game should know about. 
See [Overriding Avatars](Asset.md#overriding-avatars) for where the table is picked up from.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "ItemData",
  "require": {
    "items": {
      "30": {
        "type": "Item",
        "require": {
          "thumbnail": {
            "small": "Avatar/Items/Body_S.ojs",
            "large": "Avatar/Items/Body_L.ojs"
          },
          "female": {
            "body": {
              "default": "Avatar/Items/Body_F.ojs"
            }
          }
        },
        "attributes": {
          "id": 30,
          "isNew": false,
          "name": "Basic Body",
          "description": "",
          "gender": "Female",
          "origin": "O2Planet",
          "type": "Body",
          "price": {
            "gem": 0
          }
        }
      }
    }
  },
  "attributes": {
    "version": "3.10"
  }
}
```

</td>
<td>

```json
{
  "type": "ItemData",
  "require": {
    "items": {
      "1001": {
        "type": "Item",
        "require": {
          "thumbnail": {
            "small": "Avatar/Custom/Hair_S.json",
            "large": "Avatar/Custom/Hair_L.json"
          },
          "female": {
            "body": {
              "default": "Avatar/Custom/Hair_Idle.json"
            }
          }
        },
        "attributes": {
          "id": 1001,
          "isNew": true,
          "name": "Custom Hair",
          "description": "Example item",
          "gender": "Female",
          "origin": "O2Planet",
          "type": "Hair",
          "price": {
            "gem": 1200
          }
        }
      }
    }
  },
  "attributes": {
    "version": "custom"
  }
}
```

</td>
</tr>
</table>

### SetInfoData

Represents the item set packages sold in the shop. 
It is read from `Avatar/SetInfoData.json`, and the binary `setinfodata.*` is only consulted when that file is absent.

| Field                | Description                      |
|----------------------|----------------------------------|
| `require.sets`       | Maps a set ID to a set metadata. |
| `attributes.version` | The free-form version string.    |

Each set metadata contains:

| Field                    | Type   | Description                                                            |
|--------------------------|--------|------------------------------------------------------------------------|
| `require.items`          | array  | The item IDs in the set. A set with no items is skipped.               |
| `attributes.id`          | number | The set ID. Required.                                                  |
| `attributes.name`        | string | The set name, UTF-8. Required.                                         |
| `attributes.description` | string | The set description, UTF-8. Required.                                  |
| `attributes.gender`      | string | `Male`, `Female` or `Any`. Required.                                   |
| `attributes.origin`      | string | The planet of origin. Required.                                        |
| `attributes.discounts`   | object | Maps an item ID to a currency (`Gem`, `Cash`) and the discount amount. |

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "SetInfoData",
  "require": {
    "sets": {
      "1": {
        "require": {
          "items": [
            30,
            31,
            32
          ]
        },
        "attributes": {
          "id": 1,
          "name": "Starter Set",
          "description": "",
          "gender": "Female",
          "origin": "O2Planet"
        }
      }
    }
  },
  "attributes": {
    "version": "3.10"
  }
}
```

</td>
<td>

```json
{
  "type": "SetInfoData",
  "require": {
    "sets": {
      "900": {
        "require": {
          "items": [
            1001,
            1002
          ]
        },
        "attributes": {
          "id": 900,
          "name": "Custom Set",
          "description": "Example set",
          "gender": "Female",
          "origin": "O2Planet",
          "discounts": {
            "1001": {
              "Gem": 200
            }
          }
        }
      }
    }
  },
  "attributes": {
    "version": "custom"
  }
}
```

</td>
</tr>
</table>

## Audio

### Music

Represents a streamed music track.

| Require key | Description             |
|-------------|-------------------------|
| `music`     | The music file to play. |

| Attribute | Type | Default | Description             |
|-----------|------|---------|-------------------------|
| `loop`    | bool | `false` | Whether playback loops. |

As with `Sound`, a reference to a music file is accepted directly, without metadata.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Music",
  "require": {
    "music": "BGM:1007"
  },
  "attributes": {
    "loop": true
  }
}
```

</td>
<td>

```json
{
  "type": "Music",
  "require": {
    "music": "Music/Custom/Theme.ogg"
  },
  "attributes": {
    "loop": true
  }
}
```

</td>
</tr>
</table>

### Sound

Represents a one-shot sound effect.

| Require key | Description             |
|-------------|-------------------------|
| `sound`     | The sound file to play. |

A reference to a sound file is also accepted directly, without metadata. 
Samples inside a mounted O2Jam sound bank are addressed as `<Bank>:<index>`, e.g. `bgEffect:02`.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Sound",
  "require": {
    "sound": "bgEffect:02"
  }
}
```

</td>
<td>

```json
{
  "type": "Sound",
  "require": {
    "sound": "Music/Custom/Click.ogg"
  }
}
```

</td>
</tr>
</table>

## Standalone

### Cursor

Represents the mouse cursor set. It is not a scene node, so it is loaded on its own and applied to the window.

**Require:**
- `texture` / `sprite`

| Attribute | Type   | Description                                                  |
|-----------|--------|--------------------------------------------------------------|
| `states`  | object | Maps a cursor type to its image. Keys are cursor type names. |

Each state entry contains:

| Key         | Type   | Description                                                |
|-------------|--------|------------------------------------------------------------|
| `id`        | number | The O2Jam sprite frame index to cut the cursor image from. |
| `texCoords` | rect   | The texture rectangle, used when no `id` is given.         |
| `hotspot`   | vector | The click point inside the cursor image.                   |

State keys are matched case-sensitively against the cursor types listed in [Cursor Types](#cursor-types). With no `states`, the whole texture becomes the default cursor.

<table>
<tr><th>Interop Reference</th><th>Custom Reference</th></tr>
<tr valign="top">
<td>

```json
{
  "type": "Cursor",
  "require": {
    "sprite": "Window_Cursor.ojs"
  },
  "attributes": {
    "states": {
      "Arrow": {
        "id": 0,
        "hotspot": {
          "x": 0,
          "y": 7
        }
      },
      "Click": {
        "id": 1,
        "hotspot": {
          "x": 0,
          "y": 7
        }
      }
    }
  }
}
```

</td>
<td>

```json
{
  "type": "Cursor",
  "require": {
    "texture": "Interface/Custom/Cursor.png"
  },
  "attributes": {
    "states": {
      "Arrow": {
        "texCoords": {
          "x": 0,
          "y": 0,
          "width": 32,
          "height": 32
        },
        "hotspot": {
          "x": 0,
          "y": 7
        }
      },
      "Click": {
        "texCoords": {
          "x": 32,
          "y": 0,
          "width": 32,
          "height": 32
        },
        "hotspot": {
          "x": 0,
          "y": 7
        }
      }
    }
  }
}
```

</td>
</tr>
</table>

## Enumerations

Most enumeration values are matched case-insensitively. These are matched **case-sensitively** and must be spelled exactly as listed:

- Cursor types.
- `gender`, `origin` and `type` in [Item](#item) and [SetInfoData](#setinfodata). The `gender` attribute on [Avatar](#avatar) is case-insensitive.
- Render part and instrument keys in an item's `require` map. Both `leftArm` and `left_arm` resolve to `LeftArm`; `leftarm` does not.

State keys of buttons (`normal`, `hover`, `click`, `active`) and item price keys are literal names, not enumerations.

### Blend Mode

Specifies how a graphical node is blended with what is already drawn.

- `Auto`
- `Alpha`
- `Additive`
- `Multiplicative`
- `Min`
- `Max`
- `None`

### Size Mode

Specifies how an image fills its bound.

- `Normal`
- `Center`
- `Stretch`

### Animation Alignment

Specifies how the origin of each animation frame is placed.

- `None`
- `Center`
- `HorizontalCenter`
- `VerticalCenter`

### Text Alignment

Specifies how lines of text are aligned.

- `Default`
- `Left`
- `Center`
- `Right`

### Bitmap Number Alignment

Specifies how digits are aligned within their bound.

- `None`
- `Left`
- `Center`
- `Right`

### List Order

Specifies the direction a list fills its items.

- `Vertical`
- `Horizontal`

### Orientation

Specifies the axis a gauge or a scroll bar runs along.

- `Horizontal`
- `Vertical`

### Control States

Specifies the interaction states of a button.

- `normal`
- `hover`
- `click` (`Button` only)
- `active` (`ToggleButton`, `RadioButton`, `SpeedButton` and `ChannelButton`)

### Cursor Types

Specifies which system cursor a state replaces. Matched **case-sensitively**.

- `Arrow`
- `ArrowWait`
- `Click`
- `Wait`
- `Text`
- `Hand`
- `SizeHorizontal`
- `SizeVertical`
- `SizeTopLeftBottomRight`
- `SizeBottomLeftTopRight`
- `SizeLeft`
- `SizeRight`
- `SizeTop`
- `SizeBottom`
- `SizeTopLeft`
- `SizeBottomRight`
- `SizeBottomLeft`
- `SizeTopRight`
- `SizeAll`
- `Cross`
- `Help`
- `NotAllowed`

### Channel Modes

Specifies the tier a channel button represents.

- `Beginner`
- `Intermediate`
- `High`

### Speed Modes

Specifies how the speed value of a speed button is applied.

- `HiSpeed`
- `XrSpeed`
- `TdSpeed` (3D Speed)

### Teams

Specifies the team a player belongs to.

- `A`
- `B`
- `C`
- `D`
- `E`
- `F`
- `G`
- `H`

### Gender

Specifies the gender an avatar or an item belongs to. Matched **case-sensitively** in item and set tables.

- `Male`
- `Female`
- `Any`

### Equipment Types

Specifies the slot an item occupies.

- `Body`
- `LeftArm`
- `RightArm`
- `LeftHand`
- `RightHand`
- `Face`
- `Hair`
- `Glasses`
- `Earrings`
- `Necklace`
- `ClothesAccessories`
- `Accessories`
- `Gloves`
- `Pants`
- `Shoes`
- `Keyboard`
- `Guitar`
- `Drum`
- `Bass`
- `Top`
- `Wings`
- `InstrumentAccessories`
- `Pet`
- `HairAccessories`
- `AttributiveItem`
- `NicknameChanger`
- `PenaltyResetter`
- `BagExpansion`
- `Costume`

### Render Parts

Specifies the part of the avatar a renderable belongs to.

- `Body`
- `LeftArm`
- `RightArm`
- `Back`
- `SmallThumbnail`
- `LargeThumbnail`

### Instruments

Specifies the instrument a renderable is worn with.

- `None` (written as `default`)
- `Guitar`
- `Bass`
- `Keyboard`
- `Drum`

### Planets

Specifies the planet an item or a set originates from.

- `Unknown`
- `O2Planet`
- `Aqua`
- `Eliten`
- `Graffiti`
- `Bikini`
- `Crush`
- `Wonderland`
- `Meganut`
- `Crystal`
- `Draconic`
- `Event`

### Currency

Specifies the currency a price is denominated in.

- `Gem`
- `Cash`
