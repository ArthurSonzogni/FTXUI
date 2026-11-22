# Événements, focus et souris

Comment FTXUI route les événements clavier, déplace le focus entre les composants et gère les entrées souris.

## Parcours des événements dans l'arbre

Lorsqu'une touche est pressée, qu'une action souris survient ou que le terminal est redimensionné :

1. `App` transmet l'`Event` au **composant racine** via `root->OnEvent(event)`.
2. Un conteneur (`Container::Vertical`, `Container::Horizontal`, `Container::Tab`, `Container::Stacked`) transmet d'abord les événements clavier à son **enfant actif**.
3. Si l'enfant retourne `true`, l'événement est **consommé** et la propagation s'arrête.
4. Si l'enfant retourne `false`, le conteneur gère lui-même les touches de navigation (par exemple `ArrowUp`/`ArrowDown` dans `Container::Vertical`, `ArrowLeft`/`ArrowRight` dans `Container::Horizontal`, `Tab`/`TabReverse` entre les enfants focusables).

```text
  App → root->OnEvent(e)
           │
           ▼
     active child->OnEvent(e)
       ├── retourne true  → terminé (événement consommé)
       └── retourne false → le conteneur gère la navigation / remonte
```

## Intercepter les événements avec `CatchEvent`

`CatchEvent` enveloppe un composant et inspecte chaque `Event` **avant** qu'il n'atteigne le composant interne :

```cpp
auto app = App::Fullscreen();

auto ui = main_component
          | CatchEvent([&](Event event) {
              if (event == Event::Character('q')) {
                app.Exit();
                return true;  // Consommé : le composant interne ne verra pas 'q'
              }
              return false;   // Non consommé : transmis à main_component
            });
```

::: warning Raccourcis globaux et `Input`
Comme `CatchEvent` s'exécute **avant** les enfants, intercepter `Event::Character('q')` à la racine empêchera de saisir `'q'` dans un champ `Input` ! Deux solutions :
- Utilisez des touches avec modificateur (`Event::CtrlQ`, `Event::Escape`), ou
- Vérifiez `!input->Focused()` avant de consommer des caractères imprimables.
:::

### Vérifications d'`Event` courantes

```cpp
if (event == Event::Return) { /* Entrée */ }
if (event == Event::Escape) { /* Échap */ }
if (event == Event::CtrlC)  { /* Ctrl+C */ }
if (event.is_character())   { std::string ch = event.character(); }
if (event.is_mouse())       { Mouse m = event.mouse(); }
```

## Focus et conteneurs

- Seuls les composants dont `Focusable()` retourne `true` peuvent recevoir le focus. Un `Renderer(...)` sans enfant focusable n'est pas focusable.
- **`Container::Vertical` / `Container::Horizontal`** suivent automatiquement l'index de l'enfant actif (vous pouvez aussi passer un pointeur `int* selected` explicite pour observer ou contrôler le focus).
- `Container::Tab({tab0, tab1, tab2}, &tab_index)` route les événements uniquement vers `children[tab_index]`. Les onglets inactifs ne reçoivent pas d'événements et ne prennent pas le focus.
- **Focus programmatique :** appelez `child->TakeFocus()` pour déplacer directement le focus sur un composant donné.
- **Interroger le focus :** dans un `Renderer`, appelez `child->Focused()` (cette feuille précise a le focus) ou `child->Active()` (l'enfant ou l'un de ses descendants a le focus) pour styler les panneaux actifs.

```cpp
auto pane = Renderer(child, [&] {
  auto border_style = child->Active() ? borderDouble : borderLight;
  return child->Render() | border_style;
});
```

## Modales et isolation des événements

`Modal(dialog, &show)` superpose `dialog` au-dessus de l'interface principale (via `dbox` + `clear_under`) **et** bloque tous les événements clavier/souris vers l'arrière-plan tant que `show == true` :

```cpp
bool show_modal = false;
auto ui = main_component
          | Modal(dialog_component, &show_modal);
```

Dès que `show_modal` passe à `false`, les événements reprennent immédiatement leur routage vers `main_component`.

## Événements souris et détection de zone (hit-testing)

La prise en charge de la souris est activée automatiquement dans `App`. Une structure `Mouse` contient :

- `mouse.x`, `mouse.y` : coordonnées de cellule du terminal (indexées à partir de 0)
- `mouse.button` : `Mouse::Left`, `Mouse::Middle`, `Mouse::Right`, `Mouse::None`, `Mouse::WheelUp`, `Mouse::WheelDown`
- `mouse.motion` : `Mouse::Pressed`, `Mouse::Released`, `Mouse::Moved`
- Modificateurs : `mouse.shift`, `mouse.meta`, `mouse.control`

Pour détecter un clic ou un survol sur une zone personnalisée, capturez son rectangle rendu avec `reflect(box)` dans `Render()`, puis testez `box.Contain(x, y)` dans `OnEvent()` :

```cpp
Box card_box;
bool hovered = false;

auto card = Renderer([&] {
  auto style = hovered ? color(Color::Yellow) : color(Color::White);
  return text(" Click or hover me ")
         | style
         | borderRounded
         | reflect(card_box);
});

auto interactive_card = card
                        | CatchEvent([&](Event event) {
                            if (!event.is_mouse()) {
                              return false;
                            }
                            hovered = card_box.Contain(event.mouse().x, event.mouse().y);
                            if (hovered &&
                                event.mouse().button == Mouse::Left &&
                                event.mouse().motion == Mouse::Pressed) {
                              // Gérer le clic
                              return true;
                            }
                            return false;
                          });
```

::: tip Décorateur intégré `Hoverable`
Si vous avez seulement besoin d'un booléen de survol, utilisez le décorateur intégré au lieu d'écrire la détection souris à la main :
```cpp
bool is_hovered = false;
auto card = base_component | Hoverable(&is_hovered);
```
:::
