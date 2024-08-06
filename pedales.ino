const int joystickPinAcelerador = A0; // Joystick para el acelerador
const int joystickPinFreno = A1; // Joystick para el freno

void setup() {
  Serial.begin(9600);
  pinMode(joystickPinAcelerador, INPUT);
  pinMode(joystickPinFreno, INPUT);
}

void loop() {
  int joystickAcelerador = analogRead(joystickPinAcelerador);
  int joystickFreno = analogRead(joystickPinFreno);

  int acelerador = 0;
  int freno = 0;

  // Aumenta el valor del acelerador si el joystick del acelerador se mueve hacia adelante (arriba)
  if (joystickAcelerador < 512) {
    acelerador = map(joystickAcelerador, 512, 0, 0, 1023);
  }

  // Aumenta el valor del freno si el joystick del freno se mueve hacia adelante (arriba)
  if (joystickFreno < 512) {
    freno = map(joystickFreno, 512, 0, 0, 1023);
  }

  // Calcula el porcentaje del acelerador (0-100)
  float aceleradorPct = map(acelerador, 0, 1023, 0, 100) / 100.0;

  // Calcula la velocidad basada en el pedal del acelerador
  int velocidad = map(acelerador, 0, 1023, 0, 200); // Velocidad en km/h

  // Reduce la velocidad en función del pedal del freno
  if (freno > 0) {
    velocidad -= map(freno, 0, 1023, 0, velocidad);
    if (velocidad < 0) {
      velocidad = 0;
    }
  }

  // Calcula los RPM basados en la velocidad
  int rpm = map(velocidad, 0, 200, 600, 8000); // RPM

  // Datos ficticios aleatorios
  float temperatura = 20.0; // Temperatura en grados Celsius
  float oxigenacion = random(15, 21) + random(0, 100) / 100.0; // Oxigenación
  float combustion = random(1, 15) + random(0, 100) / 100.0; // Combustión
  int emisionGases = random(0, 500); // Emisión en ppm
  int consumoElectrico = random(0, 1000); // Consumo eléctrico en watts

  // Aumentar la temperatura si los RPM sobrepasan los 4000
  if (rpm > 4000) {
    temperatura += aceleradorPct * rpm / 100; // Ajusta el factor de incremento según sea necesario
  }

  // Enviar los datos al ESP8266
  String dataToSend = String(rpm) + "," + String(temperatura) + "," + String(oxigenacion) + "," + String(combustion) + "," + String(emisionGases) + "," + String(consumoElectrico) + "," + String(velocidad);
  Serial.println(dataToSend);

  delay(1200);
}
