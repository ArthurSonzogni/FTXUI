# Level of abstractions:
* Level 0: terminal output.
* Level 1: ftxui::Screen               
* Level 2: ftxui::Node
* Level 3: ftxui::Component

## Level 0: terminal output.
  This is std::cout. You can send text to it.

## Level 1: ftxui::Screen               
  A rectangular grid of characters.
  Use Terminal::ToString() to append its content to the console (level 0).

## Level 2: ftxui::Node
  A hierarchial set of element.
  They handle layout and render themself on the screen (level 1)
  See ftxui/dom/elements.hpp

  You can make implement your own.

## Level 3: ftxui::Component
  A hierarchical set of components.
  Components store some state and react to events:
    * keyboard
    * mouse
    * terminal event
  Components renders themself by producing ftxui::Node (Level 2)

  Implement your own!
