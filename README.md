# Automatyczna brama dwuskrzydłowa

## Opis projektu

Projekt przedstawia system automatycznego sterowania bramą dwuskrzydłową z wykorzystaniem Arduino. Brama składa się z dwóch skrzydeł poruszanych za pomocą serwomechanizmów.

System umożliwia:
- Automatyczne otwieranie i zamykanie bramy.
- Wykrywanie ruchu za pomocą czujnika PIR.
- Zatrzymywanie i wznawianie ruchu za pomocą przycisku.
- Automatyczne ponowne otwieranie bramy po wykryciu ruchu podczas zamykania.
- Sygnalizowanie ruchu bramy za pomocą migającej diody LED.

## Wykorzystane komponenty

- Arduino
- 2 × serwomechanizm
- Czujnik ruchu PIR
- Przycisk
- Dioda LED
- Rezystor do diody LED
- Przewody połączeniowe

## Podłączenie elementów

| Element | Pin Arduino |
|---|---:|
| Dioda LED | 7 |
| Lewe serwo | 9 |
| Prawe serwo | 8 |
| Czujnik PIR | 12 |
| Przycisk | 10 |

Przycisk korzysta z wewnętrznego rezystora podciągającego (`INPUT_PULLUP`), dlatego należy podłączyć go pomiędzy pin 10 a GND.

## Działanie systemu

Sterowanie bramą opiera się na maszynie stanów (FSM – Finite State Machine).

### Dostępne stany

| Stan | Opis |
|---|---|
| `OTWARTE` | Brama jest całkowicie otwarta. |
| `ZAMKNIETE` | Brama jest całkowicie zamknięta. |
| `OTWIERA_SIE` | Brama wykonuje ruch otwierania. |
| `ZAMYKA_SIE` | Brama wykonuje ruch zamykania. |
| `ZATRZYMANA_PRZY_OTWARCIU` | Ruch został zatrzymany podczas otwierania. |
| `ZATRZYMANA_PRZY_ZAMKNIECIU` | Ruch został zatrzymany podczas zamykania. |

### Sterowanie przyciskiem

- Gdy brama jest zamknięta, naciśnięcie przycisku rozpoczyna otwieranie.
- Gdy brama jest otwarta, naciśnięcie przycisku rozpoczyna zamykanie.
- Podczas otwierania lub zamykania przycisk zatrzymuje ruch.
- Kolejne naciśnięcie przycisku powoduje zmianę kierunku ruchu.

### Działanie czujnika PIR

- Podczas zamykania wykrycie ruchu powoduje natychmiastową zmianę kierunku na otwieranie.
- Jeśli brama jest zatrzymana podczas zamykania, wykrycie ruchu również rozpoczyna otwieranie.
- Podczas otwierania czujnik PIR nie zmienia kierunku ruchu.

### Sygnalizacja LED

Dioda LED:
- Świeci naprzemiennie (miga) podczas ruchu bramy.
- Jest wyłączona, gdy brama stoi.

## Parametry czasowe

| Parametr | Wartość |
|---|---:|
| Czas pełnego otwierania | 10 sekund |
| Czas pełnego zamykania | 10 sekund |
| Interwał migania LED | 400 ms |
| Czas eliminacji drgań przycisku | 50 ms |
| Interwał aktualizacji serw | 20 ms |

## Wykorzystane rozwiązania programistyczne

- **Maszyna stanów (FSM)** – organizuje działanie programu i przejścia między stanami.
- **`millis()`** – kontroluje upływ czasu bez blokowania wykonywania programu.
- **`switch-case`** – wybiera funkcję odpowiedzialną za aktualny stan bramy.
- **Debouncing** – eliminuje przypadkowe wielokrotne odczyty pojedynczego naciśnięcia przycisku.
- **Biblioteka `Servo.h`** – umożliwia sterowanie serwomechanizmami.
- **Interpolacja pozycji** – pozwala na stopniową zmianę kąta serw podczas otwierania i zamykania.
- **Monitor portu szeregowego (`Serial`)** – wyświetla komunikaty dotyczące aktualnego stanu i zdarzeń systemu.

## Uruchomienie

1. Podłącz wszystkie elementy zgodnie z tabelą połączeń.
2. Wgraj program na płytkę Arduino.
3. Uruchom system.
4. Otwórz monitor portu szeregowego z prędkością `9600 baud`.
5. Steruj bramą za pomocą przycisku i sprawdź reakcję na wykrycie ruchu przez czujnik PIR.

## Technologie

- C++
- Arduino
- Arduino Servo Library
- Tinkercad Circuits (symulacja projektu)
