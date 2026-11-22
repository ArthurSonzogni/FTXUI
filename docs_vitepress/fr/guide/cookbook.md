# Recettes pratiques

Motifs courants pour construire des applications avec FTXUI.

## 1. Threads d'arrière-plan et mises à jour en direct

Ne modifiez jamais directement l'état de l'interface depuis un thread d'arrière-plan pendant que `App::Loop()` effectue le rendu. Utilisez plutôt `App::Post()` (thread-safe) pour exécuter les mises à jour sur le thread de la boucle principale, et `App::RequestAnimationFrame()` pour planifier un nouveau rendu :

```cpp
#include <atomic>
#include <chrono>
#include <thread>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
  using namespace ftxui;

  auto app = App::Fullscreen();
  int progress = 0;

  auto ui = Renderer([&] {
    auto label = text("Downloading: " + std::to_string(progress) + "%");
    auto bar = gauge(progress / 100.f);
    return vbox({label, bar})
           | border
           | center;
  });

  std::atomic<bool> running = true;
  std::thread worker([&] {
    while (running && progress < 100) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      app.Post([&] { progress++; });
      app.RequestAnimationFrame();
    }
  });

  app.Loop(ui);
  running = false;
  worker.join();
}
```

## 2. Listes dynamiques de composants

Lorsqu'un `std::vector` d'éléments grandit ou rétrécit, reconstruisez les enfants du conteneur avec `DetachAllChildren()` et `Add(...)` (toujours sur le thread de la boucle principale, par exemple dans un callback de bouton ou via `app.Post`) :

```cpp
std::vector<std::string> items = {"Item 1", "Item 2"};
auto list_container = Container::Vertical({});

auto rebuild_list = [&] {
  list_container->DetachAllChildren();
  for (size_t i = 0; i < items.size(); ++i) {
    auto item_button = Button(items[i], [&, i] {
      items.erase(items.begin() + static_cast<ptrdiff_t>(i));
      // Différer la reconstruction pour ne pas détruire le bouton pendant son callback.
      app.Post(rebuild_list);
    });
    list_container->Add(item_button);
  }
};
rebuild_list();
```

## 3. Panneau défilant avec la molette de la souris

`vscroll_indicator` est un indicateur visuel passif. Pour rendre un panneau défilant à la molette et au clavier, associez `yframe` + `vscroll_indicator` à `focusPositionRelative` et `CatchEvent` :

```cpp
float scroll_y = 0.0f;

auto pane = Renderer([&] {
  Elements lines;
  for (int i = 0; i < 100; ++i) {
    lines.push_back(text("Line " + std::to_string(i)));
  }
  return vbox(std::move(lines))
         | focusPositionRelative(0.f, scroll_y)
         | vscroll_indicator
         | yframe
         | border;
});

pane |= CatchEvent([&](Event event) {
  if (event.is_mouse()) {
    if (event.mouse().button == Mouse::WheelDown) {
      scroll_y = std::min(1.0f, scroll_y + 0.05f);
      return true;
    }
    if (event.mouse().button == Mouse::WheelUp) {
      scroll_y = std::max(0.0f, scroll_y - 0.05f);
      return true;
    }
  }
  if (event == Event::ArrowDown) {
    scroll_y = std::min(1.0f, scroll_y + 0.05f);
    return true;
  }
  if (event == Event::ArrowUp) {
    scroll_y = std::max(0.0f, scroll_y - 0.05f);
    return true;
  }
  return false;
});
```

## 4. Raccourcis globaux et `Ctrl+C` personnalisé

Attachez `CatchEvent` au composant racine pour les raccourcis globaux. Retournez `true` pour consommer un événement, ou `false` pour le laisser se propager aux enfants :

```cpp
auto app = App::Fullscreen();
app.ForceHandleCtrlC(false);  // Permet à votre composant d'intercepter Ctrl+C.

auto root = main_ui
            | CatchEvent([&](Event event) {
                if (event == Event::Character('q') || event == Event::CtrlC) {
                  app.Exit();
                  return true;
                }
                return false;
              });
```

## 5. Modales et boîtes de confirmation

Enveloppez votre composant principal avec `Modal(dialog, &show_modal)`. Ajoutez `CatchEvent` sur la boîte de dialogue pour la fermer avec `Escape` :

```cpp
bool show_modal = false;

auto main_ui = Button("Delete item", [&] { show_modal = true; })
               | center;

auto confirm_button = Button("Confirm", [&] { show_modal = false; });
auto cancel_button = Button("Cancel", [&] { show_modal = false; });
auto modal_buttons = Container::Horizontal({
    confirm_button,
    cancel_button,
});

auto modal_layout = Renderer(modal_buttons, [&] {
  return vbox({
             text("Are you sure?") | bold,
             separator(),
             modal_buttons->Render(),
         })
         | border
         | clear_under;
});

auto modal = modal_layout
             | CatchEvent([&](Event e) {
                 if (e == Event::Escape) {
                   show_modal = false;
                   return true;
                 }
                 return false;
               });

auto root = main_ui
            | Modal(modal, &show_modal);
```

## 6. Détection de clic souris et coordonnées relatives à un élément

Capturez la boîte englobante `Box` d'un élément lors du rendu avec `| reflect(box)`, puis testez les clics dans `CatchEvent` :

```cpp
Box canvas_box;
int last_x = -1;
int last_y = -1;

auto ui = Renderer([&] {
  auto label = text("Click inside me (" + std::to_string(last_x) + ", " +
                    std::to_string(last_y) + ")");
  return label
         | border
         | reflect(canvas_box);
});

ui |= CatchEvent([&](Event event) {
  if (event.is_mouse() &&
      event.mouse().button == Mouse::Left &&
      event.mouse().motion == Mouse::Pressed &&
      canvas_box.Contain(event.mouse().x, event.mouse().y)) {
    last_x = event.mouse().x - canvas_box.x_min;
    last_y = event.mouse().y - canvas_box.y_min;
    return true;
  }
  return false;
});
```

## 7. Sortie CLI en continu et lancement d'éditeurs externes

Utilisez `App::TerminalOutput()` pour les CLI en ligne, `PrintAbove()` pour émettre des lignes permanentes dans l'historique du terminal, et `WithRestoredIO()` pour rendre temporairement la main à un programme externe :

```cpp
auto app = App::TerminalOutput();

auto log_button = Button("Log line", [&] {
  app.PrintAbove(text("[info] Task completed") | color(Color::Green));
});
auto editor_button = Button("Open editor", app.WithRestoredIO([&] {
  std::system("vim notes.txt");
}));
auto quit_button = Button("Quit", app.ExitLoopClosure());

auto ui = Container::Horizontal({
    log_button,
    editor_button,
    quit_button,
});

app.Loop(ui);
```
