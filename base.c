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

unsigned long timerStart = 0;            // Время начала отсчета после вставки монеты
const unsigned long activeTime = 300000; // 5 минут (300000 миллисекунд)
const unsigned long warningTime = 20000; // 20 секунд (20000 миллисекунд)
unsigned long lastBreathUpdate = 0;

bool gameActive = false;           // Флаг для активной игры
bool blinkPhase = false;           // Флаг для управления миганием в последние 20 секунд
unsigned long blinkStartTime = 0;  // Время начала мигания
int coinCount = 0;                 // Счетчик вставленных монет
int breathValue = 0;
bool breathUp = true;

void setup() {
  // Настраиваем пины реле как выходы
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);

  // Включаем реле при старте
  activateRelays();

  // Настраиваем остальные пины
  pinMode(coinSlotPin, INPUT_PULLUP);
  pinMode(ledCoinPin, OUTPUT);
  pinMode(ledPlayer1Pin, OUTPUT);
  pinMode(ledPlayer2Pin, OUTPUT);
  pinMode(relayPlayer1Pin, OUTPUT);
  pinMode(relayPlayer2Pin, OUTPUT);
  pinMode(relayCoinPin, OUTPUT);

  resetSystem(); // Сбрасываем систему в начальное состояние
}

void loop() {
  // Анимация светодиода, если игра не активна
  if (!gameActive) {
    breatheLed(ledCoinPin);
  }

  // Проверка на вставку монеты
  if (digitalRead(coinSlotPin) == LOW) {
    delay(50); // Антидребезг
    if (digitalRead(coinSlotPin) == LOW) {
      handleCoinInsertion(); // Обрабатываем вставку монетки
    }
  }

  if (gameActive) {
    unsigned long currentTime = millis();

    // Если прошло больше 5 минут, сбрасываем систему
    if (currentTime - timerStart > activeTime) {
      resetSystem();
    }
    // Если осталось меньше 20 секунд, мигаем светодиодом монетки
    else if (currentTime - timerStart > (activeTime - warningTime)) {
      blinkCoinWithIncreasingSpeed(currentTime);
    }
  }
}

// Активация реле в начале программы
void activateRelays() {
  delay(10000);
  digitalWrite(RELAY_1_PIN, HIGH);
  delay(2000);
  digitalWrite(RELAY_2_PIN, HIGH);
  delay(2000);
  digitalWrite(RELAY_3_PIN, HIGH);  
  delay(5000);
  digitalWrite(RELAY_4_PIN, HIGH);
}

// Обработка вставки монетки
void handleCoinInsertion() {
  coinCount++;
  gameActive = true;

  blinkCoinThreeTimes(); // Моргание индикатора монетки три раза

  if (coinCount == 1) {
    activateRelayAndLed(relayPlayer1Pin, ledPlayer1Pin);
  } 
  else if (coinCount >= 2) {
    activateAllRelaysAndLeds();
  }

  timerStart = millis();  // Запускаем таймер на 5 минут
}

// Активация всех реле и светодиодов
void activateAllRelaysAndLeds() {
  digitalWrite(relayCoinPin, LOW);    // Включаем реле для монетки
  digitalWrite(ledCoinPin, HIGH);     // Включаем светодиод для монетки

  digitalWrite(relayPlayer1Pin, LOW); // Включаем реле для 1 игрока
  digitalWrite(ledPlayer1Pin, HIGH);  // Включаем светодиод для 1 игрока

  digitalWrite(relayPlayer2Pin, LOW); // Включаем реле для 2 игрока
  digitalWrite(ledPlayer2Pin, HIGH);  // Включаем светодиод для 2 игрока
}

// Активация реле и светодиодов
void activateRelayAndLed(int relayPin, int ledPin) {
  digitalWrite(relayPin, LOW);  // Включаем реле
  digitalWrite(ledPin, HIGH);   // Включаем светодиод
}

// Сброс системы в начальное состояние
void resetSystem() {
  gameActive = false;
  coinCount = 0;
  digitalWrite(relayPlayer1Pin, HIGH);
  digitalWrite(relayPlayer2Pin, HIGH);
  digitalWrite(relayCoinPin, HIGH);
  digitalWrite(ledCoinPin, LOW);
  digitalWrite(ledPlayer1Pin, LOW);
  digitalWrite(ledPlayer2Pin, LOW);
}

// Моргание светодиода монетки три раза
void blinkCoinThreeTimes() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledCoinPin, HIGH);
    delay(70);
    digitalWrite(ledCoinPin, LOW);
    delay(100);
  }
  digitalWrite(ledCoinPin, HIGH); // Оставляем свет включенным
}

void breatheLed(int pin) {
  unsigned long currentTime = millis();
  if (currentTime - lastBreathUpdate > 8) {
    lastBreathUpdate = currentTime;
    if (breathUp) {
      breathValue++;
      if (breathValue >= 255) {
        breathValue = 255;
        breathUp = false;
      }
    } else {
      breathValue--;
      if (breathValue <= 0) {
        breathValue = 0;
        breathUp = true;
      }
    }
    analogWrite(pin, breathValue);  // Регулируем яркость светодиода
  }
}

// Мигание светодиода монетки с увеличивающейся частотой
void blinkCoinWithIncreasingSpeed(unsigned long currentTime) {
  unsigned long timeRemaining = activeTime - (currentTime - timerStart);
  unsigned long blinkInterval = map(timeRemaining, 0, warningTime, 50, 500);

  if (currentTime - blinkStartTime > blinkInterval) {
    blinkStartTime = currentTime;
    blinkPhase = !blinkPhase;
    digitalWrite(ledCoinPin, blinkPhase ? HIGH : LOW);
  }
}
