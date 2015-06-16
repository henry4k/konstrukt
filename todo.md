Mehrere Vertex Typen
--------------------

- In der Engine werden mehrere feste Vertex Typen definiert.
- Mesh und MeshBuffer speichern ihren Vertex Typ als Enum-Wert.
- Vertex Typen werden im Vertex-Modul eingerichtet.

Es gibt erstmal nur 2 Typen:

- GenericVertex:  Z.B. fuer statische Geometrie etc.
- ParticleVertex:  Fuer Partikel


Time Module
-----------

Ein Modul fuer die aktuelle Spielzeit und fuer Timer.

- Die aktuelle Spielzeit als Uniform-Variable.
- Nicht einfach `glfwGetTime` nutzen!


Overlays
--------

- Overlays werden gerendert, nachdem das basis Model gerendert wurde.
- Overlays modifizieren den Tiefenpuffer nicht.


Voxel Storage
-------------

Erstmal in Engine als simplen 3D Array implementieren.

In Lua einen Chunk-Manager und Chunks erstellen.
Es werden nur aktive Chunks vom Manager gespeichert.
Die Chunks enthalten Models und Solid(s) um die enthaltenen Voxel im Spiel
zu repraesentieren.  Im Chunk Manager muss jeder Struktur-Type registriert
werden.

Das Voxel Storage Modul sagt Lua bescheid, wenn Voxel geaendert wurden.
Darauf koennen dann die entsprechenden Chunks reagieren.

Siehe `notes/Voxel+Structures.md`


Model Manager Debug
-------------------

Wenn die Option `debug.model-ordering` aktiviert ist, werden Modelle vor dem
sortieren und rendern zufaellig eingeordnet.  Damit laesst sich feststellen,
ob das Render System richtig funktioniert.
