# Widgets personnalisés (`ComponentBase` et `Node`)

La plupart des applications FTXUI se construisent en composant les composants intégrés avec `Renderer`, `Container` et `CatchEvent`. Lorsque vous avez besoin d'un widget autonome réutilisable ou d'un dessin bas niveau cellule par cellule, vous pouvez dériver directement de `ComponentBase` ou de `Node`.

## Composant personnalisé (`ComponentBase`)

Dérivez de [`ftxui::ComponentBase`](/api/class/componentbase) lorsque vous voulez un widget interactif avec état, disposant de son propre rendu, de sa gestion d'événements et de son comportement de focus.

### Méthodes virtuelles clés

| Méthode | Rôle |
| --- | --- |
| `Element OnRender()` | Retourne l'arbre d'`Element` pour cette frame (appelée à chaque rendu). |
| `bool OnEvent(Event event)` | Gère les entrées clavier/souris. Retourne `true` si consommé, `false` pour laisser les parents le traiter. |
| `bool Focusable() const` | Retourne `true` si ce widget peut recevoir le focus clavier via `Tab` ou les flèches. |
| `void OnAnimation(animation::Params& params)` | Fait avancer les animations ; appelez `animation::RequestAnimationFrame()` tant que l'animation est en cours. |

### Exemple : un widget compteur réutilisable

```cpp
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>

using namespace ftxui;

class CounterWidget : public ComponentBase {
 public:
  explicit CounterWidget(int* value) : value_(value) {}

  Element OnRender() override {
    auto label = text(" Count: " + std::to_string(*value_) + " ");
    auto hint = text(" (+/- or click) ") | dim;
    auto body = hbox({label, hint});

    if (Focused()) {
      body = body | inverted;
    }

    return body
           | borderRounded
           | reflect(box_);
  }

  bool OnEvent(Event event) override {
    if (event == Event::Character('+') || event == Event::ArrowRight) {
      (*value_)++;
      return true;
    }
    if (event == Event::Character('-') || event == Event::ArrowLeft) {
      (*value_)--;
      return true;
    }
    if (event.is_mouse() && box_.Contain(event.mouse().x, event.mouse().y)) {
      if (event.mouse().button == Mouse::Left &&
          event.mouse().motion == Mouse::Pressed) {
        TakeFocus();
        (*value_)++;
        return true;
      }
      if (event.mouse().button == Mouse::WheelUp) {
        (*value_)++;
        return true;
      }
      if (event.mouse().button == Mouse::WheelDown) {
        (*value_)--;
        return true;
      }
    }
    return false;
  }

  bool Focusable() const override { return true; }

 private:
  int* value_;
  Box box_;
};

// Fonction fabrique respectant la convention de FTXUI :
Component Counter(int* value) {
  return Make<CounterWidget>(value);
}
```

::: tip `Make<T>(...)`
Construisez toujours vos composants avec `ftxui::Make<MyWidget>(args...)`, qui retourne un `Component` (`std::shared_ptr<ComponentBase>`).
:::

### Composer des composants enfants dans un composant personnalisé

Si votre composant personnalisé possède des composants enfants (par exemple un `Input` et un `Button`), enregistrez-les avec `Add(child)` dans le constructeur afin que `ComponentBase` relie les pointeurs parent et le transfert par défaut des événements et du focus :

```cpp
class SearchBar : public ComponentBase {
 public:
  SearchBar(std::string* query, std::function<void()> on_submit) {
    input_ = Input(query, "Search...");
    submit_ = Button("Go", std::move(on_submit));
    Add(Container::Horizontal({input_, submit_}));
  }

  Element OnRender() override {
    auto input_el = input_->Render() | flex;
    auto submit_el = submit_->Render();
    return hbox({input_el, submit_el}) | border;
  }

 private:
  Component input_;
  Component submit_;
};
```

## Élément DOM personnalisé (`Node`)

Dérivez de [`ftxui::Node`](/api/class/node) lorsque vous avez besoin d'un rendu direct cellule par cellule dans un `Screen` (par exemple un graphique personnalisé, un éditeur hexadécimal ou un shader de pixels).

### Les trois passes de mise en page

À chaque frame, [`ftxui::Render(screen, root)`](/api/dom#render) exécute trois passes sur l'arbre de `Node` :

1. `ComputeRequirement()` (de bas en haut) : renseigne `requirement_.min_x`, `requirement_.min_y`, `requirement_.flex_grow_x`, `requirement_.flex_grow_y`, etc.
2. `SetBox(Box box)` (de haut en bas) : reçoit la boîte englobante allouée (`box_`) et assigne les sous-boîtes aux éventuels enfants.
3. `Render(Screen& screen)` : dessine directement dans `screen.CellAt(x, y)` à l'intérieur de `box_`.

### Exemple : un nœud damier personnalisé

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <memory>

using namespace ftxui;

class CheckerboardNode : public Node {
 public:
  CheckerboardNode(Color a, Color b) : a_(a), b_(b) {}

  void ComputeRequirement() override {
    requirement_.min_x = 4;
    requirement_.min_y = 2;
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 1;
  }

  void Render(Screen& screen) override {
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        auto& cell = screen.CellAt(x, y);
        cell.character = " ";
        cell.background_color = ((x + y) % 2 == 0) ? a_ : b_;
      }
    }
  }

 private:
  Color a_;
  Color b_;
};

Element checkerboard(Color a, Color b) {
  return std::make_shared<CheckerboardNode>(a, b);
}
```

::: tip Performance : `screen.stencil` et `FastCellAt`
`screen.CellAt(x, y)` vérifie déjà les bornes et le pochoir (les écritures hors de `screen.stencil` sont ignorées sans danger). Pour de très grands nœuds dans un `frame`, vous pouvez éviter le travail hors écran en itérant sur `Box::Intersection(box_, screen.stencil)` et en appelant `screen.FastCellAt(x, y)`.
:::
