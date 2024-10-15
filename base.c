#include <FastLED.h>

#define NUM_LEDS 8          // Количество светодиодов
#define DATA_PIN 1          // Пин для подключения светодиодной полоски
#define RELAY_1_PIN 2       // Пин для реле 1
#define RELAY_2_PIN 3       // Пин для реле 2
#define RELAY_3_PIN 12      // Пин для реле 3
#define RELAY_4_PIN 13      // Пин для реле 4

CRGB leds[NUM_LEDS];

int pos = 0;               // Текущая позиция светодиода
int direction = 1;         // Направление движения (1 - вправо, -1 - влево)
int baseDelay = 30;        // Базовая задержка для эффекта ускорения
unsigned long lastAnimationTime = 0; // Время последнего обновления анимации

// Пины для монетоприемника и светодиодов
const int coinSlotPin = 11;       // Пин монетоприемника
const int ledCoinPin = 9;         // Пин светодиода для монетки (Coin)
const int ledPlayer1Pin = 8;      // Пин светодиода для 1 игрока
const int ledPlayer2Pin = 10;     // Пин светодиода для 2 игрока
const int relayPlayer1Pin = 6;    // Пин реле для 1 игрока
const int relayPlayer2Pin = 5;    // Пин реле для 2 игрока
const int relayCoinPin = 4;       // Пин реле для монетки

const int startButtonPin = A1;    // Пин кнопки "Старт" (аналоговый как цифровой)

unsigned long timerStart = 0;            // Время начала отсчета после вставки монеты
const unsigned long activeTime = 240000; // 4 минуты (240000 миллисекунд)
const unsigned long warningTime = 20000; // 20 секунд (20000 миллисекунд)
unsigned long lastBreathUpdate = 0;

unsigned long startButtonPressTime = 0;  // Время нажатия кнопки "Старт"
bool startButtonPressed = false;         // Флаг нажатия кнопки "Старт"

bool gameActive = false;           // Флаг для активной игры
bool blinkPhase = false;           // Флаг для управления миганием в последние 20 секунд
unsigned long blinkStartTime = 0;  // Время начала мигания
int coinCount = 0;                 // Счетчик вставленных монет
int breathValue = 0;
bool breathUp = true;

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.clear();         // Очищаем светодиоды при старте

  // Настраиваем пины реле как выходы
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);

  // Настраиваем пин кнопки как вход
  pinMode(startButtonPin, INPUT_PULLUP);

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
  // Анимация светодиодной ленты, если игра не активна
  if (!gameActive) {
    breatheLed(ledCoinPin);
    unsigned long currentTime = millis();
    if (currentTime - lastAnimationTime > baseDelay) {
      lastAnimationTime = currentTime;
      knightRiderAnimation();  // Анимация "Knight Rider"
    }
  }

  // Проверка нажатия кнопки "Старт"
  if (digitalRead(startButtonPin) == LOW && !startButtonPressed) {
    delay(50);  // Антидребезг
    if (digitalRead(startButtonPin) == LOW) {
      startButtonPressed = true;
      startButtonPressTime = millis();  // Запоминаем время нажатия
    }
  }

  // Если кнопка была нажата, отсчитываем 20 секунд для сброса системы
  if (startButtonPressed && millis() - startButtonPressTime > 20000) {
    resetSystem();  // Сброс системы через 20 секунд
    startButtonPressed = false;
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

    // Если прошло больше 4 минут, сбрасываем систему
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
  digitalWrite(RELAY_1_PIN, LOW);  // Включаем реле 1
  digitalWrite(RELAY_2_PIN, LOW);  // Включаем реле 2
  digitalWrite(RELAY_3_PIN, LOW);  // Включаем реле 3
  digitalWrite(RELAY_4_PIN, LOW);  // Включаем реле 4
}

// Функция для анимации "Knight Rider"
void knightRiderAnimation() {
  FastLED.clear();
  leds[pos] = CRGB::Red;
  
  // Соседние светодиоды с пониженной яркостью
  if (pos > 0) leds[pos - 1] = CRGB::Red / 5;
  if (pos < NUM_LEDS - 1) leds[pos + 1] = CRGB::Red / 5;

  FastLED.show();

  pos += direction;
  if (pos == 0 || pos == NUM_LEDS - 1) {
    direction = -direction;
  }
}

// Обработка вставки монетки
void handleCoinInsertion() {
  coinCount++;
  gameActive = true;

  blinkCoinThreeTimes(); // Моргание индикатора монетки три раза

  if (coinCount == 1) {
    activateRelayAndLed(relayCoinPin, ledCoinPin);
    activateRelayAndLed(relayPlayer1Pin, ledPlayer1Pin);
  } 
  else if (coinCount >= 2) {
    activateAllRelaysAndLeds();
  }

  timerStart = millis();  // Запускаем таймер на 4 минуты
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
