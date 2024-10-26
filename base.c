#define RELAY_1_PIN 2       // Пин для реле 1
#define RELAY_2_PIN 3       // Пин для реле 2
#define RELAY_3_PIN 12      // Пин для реле 3
#define RELAY_4_PIN 13      // Пин для реле 4

void setup() {
  // Настраиваем пины реле как выходы
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);
  pinMode(RELAY_4_PIN, OUTPUT);

  // Устанавливаем реле в исходное состояние (выключено)
  digitalWrite(RELAY_1_PIN, HIGH);
  digitalWrite(RELAY_2_PIN, HIGH);
  digitalWrite(RELAY_3_PIN, HIGH);
  digitalWrite(RELAY_4_PIN, HIGH);

  // Начальная задержка в 2 секунды
  delay(2000);

  // Включаем реле одно за другим с задержкой в 1 секунду
  digitalWrite(RELAY_1_PIN, LOW);  // Включаем реле 1
  delay(1000);
  digitalWrite(RELAY_2_PIN, LOW);  // Включаем реле 2
  delay(1000);
  digitalWrite(RELAY_3_PIN, LOW);  // Включаем реле 3
  delay(1000);
  digitalWrite(RELAY_4_PIN, LOW);  // Включаем реле 4
}

void loop() {
  // Основной цикл остается пустым, так как реле включаются только при старте
}
