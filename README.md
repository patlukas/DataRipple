# DataRipple

## Opis projektu
**DataRipple** to narzędzie w języku C umożliwiające generowanie i zapisywanie plików zawierających określoną ilość bloków danych o zadanym rozmiarze i typie. Narzędzie pozwala na testowanie wydajności operacji wejścia/wyjścia oraz analizowanie wpływu różnych typów danych na zapis do plików.

## Wymagania
- Kompilator C (np. `gcc`)
- System operacyjny Linux

## Sposób kompilacji
Aby skompilować program, użyj komendy:
```sh
gcc -o DataRipple DataRipple.c
```

## Sposób użycia
Program należy uruchomić z następującymi argumentami:
```sh
./DataRipple <output_file> <num_blocks> <block_size> <data_type>
```
### Argumenty:
- `<output_file>` – ścieżka do pliku, w którym zostaną zapisane dane.
- `<num_blocks>` – liczba bloków do zapisania (można dodać sufiksy `K`, `M`, `G` dla większych jednostek).
- `<block_size>` – rozmiar każdego bloku w bajtach (można dodać sufiksy `K`, `M`, `G`).
- `<data_type>` – sposób generowania danych:
  - `0` – bloki wypełnione zerami.
  - `1` – bloki wypełnione wartością `0xFF`.
  - `block_rand` lub `br` – każdy blok zawiera losowe wartości z wykorzystaniem `rand()`.
  - `all_rand` lub `r` – całe dane są generowane losowo z wykorzystaniem `rand()`.
  - `all_urand` lub `u` – całe dane są generowane losowo z wykorzystaniem `/dev/urandom`.

### Przykłady użycia:
#### Generowanie pliku `output.dat` z 100 blokami po 4 KB wypełnionymi zerami:
```sh
./DataRipple output.dat 100 4K 0
```
#### Generowanie pliku `random_data.bin` z 50 blokami po 1 MB losowych danych:
```sh
./DataRipple random_data.bin 50 1M all_rand
```

## Wynik działania
Po zakończeniu programu zostanie wyświetlona informacja o zapisie, np.:
```
Success: Wrote 100 blocks of 4096B, saving a total of 400.00KB (409600B) to output.dat in 0.002345s.
```

Jeżeli wystąpi błąd, zostanie wyświetlona odpowiednia informacja diagnostyczna.

![](https://github.ct8.pl/readme/patlukas/DataRipple)

