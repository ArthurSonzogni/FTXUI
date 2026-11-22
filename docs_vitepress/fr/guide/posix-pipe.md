# Entrée redirigée par pipe POSIX dans FTXUI

::: warning Plateformes prises en charge
Cette fonctionnalité fonctionne uniquement sur Linux et macOS. Elle n'est pas prise en charge sur Windows ni sur WebAssembly.
:::

## Qu'est-ce qu'un pipe POSIX ?

Un pipe POSIX permet à deux programmes distincts de communiquer : le premier envoie directement sa sortie en entrée du second, comme un conduit unidirectionnel de données.

**Exemple :**

Imaginez que vous vouliez lister des fichiers puis les filtrer de manière interactive.

- `ls` : liste les fichiers.
- `interactive_grep` : une application FTXUI qui filtre du texte tout en vous laissant saisir au clavier.

Vous pouvez les relier avec un pipe (`|`) :

```bash
ls -l | interactive_grep
```

Voici ce qui se passe :
1. `ls -l` liste les fichiers avec leurs détails.
2. Le `|` envoie directement cette liste à `interactive_grep`.
3. `interactive_grep` reçoit la liste et l'affiche. Comme il s'agit d'une application FTXUI, vous pouvez taper au clavier pour filtrer la liste, même si les données initiales proviennent de `ls`.

## Comment FTXUI gère l'entrée redirigée par pipe

Voyons maintenant comment FTXUI tire parti des pipes POSIX.

FTXUI permet à votre application de lire des données provenant d'un autre programme (par exemple via un pipe) tout en continuant à utiliser le clavier pour interagir. C'est très pratique pour les outils en ligne de commande interactifs qui traitent des flux de données.

En temps normal, une application FTXUI lit toutes ses entrées depuis `stdin`. Toutefois, lorsque FTXUI détecte que `stdin` est connecté à la sortie d'un pipe (autrement dit, que des données arrivent par redirection), il bascule automatiquement la lecture des entrées clavier interactives vers `/dev/tty`. Votre application peut ainsi continuer à recevoir les frappes utilisateur tout en lisant les données du pipe.

Cette fonctionnalité est **activée par défaut**.

Si votre application FTXUI doit à la fois lire des données depuis un pipe et réagir au clavier, vous n'avez généralement rien de particulier à faire :

```cpp
auto app = App::Fullscreen();
// app.HandlePipedInput(true); // Activé par défaut
app.Loop(component);
```

## Désactiver la gestion des entrées par pipe

Si vous n'avez pas besoin de cette fonctionnalité, ou si elle entre en conflit avec votre propre gestion des entrées, appelez `HandlePipedInput(false)` avant de démarrer la boucle principale :

```cpp
auto app = App::Fullscreen();
app.HandlePipedInput(false);
app.Loop(component);
```
