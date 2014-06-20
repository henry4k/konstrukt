Brainstorming
=============

## Sounds

Minimaler Abstand zu Objekten -> Predelay (0-N ms) (Warscheinlich konstant, weil die Decke/der Boden immer am nächsten sind)
Raumgröße -> Reverb Time (0.1-N s)
Gasdruck -> Lautstärke (100-0%)

Reverb Damping:
> The more less-reflective surfaces the sound bounces around from, the more the reverb is dampened the higher frequencies.


## Grafik

Normal-Maps

Glow-Maps:
Für LEDs und Lampen sicherlich ein toller Effekt.
Darf man natürlich nicht übertreiben.


## Tiles

- Tile und Structure separieren
  Man sollte zwischen Tiles und Strukturen unterscheiden.
  Denn Tiles bezeichnen immer einzelne Gitterzellen.
  Und Strukturen können sich über mehrere Tiles ausdehnen.

- Referenz-Tiles korrekt updaten
  Beim Laden die betroffenen Tiles berechnen und speichern.
  Wenn dann updateReferences() aufgerufen wird werden die alten Referenzen gelöscht, die Neuen berechnet und gespeichert.


## Namen

Apoapsis:
> Maximale Höhe einer Umlaufbahn.

Dusk and Dawn:
> Abend- und Morgendämmerung
> Dusk: Die alte Welt geht zugrunde.
> Dawn: Eine neue Welt entsteht.

Too Cold To Shiver
> Weltraum ist kalt
> Keine Zeit um sich mit zittern aufzuhalten

Silent Scream

Derelict (treibendes Wrack)


## KABOOSH!

                  [Cellular Automata]
                          ||
                          ||
                          \/
                   [Voxel Database]
                          ||
                          ||
                          \/
    [Renderer] <== [Object Manager] ==> [Physics Simulator]

Object:
- transformation
- attachment target
- graphics:
  - mesh
  - textures
  - shader
  - shader parameters
  - render flags
- physics:
  - collision shapes
  - physics flags


## Multithreading und Objekt-Referenzen

Beispielsweise Texturen:

    Index | GL Handle | RT Refs | LT Refs
    ------+-----------+---------+--------
        0 |        42 |       0 |      1
    ------+-----------+---------+--------
        1 |        47 |       0 |      0
    ------+-----------+---------+--------
        2 |        20 |       3 |      2
    ------+-----------+---------+--------
        3 |        54 |       1 |      0

`ReferenceObject_Renderer()` and `ReleaseObject_Renderer()`
modify the `RT Refs` column.
The functions may only be called by the render thread.

`ReferenceObject_Lua()` and `ReleaseObject_Lua()`
modify the `LT Refs` column.
The functions may only be called by the Lua thread.

Since both columns are only accessed by one thread,
no thread synchoniztation is needed.

Then there is the `UpdateObjects()` function.
It is called whenever all participating threads are synchronized.
(Here: Lua and render thread.)

In case of textures `UpdateObjects()` only job is to garbage collect
unreferenced objects: Objects that are neither referenced by the render
nor by the Lua thread.
