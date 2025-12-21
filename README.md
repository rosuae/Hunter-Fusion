# Metroid: Hunter Fusion

Metroid: Hunter Fusion este un platformer realizat în C++, cu structură pe camere interconectate. Jocul va utiliza biblioteca SFML pentru randare, input și sunet. Elementele principale includ un engine simplu cu sistem de randare pe bază de tilemap și sprites, fizică 2D (gravitație, viteză, coliziuni), cameră dinamică ce urmărește jucătorul și încărcare modulară a nivelurilor.

Gameplay-ul se bazează pe controlul unui personaj care poate merge, sări, trage proiectile și obține upgrade-uri precum dublu-salt sau dash. Structura de nivel este împărțită în camere conectate prin uși, iar deplasarea este limitată inițial de abilitățile jucătorului. Vor exista inamici cu comportament de bază (patrulare, urmărire, atac), proiectile, precum și bătălii cu boși ce utilizează atacuri scriptate. Sistemele implementate vor include coliziuni precise, "health system" și damage, spawn de entități și colectabile.



## Controls:
- `A, D` Move Left / Right.
- `S` Crouch.
- `W / Space (Hold)` Jump.
- `Left / Right / Up Arrow` Shoot.
- `R` Reload.
- `Escape` Menu.

## Assets:
### Sprites:
- *Player:* https://www.spriters-resource.com/browser_games/supermariobroscrossover/asset/177036/
- *Enemies:* https://www.spriters-resource.com/snes/smetroid/asset/1725/
- *Tiles:* https://www.spriters-resource.com/snes/smetroid/asset/144515/
- *More Sprites:* https://www.spriters-resource.com/snes/smetroid/
- *Sounds:*
  - https://sounds.spriters-resource.com/wii_u/nintendoland/asset/426887/
  - https://sounds.spriters-resource.com/snes/smetroid/asset/439917/
  - https://sounds.spriters-resource.com/wii_u/supermariomaker/asset/400349/



## To do:
 
- More enemies + Bosses
- Animation class to implement animations based on actions
- PowerUps + Ammo and Health Drops
- More types of ammo
- Option to change the game volume
- Ability to change player look

## Known Bugs:

- Player can run out of ammo. Drops system will be implemented
- Player can get stuck in tiles while performing actions like shooting while jumping

## Tema 2

#### Cerințe
- [x] separarea codului din clase în `.h` (sau `.hpp`) și `.cpp`
- [x] moșteniri:
  - minim o clasă de bază și **3 clase derivate** din aceeași ierarhie
  - ierarhia trebuie să fie cu bază proprie, nu derivată dintr-o clasă predefinită
  - [x] funcții virtuale (pure) apelate prin pointeri de bază din clasa care conține atributul de tip pointer de bază
    - minim o funcție virtuală va fi **specifică temei** (i.e. nu simple citiri/afișări sau preluate din biblioteci i.e. draw/update/render)
    - constructori virtuali (clone): sunt necesari, dar nu se consideră funcții specifice temei
    - afișare virtuală, interfață non-virtuală
  - [x] apelarea constructorului din clasa de bază din constructori din derivate
  - [x] clasă cu atribut de tip pointer la o clasă de bază cu derivate; aici apelați funcțiile virtuale prin pointer de bază, eventual prin interfața non-virtuală din bază
    - [x] suprascris cc/op= pentru copieri/atribuiri corecte, copy and swap
    - [x] `dynamic_cast`/`std::dynamic_pointer_cast` pentru downcast cu sens
    - [x] smart pointers (recomandat, opțional)
- [x] excepții
  - [x] ierarhie proprie cu baza `std::exception` sau derivată din `std::exception`; minim **3** clase pentru erori specifice distincte
    - clasele de excepții trebuie să trateze categorii de erori distincte (exemplu de erori echivalente: citire fișiere cu diverse extensii)
  - [x] utilizare cu sens: de exemplu, `throw` în constructor (sau funcție care întoarce un obiect), `try`/`catch` în `main`
  - această ierarhie va fi complet independentă de ierarhia cu funcții virtuale
- [x] funcții și atribute `static`
- [x] STL
- [x] cât mai multe `const`
- [x] funcții *de nivel înalt*, de eliminat cât mai mulți getters/setters/funcții low-level
- [x] minim 75-80% din codul propriu să fie C++
- [ ] la sfârșit: commit separat cu adăugarea unei noi clase derivate fără a modifica restul codului, **pe lângă cele 3 derivate deja adăugate** din aceeași ierarhie
  - noua derivată nu poate fi una existentă care a fost ștearsă și adăugată din nou
  - noua derivată va fi integrată în codul existent (adică va fi folosită, nu adăugată doar ca să fie)
- [ ] tag de `git` pe commit cu **toate bifele**: de exemplu `v0.2`
- [ ] code review #2 2 proiecte

## Tema 3

#### Cerințe
- [ ] 2 șabloane de proiectare (design patterns)
- [ ] o clasă șablon cu sens; minim **2 instanțieri**
  - [ ] preferabil și o funcție șablon (template) cu sens; minim 2 instanțieri
- [ ] minim 85% din codul propriu să fie C++
<!-- - [ ] o specializare pe funcție/clasă șablon -->
- [ ] tag de `git` pe commit cu **toate bifele**: de exemplu `v0.3` sau `v1.0`
- [ ] code review #3 2 proiecte

## Resurse
<!-- renovate: datasource=github-tags depName=SFML/SFML versioning=loose -->
- [SFML](https://github.com/SFML/SFML/tree/3.0.2) (Zlib)
- https://code.tutsplus.com/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673t (Map creation logic)

![metroid-samus-aran-vs-alien-4k-wallpaper-uhdpaper com-572@2@a](https://github.com/user-attachments/assets/b9edee4b-5e42-41d3-9d38-f47047061e10)
