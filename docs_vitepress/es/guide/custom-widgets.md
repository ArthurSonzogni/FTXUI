# Widgets personalizados (`ComponentBase` y `Node`)

La mayoría de las aplicaciones FTXUI se pueden construir componiendo componentes integrados con `Renderer`, `Container` y `CatchEvent`. Cuando necesites un widget reutilizable y autocontenido o un dibujado de celdas a bajo nivel, puedes heredar directamente de `ComponentBase` o `Node`.

## Componente personalizado (`ComponentBase`)

Hereda de [`ftxui::ComponentBase`](/api/class/componentbase) cuando quieras un widget interactivo con estado que tenga su propio renderizado, manejo de eventos y comportamiento de foco.

### Métodos virtuales principales

| Método | Propósito |
| --- | --- |
| `Element OnRender()` | Devuelve el árbol de `Element` para este fotograma (se llama en cada redibujado). |
| `bool OnEvent(Event event)` | Gestiona la entrada de teclado y ratón. Devuelve `true` si se consume o `false` para dejar que los padres lo gestionen. |
| `bool Focusable() const` | Devuelve `true` si este widget puede recibir foco de teclado mediante `Tab` o las flechas. |
| `void OnAnimation(animation::Params& params)` | Avanza las animaciones; llama a `animation::RequestAnimationFrame()` mientras la animación siga activa. |

### Ejemplo: un widget contador reutilizable

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

// Factory function matching FTXUI's convention:
Component Counter(int* value) {
  return Make<CounterWidget>(value);
}
```

::: tip `Make<T>(...)`
Construye siempre los componentes con `ftxui::Make<MyWidget>(args...)`, que devuelve un `Component` (`std::shared_ptr<ComponentBase>`).
:::

### Componer componentes hijos dentro de un componente personalizado

Si tu componente personalizado posee componentes hijos (por ejemplo, un `Input` y un `Button`), regístralos con `Add(child)` en el constructor para que `ComponentBase` configure los punteros al padre y el reenvío predeterminado de eventos y foco:

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

## Elemento DOM personalizado (`Node`)

Hereda de [`ftxui::Node`](/api/class/node) cuando necesites renderizar celda por celda directamente sobre un `Screen` (por ejemplo, una gráfica personalizada, un visor hexadecimal o un shader de píxeles).

### Las tres pasadas de diseño

En cada fotograma, [`ftxui::Render(screen, root)`](/api/dom#render) ejecuta tres pasadas sobre el árbol de `Node`:

1. `ComputeRequirement()` (de abajo arriba): establece `requirement_.min_x`, `requirement_.min_y`, `requirement_.flex_grow_x`, `requirement_.flex_grow_y`, etc.
2. `SetBox(Box box)` (de arriba abajo): recibe el rectángulo delimitador asignado (`box_`) y asigna subrectángulos a los hijos si los hay.
3. `Render(Screen& screen)`: dibuja directamente en `screen.CellAt(x, y)` dentro de `box_`.

### Ejemplo: un nodo tablero de ajedrez personalizado

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

::: tip Rendimiento: `screen.stencil` y `FastCellAt`
`screen.CellAt(x, y)` ya comprueba los límites y el recorte de stencil (las escrituras fuera de `screen.stencil` se ignoran de forma segura). Para nodos muy grandes dentro de un `frame`, puedes evitar trabajo fuera de pantalla iterando sobre `Box::Intersection(box_, screen.stencil)` y llamando a `screen.FastCellAt(x, y)`.
:::
