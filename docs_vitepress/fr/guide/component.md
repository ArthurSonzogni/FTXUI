# Module Component (`ftxui/component`)

Le module **Component** permet de construire des applications terminal interactives avec état au-dessus de `ftxui/dom` et `ftxui/screen`.

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
```

<FtxuiDemo file="component/homescreen" height="520px" title="Live demo: component/homescreen (click tabs or use arrow keys)" />

## Widgets interactifs intégrés

Tous les composants intégrés se lient directement à vos variables C++ classiques via des pointeurs ou `Ref<T>` :

| Composant | Exemple rapide | Démo interactive |
| --- | --- | --- |
| `Button` | `auto btn = Button("Click", [&] { count++; });` | [`component/button`](/examples/component/button) |
| `Input` | `auto in = Input(&text_val, "placeholder");` | [`component/input`](/examples/component/input) |
| `Checkbox` | `auto cb = Checkbox("Enable", &enabled);` | [`component/checkbox`](/examples/component/checkbox) |
| `Menu` | `auto m = Menu(&entries, &selected);` | [`component/menu`](/examples/component/menu) |
| `Toggle` | `auto tg = Toggle(&entries, &selected);` | [`component/toggle`](/examples/component/toggle) |
| `Radiobox` | `auto rb = Radiobox(&entries, &selected);` | [`component/radiobox`](/examples/component/radiobox) |
| `Dropdown` | `auto dd = Dropdown(&entries, &selected);` | [`component/dropdown`](/examples/component/dropdown) |
| `Slider` | `auto sl = Slider("Volume:", &val, 0, 100, 5);` | [`component/slider`](/examples/component/slider) |
| `Collapsible` | `auto col = Collapsible("Details", child);` | [`component/collapsible`](/examples/component/collapsible) |
| `Window` | `auto win = Window({.inner = child, .title = "Title"});` | [`component/window`](/examples/component/window) |
| `ResizableSplit` | `ResizableSplitLeft(left, right, &split_size)` | [`component/resizable_split`](/examples/component/resizable_split) |

Chaque widget accepte également une structure `*Option` (par exemple `ButtonOption::Animated()`, `MenuOption::HorizontalAnimated()`, `InputOption`) pour personnaliser le style, les animations et les callbacks (`on_change`, `on_enter`, `transform`).

## Composer mise en page et événements (`Container` + `Renderer`)

Dans FTXUI, les **conteneurs** gèrent le routage clavier/souris et le focus, tandis que **`Renderer`** contrôle la mise en page visuelle :

```cpp
std::string username;
std::string password;

auto input_user = Input(&username, "username");
InputOption password_option;
password_option.password = true;
auto input_pass = Input(&password, "password", password_option);
auto submit = Button("Sign in", [&] { app.Exit(); });

auto form = Container::Vertical({
    input_user,
    input_pass,
    submit,
});

auto ui = Renderer(form, [&] {
  auto header = text("Login") | bold | hcenter;
  auto user_row = hbox({text("User: "), input_user->Render()});
  auto pass_row = hbox({text("Pass: "), input_pass->Render()});
  auto btn_row = submit->Render() | hcenter;

  return vbox({
             header,
             separator(),
             user_row,
             pass_row,
             separator(),
             btn_row,
         })
         | borderRounded;
});
```

### Types de conteneurs

- `Container::Vertical({...})` : les flèches Haut/Bas et Tab déplacent le focus verticalement.
- `Container::Horizontal({...})` : les flèches Gauche/Droite et Tab déplacent le focus horizontalement.
- `Container::Tab({...}, &active_tab)` : seul l'enfant actif est rendu et reçoit les événements.
- `Container::Stacked({...})` : rend tous les enfants dans l'ordre (pratique pour des panneaux `Window` flottants).

## Décorateurs de composants

Les composants se composent avec le même opérateur `|` que les éléments DOM :

- `CatchEvent(fn)` : intercepte les événements avant que l'enfant ne les voie (retourne `true` pour consommer, `false` pour laisser propager). Voir [Événements, focus et souris](/fr/guide/events-and-focus).
- `Maybe(&show)` : affiche ou masque conditionnellement un composant et désactive sa gestion d'événements.
- `Modal(dialog, &show)` : superpose `dialog` au-dessus de l'interface principale et bloque les événements en arrière-plan tant que `show` vaut `true`. Voir [`component/modal_dialog`](/examples/component/modal_dialog).
- `Hoverable(&is_hovered)` : indique si le curseur de la souris survole un composant.

```cpp
bool show_help = false;

auto ui = main_component
          | Modal(help_dialog, &show_help)
          | CatchEvent([&](Event event) {
              if (event == Event::Character('?')) {
                show_help = !show_help;
                return true;
              }
              return false;
            });
```

## Mettre à jour depuis des threads d'arrière-plan

`App` ne redessine que lorsqu'un événement arrive ou qu'une frame est demandée. Pour mettre à jour l'état de l'interface en toute sécurité depuis un thread de travail, utilisez `app.Post(...)` (exécuté sur le thread de la boucle principale) suivi de `app.RequestAnimationFrame()` :

```cpp
std::thread worker([&] {
  auto result = fetch_data();
  app.Post([&, result = std::move(result)] {
    data = result;
  });
  app.RequestAnimationFrame();
});
```

## Afficher au-dessus d'une application en ligne (`PrintAbove`)

En mode `App::TerminalOutput()`, l'interface interactive s'affiche en ligne sous la sortie précédente du shell. Utilisez `app.PrintAbove(element)` (thread-safe) pour ajouter des lignes permanentes de logs ou de chat au-dessus de l'invite active :

```cpp
auto app = App::TerminalOutput();
app.PrintAbove(paragraph("Build finished!") | color(Color::Green));
```

Voir [`component/print_above`](/examples/component/print_above).

## Boucles d'événements personnalisées (`Loop`)

Si vous souhaitez intégrer FTXUI dans une boucle de jeu ou un réacteur réseau existant, utilisez [`ftxui::Loop`](/api/class/loop) à la place de `app.Loop(...)` :

```cpp
#include <ftxui/component/loop.hpp>

auto app = App::Fullscreen();
Loop loop(&app, ui);

while (!loop.HasQuitted()) {
  update_simulation();
  app.RequestAnimationFrame();
  loop.RunOnce();
}
```

Voir [`component/custom_loop`](/examples/component/custom_loop).

## Restaurer les entrées/sorties du terminal (`WithRestoredIO`)

Pour suspendre temporairement le mode brut du terminal et lancer un programme externe (comme `$EDITOR`, `less` ou `git`), enveloppez l'appel avec `app.WithRestoredIO(...)` :

```cpp
auto edit_btn = Button("Open Editor", app.WithRestoredIO([&] {
  std::system("vim notes.txt");
}));
```

Voir [`component/with_restored_io`](/examples/component/with_restored_io).

## Prochaines étapes

- [Événements, focus et souris](/fr/guide/events-and-focus) : routage des événements, focus et détection souris
- [Widgets personnalisés](/fr/guide/custom-widgets) : dériver `ComponentBase` et `Node`
- [Référence API `ftxui/component`](/api/component) : liste complète des fonctions et options
