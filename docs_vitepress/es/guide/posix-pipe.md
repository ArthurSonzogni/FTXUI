# Entrada por tubería POSIX en FTXUI

::: warning Soporte de plataformas
Esta característica solo funciona en Linux y macOS. No está soportada en Windows ni en WebAssembly.
:::

## ¿Qué es una tubería POSIX?

Una tubería (pipe) POSIX permite que dos programas independientes se comuniquen. Un programa envía su salida directamente como entrada al otro programa, como si fuera un conducto unidireccional de datos.

**Ejemplo:**

Imagina que quieres listar archivos y luego filtrarlos de forma interactiva.

- `ls`: lista archivos.
- `interactive_grep`: una aplicación FTXUI que filtra texto y te permite escribir.

Puedes conectarlos con una tubería (`|`):

```bash
ls -l | interactive_grep
```

Esto es lo que ocurre:
1. `ls -l` lista los archivos con sus detalles.
2. El operador `|` envía ese listado directamente a `interactive_grep`.
3. `interactive_grep` recibe el listado y lo muestra. Al ser una aplicación FTXUI, puedes escribir para filtrar la lista aunque los datos iniciales procedan de `ls`.

## Cómo gestiona FTXUI la entrada por tubería

Ahora que sabes qué es una tubería POSIX, veamos cómo la maneja FTXUI.

FTXUI permite que tu aplicación lea datos de otros programas (como desde una tubería) sin perder la posibilidad de interactuar con el teclado. Esto resulta muy útil para herramientas interactivas de línea de comandos que procesan datos.

Normalmente, las aplicaciones FTXUI reciben toda la entrada desde `stdin`. Sin embargo, cuando FTXUI detecta que `stdin` está conectado a la salida de una tubería (es decir, se están redirigiendo datos hacia tu aplicación), cambia automáticamente para leer la entrada interactiva del teclado desde `/dev/tty`. Así tu aplicación puede seguir recibiendo la entrada del usuario mientras procesa los datos recibidos por la tubería.

Esta característica está **activada por defecto**.

Si tu aplicación FTXUI necesita leer datos por tubería y al mismo tiempo responder al teclado, normalmente no tienes que hacer nada especial:

```cpp
auto app = App::Fullscreen();
// app.HandlePipedInput(true); // Enabled by default
app.Loop(component);
```

## Desactivar la entrada por tubería

Si no necesitas esta característica o entra en conflicto con tu propia gestión de entrada, llama a `HandlePipedInput(false)` antes de iniciar el bucle principal:

```cpp
auto app = App::Fullscreen();
app.HandlePipedInput(false);
app.Loop(component);
```
