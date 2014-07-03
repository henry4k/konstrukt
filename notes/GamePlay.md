Mixins:
    "Kann Rohre enthalten" (Kann mehrere separate Rohre des gleichen Typs führen.)
    "Rahmenfunktionalität"
    "Benötigt Stromanschluss"
    "Benötigt Kommunikationsanschluss"

Strukturen:
    Leere:
        Kann Rohren enthalten.

    Gerüst: Leere
        Rahmenfunktionalität.
        Kann Rohren enthalten.

        Jede Seite kann mit einer Platte versehen werden:
            verschweisst:
                100% Abdichtung
                Stabil (wiedesteht höheren Druckunterschieden)
                Kann nur langsam entfernt werden.

            verschraubt:
                50% Abdichtung
                Stabil (wiedersteht höheren Druckunterschieden)
                Kann entfernt werden.

            Deckplatte:
                0% Abdichtung
                Instabil (wiedersteht kleineren Druckunterschieden)
                Kann schnell entfernt werden.

    Schleuse: Gerüst (2D; ohne Platten; ohne Rohre)
        Benötigt Stromanschluss.
        Benötigt Kommunikationsanschluss.

    Einfaches Fenster: Gerüst (2D; ohne Platten; ohne Rohre)
        50% Abdichtung
        Instabil (wiedersteht sehr kleinen Druckunterschieden)

    Sicheres Fenster: Gerüst (2D; ohne Platten; ohne Rohre)
        100% Abdichtung
        Stabil (wiedersteht höheren Druckunterschieden)

    Akkumulator: Leere
        Benötigt Stromanschluss.

    Recheneinheit: Leere
        Benötigt Stromanschluss.
        Benötigt Kommunikationsanschluss.

        Zugriff nur über ein angeschlossenes Terminal
        oder über die Kommunikationsleitung.

    Terminal: Recheneinheit
        Benötigt Stromanschluss.
        Benötigt Kommunikationsanschluss.

    Display: Leere
        Benötigt Stromanschluss.
        Benötigt Kommunikationsanschluss.

Rohre:
    Stromverbindung
    Kommunikationsverbindung (aka LAN-Kabel)
    Fluidverbindung

Kommunikationsnetzwerk: (aka LAN)
    Alle ans Netzwerk angeschlossenen Geräte können untereinander kommunizieren.
    Jedes Gerät verfügt über eine Liste von Aktionen, die man an diesem ausführen kann.