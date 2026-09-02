# HeresABug

A Qt 6 / QML desktop tool for capturing **structured bug reports while operating a physical device**.

## Why this exists

When testers find a bug on real hardware, they usually write it up by hand: free-form notes,
screenshots, a paragraph in a chat channel. Every report comes back in a different shape — different
wording for the same button, missing repro steps, no consistent notion of "what was the outcome".
Collecting and triaging those reports is slow, and comparing them is nearly impossible.

HeresABug replaces the free-form write-up with a **recorder**. While a session is being recorded,
every UI action the tester performs on an on-screen replica of the device is logged as a fixed-width,
machine-readable event. The tester ends the session with a single verdict (PASS / FAIL), and the
whole sequence is appended as **one row of an Excel `.xlsx` file** in a fixed layout. Reports are now
uniform, diff-able, and countable.

## Reusable for any device

The app is not hard-wired to one device. The C++ core (recording, Excel I/O, navigation) is fixed;
the **UI is data**. Each device gets its own folder under `qml/`, containing QML files that mirror
*that* device's screens and controls. You select which one to build with a single CMake option:

```
cmake --preset qt-mingw -DHERESABUG_APP_DIR=<your-device-folder>
```

`qml/example/` is a complete working reference; `qml/example_from_scratch/` is a minimal starting
point. To support a new device you add `qml/<device>/` with a `Main.qml`, a `MetaUi.qml`, a
`pages.json`, and one `.qml` file per screen — no C++ changes.

## Build & run

Requirements (paths are pinned in `CMakePresets.json`):

- Qt 6.11.2 for MinGW 64-bit, and the bundled MinGW 13.1.0 toolchain + Ninja
- A vcpkg instance with `OpenXLSX` installed for the `x64-mingw-dynamic` triplet
  (`vcpkg install openxlsx:x64-mingw-dynamic`)

```
cmake --preset qt-mingw -DHERESABUG_APP_DIR=example
cmake --build --preset qt-mingw
```

The executable is `build/HeresABug.exe`. Running it needs both the Qt `mingw_64/bin` directory and
the vcpkg triplet's `bin` directory (`.../installed/x64-mingw-dynamic/bin`, for the OpenXLSX runtime
DLLs) on `PATH`.

There is no test target or CI. The one automated check is Qt's generated qmllint pass:
`cmake --build --preset qt-mingw --target all_qmllint`.

## How a recording session works

1. **New File** / **Load File** — choose the `.xlsx` the reports are written to.
2. **Record Start** — begins a session (disabled until a file is chosen). Recording jumps to the
   first page.
3. The tester drives the on-screen device. Each control press logs one event; moving between screens
   is navigation. Nothing is logged while recording is stopped.
4. **Record Stop / Start** can be used to pause. Resuming returns to the exact page recording was
   stopped on (by unwinding the navigation done while paused, not by stacking onto it).
4. **Export** — appends the session as one row, then resets the app to its start state (recording
   off, empty record, back on the first page). The file stays selected for the next report.
5. **Delete One** removes the last event (and reverts the page move that came with it); **Clear**
   discards the whole in-progress record.

### Excel layout

`Sheet1`: `A1` = `"Total Counts"`, `B1` = row count, row 2 is the header
(`Bug Report ID | Events | Results | Note`), reports start at row 3 — one per row:

| Column | Contents |
| --- | --- |
| A `Bug Report ID` | every event's packed id concatenated, then the result id |
| B `Events` | each event's `{page_id:value}` string, joined by `", "` |
| C `Results` | the verdict's `{000:value}` string |
| D `Note` | note that need to decribe  more |

`reference/REF_bug_report.xlsx` is a sample in this layout.

## Architecture (C++ core)

Three `QObject` controllers are registered in `main.cpp` as QML context properties; each wraps a
plain, Qt-free class that holds the real logic:

| Controller | Wraps | Responsibility |
| --- | --- | --- |
| `record_controller` | `record` | the in-progress event list + verdict; the recording on/off gate; undo bookkeeping |
| `excel_io_controller` | `excel_io` | CRUD over the `.xlsx` file (OpenXLSX) |
| `navigation_controller` | `page_registry` | which page id/type is active, the back-history, the pause/resume anchor |

**Event id** (`common.hpp`, `EVENT_ID_LENGTH` = 10 characters):

```
page_id(3) + ui_id(3) + ui_#(2) + ui_action(2)
```

`record_controller` wraps every logged `value` as `"{" + page_id + ":" + value + "}"` before storing
it, so what lands in the Excel cell is always `{page_id:value}`.

Per-project rule: every UI event handler is a one-line delegate to a single-responsibility class or
component — never inline logic. See `CODING_GUIDELINES.md`.

---

# Authoring a device UI

Everything below lives in `qml/<device>/`, except the shared toolkit in `qml/common/`. Files in a
device folder must `import HeresABug` to see the toolkit (the two folders are merged into one QML
module but are physically separate).

## The shared toolkit — `qml/common/`

These components are built into *every* device build. You compose them; you don't edit them.

### Page-scoped controls

Placed on a screen, tagged with a full event id, and logged **only while recording**. Common
properties: `page_id`, `ui_id`, `ui_num`, `ui_action`, `value`.

| Component | Behaviour |
| --- | --- |
| `Button` | logs one event on click |
| `NavButton` | a `Button` that *also* navigates to `target_page_id` — logs the event first, then switches page. Needs no `onClicked`. |
| `ToggleButton` | an on/off switch. `ui_action` is derived from its state (`"01"` on / `"00"` off), not set by you; it appends `" On"` / `" Off"` to `value` |
| `InputField` | a text field. On editing-finished it logs `value + " " + <text typed>` |

### Meta control — page-independent

| Component | Behaviour |
| --- | --- |
| `MetaButton` | `page_id` is fixed to `"000"`. Routes on `ui_id`: **`"RES"`** submits the session verdict (this is the only thing that enables Export); any other `ui_id` logs a page-independent event (still recording-gated), e.g. a hardware key. |

### App-chrome — used by `Main.qml`, not by pages

`NewFileButton`, `LoadFileButton`, `RecordToggleButton`, `DeleteOneButton`, `ClearRecordButton`,
`ExportButton`, `PageLoader` (renders the active page in a sub-region and overlays a Back button),
`EventLogText` (the status-panel event line — last 5 events, with a leading `"..."` when there are
more).

### `ui_id` mnemonics (convention, not enforced)

`BTN` button / nav · `TGL` toggle · `TXT` text field · `RES` verdict (PASS/FAIL) ·
`PWR` device hard key. `ui_#` is a `"00"`-based index per `(page, ui_id)` pair, in document order.

## `Main.qml` — the window

The device folder's `Main.qml` provides the module's `Main` type (what the app loads at startup). It
is the same *shape* for every device — only the arrangement changes:

- top-left: `NewFileButton` + `LoadFileButton`
- top-right: `RecordToggleButton` + `DeleteOneButton` + `ClearRecordButton` + `ExportButton`
- left, full height: a `Rectangle` containing one `PageLoader` — **the only region that changes on
  navigation**
- right: a status box (recording state, event count, an `EventLogText`, the current verdict), then
  one `MetaUi` instance below it

`Main.qml` also holds one wiring block so **Delete One** can revert a page move:

```qml
Connections {
    target: navigation_controller
    function onCurrent_page_changed() { record_controller.mark_navigation_occurred() }
}
```

The file/record controls and the status box are identical for every device, so they are written out
here directly. Everything device-specific is delegated to `MetaUi.qml`.

> A page must never contain a `PageLoader` itself — it is already *being shown by* the one in
> `Main.qml`, and would recurse.

## `MetaUi.qml` — the device's page-independent controls

`MetaUi.qml` is the one file where a device declares its **hardware buttons and its verdict
buttons** — the controls that aren't tied to any single screen. It is a `Column` with two framed
areas:

- **device / hardware buttons** — `MetaButton`s with a non-`"RES"` `ui_id` (e.g. `ui_id: "PWR"`),
  one per physical key the tester can press during a repro (`qml/example/` has `ON/OFF` and `STOP`)
- **verdict buttons** — `MetaButton`s with `ui_id: "RES"`, normally `PASS` and `FAIL`. Pressing one
  sets the session's single result; the last press wins.

`Main.qml` only *places* the `MetaUi`; it does not know what's in it. A device with no hardware keys
yet leaves the first area empty (see `qml/example_from_scratch/MetaUi.qml`).

```qml
// qml/<device>/MetaUi.qml
import QtQuick
import HeresABug

Column {
    property int item_height: 40

    Row {   // hardware keys
        MetaButton { text: "ON/OFF"; ui_id: "PWR"; ui_num: "00"; ui_action: "01"; value: "Power ON/OFF" }
        MetaButton { text: "STOP";   ui_id: "PWR"; ui_num: "00"; ui_action: "02"; value: "Power STOP" }
    }
    Row {   // verdict
        MetaButton { text: "PASS"; ui_id: "RES"; ui_num: "00"; ui_action: "01"; value: "Result PASS" }
        MetaButton { text: "FAIL"; ui_id: "RES"; ui_num: "00"; ui_action: "02"; value: "Result FAIL" }
    }
}
```

## Drawing a screen

Each screen is a plain QML file (a `Rectangle` or `Item`) that fills its parent and composes toolkit
controls. It does **not** anchor itself to the window or manage navigation chrome — `PageLoader`
handles placement and the Back button.

```qml
// qml/<device>/HomeScreen.qml
import QtQuick
import HeresABug

Rectangle {
    anchors.fill: parent
    color: "#eef6ff"

    Column {
        anchors.centerIn: parent
        spacing: 12

        Button {
            text: "Log sample event"
            page_id: "001"; ui_id: "BTN"; ui_num: "00"; ui_action: "00"
            value: text
        }
        NavButton {
            text: "Go to Settings"
            page_id: "001"; ui_id: "BTN"; ui_num: "01"; ui_action: "00"
            value: text
            target_page_id: "002"      // <- forward navigation
        }
    }
}
```

Tag **every** page-scoped control with the four id parts. `value` is usually just `text`.

## Defining navigation

Navigation is driven by `pages.json` in the device folder — a `page_id -> qml_type` map plus the
starting page:

```json
{
    "initial_page_id": "001",
    "pages": [
        { "page_id": "001", "qml_type": "HomeScreen" },
        { "page_id": "002", "qml_type": "SettingsScreen" },
        { "page_id": "003", "qml_type": "DiagnosticsScreen" }
    ]
}
```

- **Forward:** set `target_page_id` on a `NavButton`. On click it logs its event, then
  `navigation_controller.go_to_page(target_page_id)` runs, pushing the previous page onto the
  back-history.
- **Back:** the Back button `PageLoader` overlays pops the back-history
  (`navigation_controller.go_back()`). No screen needs its own back control.
- **Session start / after Export:** `go_to_initial_page()` clears the history and jumps to
  `initial_page_id`.
- Every `page_id` is a 3-digit string (`"001"`..`"999"`), matching the event id's `page_id` slot.
  `pages.json` must exist even if empty (`{}`).

`PageLoader` creates each visited screen once and keeps it alive (hidden) when you navigate away, so
a screen's local state — a `ToggleButton`'s position, text typed into an `InputField` — survives
leaving and returning.

## Checklist: add a new device

1. `mkdir qml/<device>/`
2. Add `Main.qml` (copy `qml/example/Main.qml`, adjust layout), `MetaUi.qml` (your hardware +
   verdict buttons), and `pages.json`.
3. Add one `.qml` per screen; register each in `pages.json`; tag every control with its event id.
4. `cmake --preset qt-mingw -DHERESABUG_APP_DIR=<device>` then
   `cmake --build --preset qt-mingw`.

New `.qml` files are picked up automatically on the next configure — no `CMakeLists.txt` edit.
