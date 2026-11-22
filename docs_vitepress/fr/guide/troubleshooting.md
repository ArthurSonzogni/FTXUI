# Dépannage et FAQ

Solutions aux problèmes les plus fréquents de compilation, d'exécution et de migration.

## Les événements n'atteignent pas mes composants enfants

**Symptôme :** les boutons, champs de saisie ou menus s'affichent à l'écran, mais le clavier et la souris ne déclenchent rien.

**Cause :** vous avez utilisé `Renderer([&] { return child->Render(); })` sans passer `child` (ou un `Container` contenant vos enfants) en premier argument.

**Solution :** associez toujours un `Container` à `Renderer(container, ...)` :
```cpp
auto container = Container::Vertical({input, button});
auto ui = Renderer(container, [&] {
  return vbox({input->Render(), button->Render()})
         | border;
});
```

Consultez le guide [Modèle mental](/fr/guide/mental-model) pour plus de détails.

---

## Références indéfinies lors de l'édition de liens manuelle

**Symptôme :** erreurs de l'éditeur de liens mentionnant [`ftxui::Node`](/api/class/node), [`ftxui::Screen`](/api/class/screen) ou [`ftxui::Element`](/api/component#element).

**Solution :** avec les bibliothèques statiques, l'ordre de liaison compte (`component` dépend de `dom`, qui dépend de `screen`) :
```bash
g++ main.cpp -lftxui-component -lftxui-dom -lftxui-screen
```
Avec CMake, liez simplement `ftxui::ftxui` (ou `ftxui::component`, qui tire `dom` et `screen` par transitivité).

---

## Compilation croisée MinGW sous Linux (`'mutex' is not a member of 'std'`)

**Symptôme :** la compilation avec `x86_64-w64-mingw32-g++` sous Debian/Ubuntu échoue avec `error: 'mutex' is not a member of 'std'`.

**Solution :** le compilateur MinGW par défaut sous Debian/Ubuntu utilise le modèle de threads `win32`, qui n'implémente pas `<mutex>` et `<thread>` de C++11. Basculez vers la variante `posix` :
- Dans votre fichier toolchain CMake, définissez `CMAKE_CXX_COMPILER` à `x86_64-w64-mingw32-g++-posix`, ou
- Exécutez `sudo update-alternatives --config x86_64-w64-mingw32-g++` et sélectionnez `x86_64-w64-mingw32-g++-posix`.

---

## Collisions de macros Windows (`RGB`, `DrawText`, `min`, `max`)

**Symptôme :** erreurs de compilation étranges dans `<ftxui/screen/color.hpp>` ou `<ftxui/dom/canvas.hpp>` après avoir inclus `<windows.h>`.

**Solution :** définissez `NOMINMAX` et `WIN32_LEAN_AND_MEAN` avant d'inclure `<windows.h>` (ou passez `-DNOMINMAX` dans CMake). FTXUI annule déjà les macros `RGB` et `DrawText` dans ses en-têtes, mais définir `NOMINMAX` évite les conflits avec `std::min`/`std::max`.

---

## Couleurs, Unicode ou souris inopérants dans un terminal

- **Forcer la détection des couleurs :**
  ```cpp
  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  ```
- **Désactiver les caractères de blocs Unicode / utiliser le mode de secours ASCII :**
  ```cpp
  auto quirks = Terminal::GetQuirks();
  quirks.SetComponentAscii(true);
  Terminal::SetQuirks(quirks);
  ```
- **Windows 7 / anciennes consoles :** l'ancienne console Windows `conhost` (avant Windows 10) ne prend pas en charge les séquences d'échappement Virtual Terminal (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`). Utilisez Windows Terminal ou un terminal moderne compatible VT100.

---

## Migrer de FTXUI v6 vers v7

FTXUI v7 a renommé trois classes fondamentales pour plus de clarté (des alias dépréciés restent disponibles pour la rétrocompatibilité) :

| FTXUI v6 (déprécié) | FTXUI v7 | En-tête |
| --- | --- | --- |
| `ScreenInteractive` | `App` | `<ftxui/component/app.hpp>` |
| `Pixel` / `PixelAt()` | `Cell` / `CellAt()` | `<ftxui/screen/cell.hpp>` |
| `Image` / `DrawImage()` | `Surface` / `DrawSurface()` | `<ftxui/screen/surface.hpp>` |
