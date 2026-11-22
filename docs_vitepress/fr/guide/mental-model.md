# Modèle mental

FTXUI repose sur quelques idées fondamentales. Les comprendre dès le départ rend l'API beaucoup plus simple à utiliser.

## 1. Trois modules en couches

FTXUI est découpé en trois modules qui s'empilent les uns sur les autres (`screen → dom → component`) :

- **`ftxui/screen`** ([guide](/fr/guide/screen), [API](/api/screen)) : grille 2D bas niveau de cellules stylisées (`Screen`, `Surface`, `Cell`, `Color`, `Box`).
- **`ftxui/dom`** ([guide](/fr/guide/dom), [API](/api/dom)) : arbre de mise en page sans état (`Element`, `hbox`, `vbox`, `border`, `table`, `canvas`, `Render()`).
- **`ftxui/component`** ([guide](/fr/guide/component), [API](/api/component)) : widgets interactifs et boucle d'événements (`Component`, `Button`, `Input`, `Menu`, `Container`, `App`).

Vous pouvez aussi utiliser la cible parapluie (`ftxui::ftxui` dans CMake, `@ftxui//:ftxui` dans Bazel, `#include <ftxui/ftxui.hpp>`) pour inclure les trois d'un coup.

## 2. Elements vs Components

| | **Element** (`ftxui/dom`) | **Component** (`ftxui/component`) |
| --- | --- | --- |
| **Durée de vie** | Éphémère, reconstruit à chaque frame | Persistant, créé une seule fois avant `app.Loop(...)` |
| **État** | Arbre de valeurs sans état | Conserve l'état du focus et des références vers votre état C++ |
| **Entrées** | Ne reçoit pas d'événements | Gère les `Event` clavier et souris via `OnEvent` |
| **Exemples** | `text`, `hbox`, `vbox`, `border`, `gauge`, `table`, `canvas` | `Button`, `Input`, `Menu`, `Slider`, `Modal`, `Window`, `Container` |

Votre interface est une fonction de votre état C++. Au lieu de modifier le libellé d'un widget en place, vous mettez à jour des variables C++ classiques (`std::string`, `int`, `bool`, `std::vector`) et laissez `Render()` reconstruire un nouvel arbre d'`Element` à la frame suivante.

## 3. Arbre d'événements vs arbre de mise en page visuelle

Dans FTXUI, le **routage des événements et du focus** est volontairement séparé de la **mise en page visuelle** :

- Les **Containers** (`Container::Vertical`, `Container::Horizontal`, `Container::Tab`, `Container::Stacked`) définissent l'**arbre d'événements et de focus** (quel enfant reçoit les entrées clavier, et comment les flèches ou `Tab` déplacent le focus).
- **`Renderer(container, [&] { ... })`** définit la **mise en page visuelle** tout en transférant tous les événements à `container`.

::: danger Piège courant
Appeler `child->Render()` dans `Renderer([&] { ... })` **sans** passer l'enfant (ou un conteneur regroupant tous les enfants) en premier argument signifie que les événements clavier et souris n'atteindront **jamais** ces enfants !
:::

```cpp
// FAUX : `input` et `button` sont dessinés, mais ne reçoivent jamais d'événements !
auto ui = Renderer([&] {
  return vbox({input->Render(), button->Render()});
});

// CORRECT : `container` route les événements et le focus ; `Renderer` contrôle la mise en page.
auto container = Container::Vertical({input, button});
auto ui = Renderer(container, [&] {
  return vbox({
      text("Login") | bold | hcenter,
      separator(),
      input->Render(),
      button->Render(),
  }) | border;
});
```

Notez que vous n'êtes pas obligé d'appeler `container->Render()` dans la lambda. Appeler `->Render()` sur chaque enfant permet de les agencer librement dans n'importe quelle mise en page (`vbox`, `hbox`, `window`, `gridbox`, etc.).

## 4. Cycle de vie d'une frame

```text
  Entrée terminal / App::Post / RequestAnimationFrame
                     │
                     ▼
        root_component->OnEvent(event)
        (modifie votre état C++ classique)
                     │
                     ▼
        Element tree = root_component->Render()
        (nouvel arbre DOM sans état issu de l'état)
                     │
                     ▼
        Render(screen, tree) → diff → terminal
```

- FTXUI ne redessine l'écran que lorsqu'un événement arrive ou qu'une frame d'animation est demandée.
- La sortie écran est comparée (**diff**) avec la frame précédente et encadrée par des mises à jour synchronisées (DEC mode 2026) pour éviter le scintillement et le tearing.

## 5. Choisir un mode d'`App`

| Fabrique | Comportement | Idéal pour |
| --- | --- | --- |
| `App::Fullscreen()` | Utilise le tampon d'écran alternatif (taille complète du terminal, restauré à la sortie). | TUI plein écran (tableaux de bord, éditeurs, gestionnaires de fichiers) |
| `App::TerminalOutput()` | Dessine en ligne sous la sortie précédente du shell ; supporte `PrintAbove()`. | CLI interactives, invites, outils de progression ou de logs |
| `App::FitComponent()` | S'adapte à la largeur et à la hauteur minimales du composant. | Widgets intégrés compacts et invites rapides |
| `App::FixedSize(w, h)` | Largeur et hauteur fixes. | Tests et affichages à dimensions fixes |
