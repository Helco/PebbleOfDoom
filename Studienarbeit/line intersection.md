# line intersection

## Algorithmus 1: Linienschnitt
1. Die Strecken werden als Linien angesehen und der Schnittpunkt zwischen den Linien berechnet.
2. Überpüfen ob der Schnitpunkt auf einer der Geraden liegt.

### Durchführung
1. Strecke durch Geradengleicung beschreiben <br>
    y = mx + n <br>
    n = Punkt1 <br>
    m = Differenzenquotient zwischen Punkt1 und Punkt2
2. beide Geradengleichungen gleichsetzen und Schnittpunkt ausrechnen <br>
    Achtung: parallele Geraden leifern kein Ergebnis bzw. gleiche Geraden liefern undendlich viele Ergebnisse
3. Ortsvektor des Punkts in eine Geradegleichung einsetzen und nach x auflösen <br>
    0 < x < 1 --> Punkt liegt auf der Strecke <br>
    x < 0 || x > 1 --> Punkt liegt nicht auf der Strecke <br>

Source:
- https://www.frustfrei-lernen.de/mathematik/schnittpunkt-zweier-geraden.html
- http://rechen-fuchs.de/lagebeziehung-punkt-strecke/ 