# 
* Level 0: terminal output.
* Level 1: ftxui::Screen               
* Level 2: ftxui::Node
* Level 3: ftxui::Component

## Level 0: terminal output.
  The terminal you know, you can append text on it. It is represented by
  std::cout.

## Level 1: ftxui::Screen               
  A rectangular grid of characters.
  Use Terminal::ToString() to append its content into the console.

## Level 2: ftxui::Node
  A hierarchical set of element.
  They handle layout and Render themself on the screen.
  See ftxui/dom/elements.hpp

  You can make implement your own.

## Level 3: ftxui::Component
  A hierarchical set of component. A component render itself by producing
  ftxui::Node in Component::Render().

  Some component can handle events:
    * keyboard
    * mouse
    * terminal event

  Implement your own!
