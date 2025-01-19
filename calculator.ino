#include <Wire.h> // Include biblioteca pentru comunicare I2C
#include <LiquidCrystal_I2C.h> // Include biblioteca pentru LCD-ul I2C
#include <Keypad.h> // Include biblioteca pentru tastatură

int buzzer_Pin = 10; // Pinul la care este conectat buzzer-ul
int led_Pin = 11; // Pinul la care este conectat LED-ul
int button_Pin = 8; // Pinul la care este conectat butonul
bool calculatorOn = false; // Variabila pentru a urmări starea calculatorului (pornit/oprit)
bool result = false; // Variabila pentru a urmări dacă este prezent un rezultat
int Number = 0; // Variabila pentru a stoca un număr introdus
int Num1 = 0; // Variabila pentru primul număr din operație
int Num2 = 0; // Variabila pentru al doilea număr din operație
char action = ' '; // Variabila pentru a stoca operația matematică

int sound_On = 3000; // Sunet pentru starea "ON"
int sound_Off = 2000; // Sunet pentru starea "OFF"

LiquidCrystal_I2C lcd(0x27, 16, 2); // Inițializare LCD cu adresa 0x27 și dimensiunile 16x2

const byte ROWS = 4; // Numărul de rânduri ale tastaturii
const byte COLS = 4; // Numărul de coloane ale tastaturii

char keys[ROWS][COLS] = { // Matricea pentru harta tastelor
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3}; // Vectorul pentru pini rând
byte colPins[COLS] = {4, 5, 6, 7}; // Vectorul pentru pini coloană

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Inițializare tastatură

void setup() {
    pinMode(led_Pin, OUTPUT); // Setează pinul LED-ului ca ieșire
    pinMode(button_Pin, INPUT); // Setează pinul butonului ca intrare

    lcd.begin(); // Inițializează LCD-ul
    lcd.backlight(); // Activează iluminarea LCD-ului
    lcd.print("DIY Calculator"); // Afișează un mesaj de pornire pe LCD
    lcd.setCursor(0, 1);
    lcd.print("-CircuitDigest");
    delay(2000);
    lcd.clear();
    digitalWrite(led_Pin, LOW); // Oprește LED-ul la pornire
}

void loop() {
    // Variabila statică pentru a reține starea butonului
    static bool wasPressed = false;

    // Verifică dacă butonul este apăsat și nu a fost apăsat anterior
    if (digitalRead(button_Pin) == LOW && !wasPressed) {
        // Setează wasPressed ca fiind true pentru a indica că butonul a fost apăsat
        wasPressed = true;

        // Verifică și gestionează starea calculatorului
        if (!calculatorOn) {
            // Pornește calculatorul și afișează "Calculator ON" pe LCD
            calculatorOn = true;
            digitalWrite(led_Pin, HIGH);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Calculator ON ");
            delay(500);

            // Generează sunet pentru starea "ON"
            tone(buzzer_Pin, sound_On, 200);
            delay(200);
            noTone(buzzer_Pin);
        } else {
            // Oprește calculatorul și afișează "Calculator OFF" pe LCD
            calculatorOn = false;
            digitalWrite(led_Pin, LOW);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Calculator OFF");
            delay(500);

            // Generează sunet pentru starea "OFF"
            tone(buzzer_Pin, sound_Off, 200);
            delay(200);
            noTone(buzzer_Pin);
        }
    } else if (digitalRead(button_Pin) == HIGH) {
        // Dacă butonul este eliberat, actualizează wasPressed pentru a permite detectarea ulterioară a apăsării butonului
        wasPressed = false;
    }

    // Verifică dacă calculatorul este pornit și gestionează comenzile de la tastatură
    if (calculatorOn) {
        char key = kpd.getKey();
        if (key != NO_KEY) {
            DetectButtons(key); // Apel la funcția care gestionează butoanele
            DisplayResult(); // Afișează rezultatul
            if (result == true) {
                CalculateResult(); // Calculează rezultatul
                DisplayResult(); // Afișează rezultatul calculat
                Num1 = Num2 = Number = 0; // Resetarea variabilelor pentru următoarea operație
                result = false;
            }
        }
    }
}

void DetectButtons(char key) {
    lcd.clear(); // Curăță ecranul LCD

    if (key == '*') {
        Serial.println("Button Cancel"); // Afișează în Serial Monitor mesajul pentru anularea operației
        Number = Num1 = Num2 = 0; // Resetează variabilele pentru a anula operația
        result = false; // Setează rezultatul ca fiind fals
    } else if (key >= '0' && key <= '9') {
        if (Number == 0) {
            Number = key - '0'; // Converteste caracterul tastat in numar
        } else {
            Number = (Number * 10) + (key - '0'); // Construiește numărul din mai multe cifre
        }

        // Generează sunet specific pentru cifră
        switch (key) {
            case '0':
                tone(buzzer_Pin, 470, 200);
                break;
            case '1':
                tone(buzzer_Pin, 523, 200);
                break;
            case '2':
                tone(buzzer_Pin, 700, 200);
                break;
            case '3':
                tone(buzzer_Pin, 410, 200);
                break;
            case '4':
                tone(buzzer_Pin, 998, 200);
                break;
            case '5':
                tone(buzzer_Pin, 120, 200);
                break;
            case '6':
                tone(buzzer_Pin, 250, 200);
                break;
            case '7':
                tone(buzzer_Pin, 310, 200);
                break;
            case '8':
                tone(buzzer_Pin, 380, 200);
                break;
            case '9':
                tone(buzzer_Pin, 1030, 200);
                break;
        }
        delay(200); // O mică pauză între sunet și oprirea sunetului
        noTone(buzzer_Pin); // Oprirea sunetului
    } else if (key == '#') {
        Serial.println("Button Equal"); // Afișează în Serial Monitor mesajul pentru egal
        Num2 = Number; // Salvează numărul introdus pentru a fi folosit în operație
        result = true; // Setează rezultatul ca fiind adevărat pentru a calcula operația
       
        // Generează sunet pentru egal
        tone(buzzer_Pin, 950, 200);
        delay(200);
        noTone(buzzer_Pin);
    } else if (key == 'A') {
        Serial.println("Addition"); // Afișează în Serial Monitor mesajul pentru adunare
        action = '+'; // Setează operația curentă ca fiind adunare
        Num1 = Number; // Salvează numărul introdus pentru a fi folosit în operație
        Number = 0; // Resetează numărul pentru introducerea celui de-al doilea număr
        
        // Generează sunet pentru adunare
        tone(buzzer_Pin, 660, 200);
        delay(200);
        noTone(buzzer_Pin);
    } else if (key == 'B') {
        Serial.println("Subtraction"); // Afișează în Serial Monitor mesajul pentru scădere
        action = '-'; // Setează operația curentă ca fiind scădere
        Num1 = Number; // Salvează numărul introdus pentru a fi folosit în operație
        Number = 0; // Resetează numărul pentru introducerea celui de-al doilea număr
        
        // Generează sunet pentru scădere
        tone(buzzer_Pin, 880, 200);
        delay(200);
        noTone(buzzer_Pin);
    } else if (key == 'C') {
        Serial.println("Multiplication"); // Afișează în Serial Monitor mesajul pentru înmulțire
        action = '*'; // Setează operația curentă ca fiind înmulțire
        Num1 = Number; // Salvează numărul introdus pentru a fi folosit în operație
        Number = 0; // Resetează numărul pentru introducerea celui de-al doilea număr
        
        // Generează sunet pentru înmulțire
        tone(buzzer_Pin, 910, 200);
        delay(200);
        noTone(buzzer_Pin);
    } else if (key == 'D') {
        Serial.println("Division"); // Afișează în Serial Monitor mesajul pentru împărțire
        action = '/'; // Setează operația curentă ca fiind împărțire
        Num1 = Number; // Salvează numărul introdus pentru a fi folosit în operație
        Number = 0; // Resetează numărul pentru introducerea celui de-al doilea număr
        
        // Generează sunet pentru împărțire
        tone(buzzer_Pin, 778, 200);
        delay(200);
        noTone(buzzer_Pin);
    }
}

void CalculateResult() {
    if (action == '+')
        Number = Num1 + Num2; // Adunarea numerelor
    else if (action == '-')
        Number = Num1 - Num2; // Scăderea numerelor
    else if (action == '*')
        Number = Num1 * Num2; // Înmulțirea numerelor
    else if (action == '/')
        Number = Num1 / Num2; // Împărțirea numerelor
}

void DisplayResult() {
    lcd.setCursor(0, 0);
    lcd.print(Num1); // Afișează primul număr
    lcd.print(action); // Afișează simbolul operației
    lcd.print(Num2); // Afișează al doilea număr

    if (result == true) { // Verifică dacă rezultatul este disponibil pentru afișare
        lcd.print(" =");
        lcd.print(Number); // Afișează rezultatul

        // Parcurge fiecare cifră din rezultat și generează sunetul specific pentru fiecare cifră
        int temp = Number;
        while (temp != 0) {
            int digit = temp % 10; // Ia ultima cifră din rezultat
            switch (digit) {
                case 0:
                    tone(buzzer_Pin, 470, 200);
                    break;
                case 1:
                    tone(buzzer_Pin, 523, 200);
                    break;
                case 2:
                    tone(buzzer_Pin, 700, 200);
                    break;
                case 3:
                    tone(buzzer_Pin, 410, 200);
                    break;
                case 4:
                    tone(buzzer_Pin, 938, 200);
                    break;
                case 5:
                    tone(buzzer_Pin, 120, 200);
                    break;
                case 6:
                    tone(buzzer_Pin, 250, 200);
                    break;
                case 7:
                    tone(buzzer_Pin, 310, 200);
                    break;
                case 8:
                    tone(buzzer_Pin, 380, 200);
                    break;
                case 9:
                    tone(buzzer_Pin, 1030, 200);
                    break;
            }
            delay(200); // Pauză între sunetele cifrelor
            noTone(buzzer_Pin); // Oprește sunetul pentru cifra respectivă
            temp /= 10; // Trece la următoarea cifră din rezultat
        }
    }

    lcd.setCursor(0, 1);
    lcd.print(Number); // Afișează rezultatul final pe ecran
}

