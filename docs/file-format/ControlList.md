# O2Jam `ControlList.txt` File Format Documentation

This document describes the O2Jam `ControlList.txt` format used to define game user-interface composition.

## File Structure

The control list is defined in a text-based format with a hierarchical structure of states, objects, and nested sets.
This format is similar to VDF or JSON.

- Comments begin with double slashes (`//`) and continue to the end of the line
- Parameters are separated by whitespace (spaces, tabs, or a combination)
- Parameters are either a number formatted in hexadecimal or a string expressing file name enclosed in double quotes (`"file_name.ext"`)
- Each object definition (name and parameters) must appear within a single line.
- Curly braces (`{}`) represents a container object which can contains other objects.

## Top-Level Definitions

### `NUMBER_OF_STATES`

Defines the number of `STATE_*` objects that will be present in the file.

```
NUMBER_OF_STATES    0x01
```

### `STATE`

States represent states (or also known as scenes) in the game. Each state must have a unique name and typically start with the `STATE_` prefix.

```
STATE_LOGO    0x03    0x00
{
    // State objects
}
```

Parameters:
1. Number of state objects
2. State ID

## Object Types

### `BOUND` Declaration

Each `STATE` must contain exactly one `BOUND` declaration, which points to a `.bnd` file describing the boundaries (position, dimensions) of objects in the state.

```
BOUND    "file.bnd"
```

Parameter:
- Boundary file name

### `IDC` Objects

`IDC` (Stands for `ID Control`) is a regular object act as basic building blocks of the control list. 
They typically follow the naming convention `IDC_<Type>_<Name>`.

```
IDC_IMAGE_BACKGROUND    0x11020304    "background.ojs"
```

Parameters:
1. Object attributes (hexadecimal—split into four parts: 1 byte each)
   - Byte 1: State ID (typically STATE ID + 11)
   - Byte 2: Object type
   - Byte 3: Group/layer ID
   - Byte 4: Object ID
2. Asset file name (typically ending in `.ojs`, `.oji`, `.ojt`, or `.oja`)

### `SET` Objects

SET objects are containers that group multiple objects, typically arranged as rows or columns.

```
SET 0x02    0x11600200
{
    // Nested objects
}
```

Parameters:
1. Number of objects inside the SET
2. Object attributes (same format as regular objects)

## Object Type Codes

The second byte of the object attributes defines the object type:

| Code | Object Type            |
|------|------------------------|
| 00   | `STATE`                |
| 01   | Toggle/Radio/Checkbox  |
| 10   | Image/Sprite           |
| 02   | Scroll Bar             |
| 20   | Button                 |
| 30   | Input Text Box         |
| 50   | Interactable/Clickable |
| 60   | `SET`                  |
| 70   | Avatar                 |

Note: The game often uses `Interactable` as a `Button` or a `Toggle`.

## Example

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