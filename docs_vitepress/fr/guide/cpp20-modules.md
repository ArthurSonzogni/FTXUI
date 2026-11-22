# Modules C++20

::: warning Expérimental
Cette fonctionnalité est encore en développement et son API peut évoluer dans de futures versions. Si vous rencontrez un problème ou avez des suggestions, n'hésitez pas à ouvrir une issue sur GitHub.
:::

FTXUI prend en charge de manière expérimentale les [modules C++20](https://en.cppreference.com/w/cpp/language/modules) afin de réduire les temps de compilation et d'améliorer l'organisation du code. Chaque partie de la bibliothèque dispose d'un module correspondant, découpé en partitions par en-tête.

Utilisez l'option `FTXUI_BUILD_MODULES` lors de la compilation de FTXUI pour exposer les modules C++20, par exemple avec CMake et Ninja :

```sh
cmake \
    -DCMAKE_GENERATOR=Ninja \
    -DFTXUI_BUILD_MODULES=ON \
    ..

ninja
```

::: info Note
Pour utiliser les modules, il vous faut un compilateur C++20, CMake 3.20 ou plus récent, et un générateur compatible tel que Ninja. Les générateurs Makefile ne prennent pas en charge les modules C++20.
:::

Dans votre propre code, vous pouvez ensuite importer les modules normalement :

```cpp
import ftxui;

using ftxui::App;
using ftxui::Button;
using ftxui::Component;

int main() {
  App app = App::TerminalOutput();
  Component button = Button("Click me", app.ExitLoopClosure());
  app.Loop(button);
  return 0;
}
```

Écrire `import ftxui;` revient à inclure tous les en-têtes `<ftxui/**/*.hpp>` et rend l'ensemble de la bibliothèque disponible via un seul module.

Pour lier correctement les modules avec CMake, utilisez `target_link_libraries` afin de récupérer les bons drapeaux de compilation et d'édition de liens :

```cmake
target_link_libraries(my_executable
    #...autres cibles...
    PRIVATE ftxui::modules
)
```

### Liste des modules

Bien que `import ftxui;` expose toute la bibliothèque, FTXUI est conçu par couches. Si vous n'avez besoin que de certaines fonctionnalités, vous pouvez importer directement les modules indépendants :

- `ftxui` (module de confort qui réexporte tout ce qui suit)
    - `ftxui.component` (composants interactifs, événements et boucles d'événements)
    - `ftxui.dom` (mise en page et styles via les Elements)
    - `ftxui.screen` (rendu terminal, cellules et couleurs)
    - `ftxui.util` (utilitaires internes)

Par exemple :
```cpp
import ftxui.screen;
import ftxui.dom;

// Utiliser uniquement les fonctionnalités screen et dom...
```
