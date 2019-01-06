# FTXUI

A C++ library for making text based user interface.

## Feature
 * Functional style.
 * Simple and elegant syntax (in my opinion).
 * No dependencies.

## Example:
~~~cpp
  vbox(
    hbox(
      text(L"left") | frame,
      text(L"middle") | frame | flex,
      text(L"right") | frame
    ),
    gauge(0.5) | frame
  )
~~~

~~~bash
┌────┐┌───────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                         ││right│
└────┘└───────────────────────────────────────────────────────────────┘└─────┘
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
~~~

## Tutorial
See [Tutorial](./tutorial.md)

