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

---

## Algorithmus 2: Alg. mit baryzentrischen Koordinaten
1. Erste Strecke nehmen und die baryzentrischen Koordinaten der beiden Punkte der anderen Strecke berechnen
2. Wenn beide Koordinaten auf einer Seite liegen gibt es keinen Schnittpunkt <br>
3. Wenn nicht dann 1. und 2. Mit anderer Strecke durchführen
4. Wenn diese auf einer Seite liege, so gibt es keinen Schnittpunkt
5. Wenn diese auch auf entgegengesetzten Seiten liegen, ist ein schnittpunkt vorhanden <br>
Ergebnis: Wissen ob ein Schnittpunkt vorhanden ist
6. Schnittpunkt gegebenenfalls berechnen mit Schritt 1. und 2. des Algorithmus 1

Source: Keine Quelle vorhanden da der Algorithmus selbst ausgedacht ist (vermutlich existiert dieser allerdings)

Interesante Sache: <br>
https://physik.cosmos-indirekt.de/Physik-Schule/Baryzentrische_Koordinaten <br>
Beschreibt wie Parallelität zweier Geraden berechnet werden kann

---

## Algorithmus 3:
- Algorithmus von https://rosettacode.org/wiki/Find_the_intersection_of_two_lines#C.2B.2B C++ Implementierung, leider ist mir nicht bewusst ob dieser das gewünschte Ergebnis liefert oder inkorrekt ist

### Durchführung
- Gerade A(A1,A2) und B(B1,B2)
- Matrix <br>[(a,b) <br> (c,d)]<br>
- Determinate der Matrix: a*d-b*c
- a = A1.x - A2.x
- b = A1.y - A2x
- c = B1.x - B2.x
- d = B1.y - B2.y
- Determinate(a,b,c,d)
- Wenn Determinate == 0 dann parallel
- Sonst Schittpunkt P(x,y)
- P.x = Det(Det(A), a, Det(B), c) / Determinate(a,b,c,d)
- P.y = Det(Det(A), b, Det(B), d) / Determinate(a,b,c,d)

---

## Algorithmus 4: plane sweep
Nur mit aufgenommen als Algorithmus der beliebige viele Strecken auf all ihre Schnittpunkte überprüft. <br>
Source: https://people.inf.elte.hu/fekete/algoritmusok_msc/terinfo_geom/konyvek/Computational%20Geometry%20-%20Algorithms%20and%20Applications,%203rd%20Ed.pdf 