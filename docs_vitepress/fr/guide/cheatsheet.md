# Aide-mémoire

Une référence rapide sur une page pour développer au quotidien avec FTXUI.

## En-têtes

```cpp
#include <ftxui/screen/screen.hpp>       // Screen, Surface, Cell, Color, Box
#include <ftxui/dom/elements.hpp>        // hbox, vbox, text, border, flex, ...
#include <ftxui/dom/table.hpp>           // Table
#include <ftxui/dom/canvas.hpp>          // Canvas
#include <ftxui/component/component.hpp> // Button, Input, Menu, Container, ...
#include <ftxui/component/app.hpp>       // App
#include <ftxui/component/loop.hpp>      // Loop (boucle d'événements manuelle)
```

## Éléments de mise en page (`ftxui/dom`)

| Élément | Rôle |
| --- | --- |
| `hbox({...})` / `vbox({...})` | Boîte horizontale / verticale |
| `dbox({...})` | Calques empilés en profondeur (à associer avec `clear_under`) |
| `flexbox({...}, config)` | Mise en page à retour à la ligne de type CSS (raccourcis `hflow`, `vflow`) |
| `gridbox({ {...}, {...} })` | Grille 2D alignée |
| `filler()` / `emptyElement()` | Espace vide extensible / élément 0×0 |
| `separator()` | Séparateur horizontal ou vertical (`separatorLight`, `Double`, `Heavy`, `Dashed`) |
| `window(title, content)` | Boîte encadrée avec barre de titre |

## Décorateurs (à chaîner avec `|` à gauche)

```cpp
auto el = text("Hello")
          | bold
          | color(Color::Cyan)
          | bgcolor(Color::Black)
          | borderRounded
          | center
          | flex;
```

| Catégorie | Décorateurs |
| --- | --- |
| **Style de texte** | `bold`, `dim`, `italic`, `inverted`, `underlined`, `underlinedDouble`, `blink`, `strikethrough` |
| **Couleur** | `color(Color::Red)`, `bgcolor(Color::Blue)`, `color(LinearGradient(...))` |
| **Bordures** | `border`, `borderLight`, `borderHeavy`, `borderDouble`, `borderRounded`, `borderDashed`, `borderEmpty`, `borderStyled(...)` |
| **Flex et taille** | `flex`, `flex_grow`, `flex_shrink`, `xflex`, `yflex`, `notflex`, `size(WIDTH, EQUAL, 20)` (`LESS_THAN`, `GREATER_THAN`) |
| **Alignement** | `center`, `hcenter`, `vcenter`, `align_right` |
| **Défilement** | `frame`, `xframe`, `yframe`, `vscroll_indicator`, `hscroll_indicator`, `focus`, `focusPositionRelative(x, y)` |
| **Utilitaires** | `clear_under`, `reflect(box)`, `automerge`, `hyperlink(url)` |

## Composants interactifs (`ftxui/component`)

| Composant | Utilisation typique |
| --- | --- |
| `Button` | `auto btn = Button("Click", [&] { ... });` |
| `Input` | `auto in = Input(&str, "placeholder");` |
| `Checkbox` | `auto cb = Checkbox("Enable", &flag);` |
| `Menu` | `auto m = Menu(&entries, &selected);` |
| `Radiobox` | `auto rb = Radiobox(&entries, &selected);` |
| `Dropdown` | `auto dd = Dropdown(&entries, &selected);` |
| `Toggle` | `auto tg = Toggle(&entries, &selected);` |
| `Slider` | `auto sl = Slider("Volume:", &val, 0, 100, 5);` |
| `Collapsible` | `auto col = Collapsible("Details", child);` |
| `Modal` | `auto ui = main \| Modal(dialog, &show);` |
| `Window` | `auto win = Window({.inner = child, .title = "Title"});` |
| `ResizableSplit` | `ResizableSplitLeft(left, right, &size)` (`Right`, `Top`, `Bottom`) |
| `Container` | `Container::Vertical({...})`, `Horizontal`, `Tab({...}, &idx)`, `Stacked` |
| `Renderer` | `Renderer(container, [&] { return vbox({...}); })` |
| `CatchEvent` | `comp \| CatchEvent([&](Event e) { return false; })` |
| `Maybe` | `comp \| Maybe(&visible)` |
| `Hoverable` | `comp \| Hoverable(&hovered)` |

## Événements courants (`ftxui::Event`)

| Touches | Événement |
| --- | --- |
| Caractère imprimable | `Event::Character('q')`, `event.is_character()`, `event.character()` |
| Flèches | `Event::ArrowUp`, `ArrowDown`, `ArrowLeft`, `ArrowRight` (`*Ctrl`, `*Alt`) |
| Navigation | `Event::Tab`, `TabReverse`, `Home`, `End`, `PageUp`, `PageDown` |
| Édition | `Event::Return`, `Escape`, `Backspace`, `Delete`, `Insert` |
| Contrôle | `Event::CtrlC`, `CtrlD`, `CtrlA`..`CtrlZ`, `AltA`..`AltZ`, `F1`..`F12` |
| Souris | `event.is_mouse()`, `event.mouse().button` (`Mouse::Left`, `WheelUp`, `WheelDown`), `event.mouse().motion` (`Pressed`, `Released`, `Moved`), `x`, `y` |

## Cycle de vie d'`App`

```cpp
auto app = App::Fullscreen();      // ou TerminalOutput(), FitComponent(), FixedSize(w, h)
app.Loop(root_component);          // Boucle principale bloquante
app.Exit();                        // Quitte la boucle (ou app.ExitLoopClosure())
app.Post([&] { /* state */ });     // Callback thread-safe sur le thread principal
app.RequestAnimationFrame();       // Planifie un nouveau rendu
app.PrintAbove(element);           // Sortie permanente au-dessus (mode TerminalOutput)
app.WithRestoredIO([&] { ... });   // Suspend le mode brut pour lancer un éditeur/outil externe
```
