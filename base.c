#define RELAY_1_PIN 2       // Пин для реле 1
#define RELAY_2_PIN 3       // Пин для реле 2
#define RELAY_3_PIN 12      // Пин для реле 3
#define RELAY_4_PIN 13      // Пин для реле 4

// Пины для монетоприемника и светодиодов
const int coinSlotPin = 11;       // Пин монетоприемника
const int ledCoinPin = 9;         // Пин светодиода для монетки (Coin)
const int ledPlayer1Pin = 8;      // Пин светодиода для 1 игрока
const int ledPlayer2Pin = 10;     // Пин светодиода для 2 игрока
const int relayPlayer1Pin = 6;    // Пин реле для 1 игрока
const int relayPlayer2Pin = 5;    // Пин реле для 2 игрока
const int relayCoinPin = 4;       // Пин реле для монетки

int coinCount = 0;  // Счетчик вставленных монет

void setup() {
  // Настраиваем пины реле как выходы
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);

  // Настраиваем остальные пины как выходы
  pinMode(coinSlotPin, INPUT_PULLUP);
  pinMode(ledCoinPin, OUTPUT);
  pinMode(ledPlayer1Pin, OUTPUT);
  pinMode(ledPlayer2Pin, OUTPUT);
  pinMode(relayPlayer1Pin, OUTPUT);
  pinMode(relayPlayer2Pin, OUTPUT);
  pinMode(relayCoinPin, OUTPUT);

  // Инициализация реле в выключенном состоянии
  digitalWrite(RELAY_1_PIN, HIGH);
  digitalWrite(RELAY_2_PIN, HIGH);
  digitalWrite(RELAY_3_PIN, HIGH);
  digitalWrite(RELAY_4_PIN, HIGH);
  digitalWrite(relayPlayer1Pin, HIGH);
  digitalWrite(relayPlayer2Pin, HIGH);
  digitalWrite(relayCoinPin, HIGH);

  // Задержка перед включением реле поочередно
  delay(2000);

  digitalWrite(RELAY_1_PIN, LOW);  // Включаем реле 1
  delay(1000);
  digitalWrite(RELAY_2_PIN, LOW);  // Включаем реле 2
  delay(1000);
  digitalWrite(RELAY_3_PIN, LOW);  // Включаем реле 3
  delay(1000);
  digitalWrite(RELAY_4_PIN, LOW);  // Включаем реле 4
}

void loop() {
  // Проверка на вставку монеты
  if (digitalRead(coinSlotPin) == LOW) {
    delay(50); // Антидребезг
    if (digitalRead(coinSlotPin) == LOW) {
      handleCoinInsertion();  // Обработка вставки монеты
    }
  }
}

// Обработка вставки монеты
void handleCoinInsertion() {
  coinCount++;

  if (coinCount == 1) {
    activateRelayAndLed(relayPlayer1Pin, ledPlayer1Pin);  // Активируем кнопку 1 игрока
  } 
  else if (coinCount == 2) {
    activateRelayAndLed(relayCoinPin, ledCoinPin);        // Активируем кнопку Coin
    activateRelayAndLed(relayPlayer2Pin, ledPlayer2Pin);  // Активируем кнопку 2 игрока
  }
}

// Активация реле и светодиодов
void activateRelayAndLed(int relayPin, int ledPin) {
  digitalWrite(relayPin, LOW);  // Включаем реле
  digitalWrite(ledPin, HIGH);   // Включаем светодиод
}

// Сброс системы в начальное состояние
void resetSystem() {
  coinCount = 0;
  digitalWrite(relayPlayer1Pin, HIGH);
  digitalWrite(relayPlayer2Pin, HIGH);
  digitalWrite(relayCoinPin, HIGH);
  digitalWrite(ledCoinPin, LOW);
  digitalWrite(ledPlayer1Pin, LOW);
  digitalWrite(ledPlayer2Pin, LOW);
}
