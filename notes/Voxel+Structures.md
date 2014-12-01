#### Serialisierung/Deserialisierung der Welt ####

Die Voxel-Daten können ohnehin leicht serialisiert werden.
Man würde also einfach den Voxel-Array als Blob auf die Festplatte schreiben.

Dazu könnte im Script ein Funktion bereitstehen.
::native.WriteVoxels( filename )

Wenn man dann übers Script die Welt speichern will,
werden die Voxel-Daten geschrieben und ein JSON-Dokument erzeugt,
wo das Script z.B. Entities etc speichert.


#### Wie können Voxel modularer programmiert werden? ####

Voxel haben eine Typ-ID, einen Bereich für Typ-spezifische Daten und die CA-Daten:
Fluide, Temperatur, ...

Da zusammenhängende Strukturen gebaut werden können,
welche mehr als einen Voxel einnehmen, muss zwischen Voxeln und Strukturen
unterschieden werden.

Eine Struktur umfasst ein oder mehrere Voxel, welche logisch zusammengehören:
Ein Wand-Element besteht zum Beispiel nur aus einem Voxel.
Eine Luftschleuse dagegen umfasst mehrere Voxel.
Strukturen müssen immer eine AABB bilden? (Optimierungen?)

Implementation:

v1:
Eine Struktur hat einen Kern-Voxel,
auf welchen alle anderen Voxel der Struktur verweisen.
Die Verweise sind immer relative Offset-Werte.
Die Klasse der Struktur, allerdings selber wissen, welche Voxel zu ihr gehören.
Ansonsten könnten 'Zombie'-Voxel entstehen, welche auf eine nicht existierende
oder falsche Struktur verweisen.

v2:
Strukturen sind immer AABBs.
Der 'Kern-Voxel' ist immer der Ursprung des AABBs.
Dadurch hat man prinzipiell immernoch die Probleme von v1,
aber man kann sehr leicht die Integrität prüfen
und die Implementation ist sehr einfach, wodurch weniger Fehler entstehen sollten.

```
Voxel:
    init(x,y,z):
        read()
        Init sollte aber nicht direkt aufgerufen werden,
        etwas wie world.getVoxelAt(x,y,z) währe logischer.

    read():
        Überschreibt DAO mit Voxel-Daten an gegebener Position.

    write():
        Wenn dirty, schreibt Voxel-Daten.

    Die Position darf nicht verändert werden.

    DAO wird als Dirty markiert, wenn Eigenschaften verändert wurden.
```

Voxel haben folgendes Format:
- Typ-ID
- Offset zum Ursprungspunkt
- Brenutzerdaten
- CA Daten (Fluide, Temperatur, ...)


Man muss Struktur-Typen registrieren.
registerStructure(class)
Die ID wird iwie automatisch erzeugt:
- Hash aus Klassen-Name
- automatisches Hochzählen
- aus Datei laden
- aus Klassenvariable

struct = getStructureAt(x,y,z)
An der gegebenen Position wird die Typ-ID ausgelesen.
Über die Typ-ID wird die Struktur-Klasse ermittelt.


Structure:
    init(x,y,z):
        Ursprungsposition.
        origin = {x,y,z}
        read()

    check():
        Integritätstest.
        Alle zu der Struktur gehörigen Voxel werden auf Integrität geprüft.

    size():
        Muss von der Klasse implementiert werden.

    Hat ausserdem diverse Callbacks, z.B.:

    generateSolid(buffer):
        Wird z.B. vom Chunk aufgerufen, wenn ihn ihm Voxel geändert wurden
        und dieser nun die statischen Solids aktualisieren muss.

    generateModel(buffer):
        Wird z.B. vom Chunk aufgerufen, wenn ihn ihm Voxel geändert wurden
        und dieser nun die statischen Models aktualisieren muss.

    etc.