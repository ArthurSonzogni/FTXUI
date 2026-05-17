// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef FTXUI_UTIL_EXPORT_H_
#define FTXUI_UTIL_EXPORT_H_

// In an amalgamated build, we don't want any export/import annotations.
#if defined(FTXUI_AMALGAMATED)
#define FTXUI_EXPORT(component)
#define INSIDE_FTXUI_COMPONENT_IMPL(component) 0
#else

// Used to annotate symbols which are exported by the component named
// |component|. Note that this only does the right thing if the corresponding
// component target's sources are compiled with |IS_FTXUI_$component_IMPL|
// defined as 1. For example:
//
//   class FTXUI_EXPORT(FOO) Bar {};
//
// If IS_FTXUI_FOO_IMPL=1 at compile time, then Bar will be annotated using the
// FTXUI_EXPORT_ANNOTATION macro defined below. Otherwise it will be
// annotated using the FTXUI_IMPORT_ANNOTATION macro.
#define FTXUI_EXPORT(component)                         \
  FTXUI_MACRO_CONDITIONAL_(IS_FTXUI_##component##_IMPL, \
                           FTXUI_EXPORT_ANNOTATION, FTXUI_IMPORT_ANNOTATION)

// Indicates whether the current compilation unit is being compiled as part of
// the implementation of the component named |component|. Expands to |1| if
// |IS_FTXUI_$component_IMPL| is defined as |1|; expands to |0| otherwise.
//
// Note in particular that if |IS_FTXUI_$component_IMPL| is not defined at all,
// it is still fine to test INSIDE_FTXUI_COMPONENT_IMPL(component), which
// expands to |0| as expected.
#define INSIDE_FTXUI_COMPONENT_IMPL(component) \
  FTXUI_MACRO_CONDITIONAL_(IS_FTXUI_##component##_IMPL, 1, 0)

#endif

// Compiler-specific macros to annotate for export or import of a symbol. No-op
// in non-component builds. These should not see much if any direct use.
// Instead use the FTXUI_EXPORT macro defined above.
#if defined(COMPONENT_BUILD)
#if defined(WIN32)
#define FTXUI_EXPORT_ANNOTATION __declspec(dllexport)
#define FTXUI_IMPORT_ANNOTATION __declspec(dllimport)
#else  // defined(WIN32)
#define FTXUI_EXPORT_ANNOTATION __attribute__((visibility("default")))
#define FTXUI_IMPORT_ANNOTATION __attribute__((visibility("default")))
#endif  // defined(WIN32)
#else   // defined(COMPONENT_BUILD)
#define FTXUI_EXPORT_ANNOTATION
#define FTXUI_IMPORT_ANNOTATION
#endif  // defined(COMPONENT_BUILD)

// Below this point are several internal utility macros used for the
// implementation of the above macros. Not intended for external use.

#define FTXUI_MACRO_EXPAND(x) x

// Helper for conditional expansion to one of two token strings. If |condition|
// expands to |1| then this macro expands to |consequent|; otherwise it expands
// to |alternate|.
#define FTXUI_MACRO_CONDITIONAL_(condition, consequent, alternate) \
  FTXUI_MACRO_EXPAND(FTXUI_MACRO_SELECT_THIRD_ARGUMENT_(           \
      FTXUI_MACRO_CONDITIONAL_COMMA_(condition), consequent, alternate))

// Expands to a comma (,) iff its first argument expands to |1|. Used in
// conjunction with |FTXUI_MACRO_SELECT_THIRD_ARGUMENT_()|, as the presence
// or absense of an extra comma can be used to conditionally shift subsequent
// argument positions and thus influence which argument is selected.
#define FTXUI_MACRO_CONDITIONAL_COMMA_(...) \
  FTXUI_MACRO_EXPAND(FTXUI_MACRO_CONDITIONAL_COMMA_IMPL_(__VA_ARGS__, dummy))
#define FTXUI_MACRO_CONDITIONAL_COMMA_IMPL_(x, ...) \
  FTXUI_MACRO_CONDITIONAL_COMMA_##x##_
#define FTXUI_MACRO_CONDITIONAL_COMMA_1_ ,

// Helper which simply selects its third argument. Used in conjunction with
// |FTXUI_MACRO_CONDITIONAL_COMMA_()| above to implement conditional macro
// expansion.
#define FTXUI_MACRO_SELECT_THIRD_ARGUMENT_(...) \
  FTXUI_MACRO_EXPAND(                               \
      FTXUI_MACRO_SELECT_THIRD_ARGUMENT_IMPL_(__VA_ARGS__, dummy))
#define FTXUI_MACRO_SELECT_THIRD_ARGUMENT_IMPL_(a, b, c, ...) c

#endif  // FTXUI_UTIL_EXPORT_H_
