//==================================================
#include <EEPROM.h>                                 // Library EEPROM

// Definisikan PIN, agar lebih mudah
#define WATER        A5
#define LDR          A4
#define BUP          A0
#define BDN          A1
#define RLY1         4
#define RLY2         12
#define PWM1         3
#define PWM2         11

#define delayMotor   1000    // Delay 1000 ms untuk nyala motor
#define batasCahaya  30      // Batas persentase Data Cahaya < 30 = Mendung
#define batasHujan   30      // Batas persentase Data Cahaya > 30 = Hujan

#define TUTUP        0
#define BUKA         1


// Deklarasi Variable
int hujan, cahaya, spd, jemuran;
int  toleransi   = 2;             // Toleransi untuk jaga-jaga naik turunya data terlalu cepat.
byte speedMotor  = 100;           // kecepatan awal motor, jika tidak disimpan di EEPROM
String stats;                     // Status Serial Debug     
//==================================================
void setup() {
  Serial.begin(9600);       // Aktifkan Serial Monitoring

  // Ubah mode PIN
  pinMode(BUP, INPUT_PULLUP);
  pinMode(BDN, INPUT_PULLUP);
  pinMode(RLY1, OUTPUT);
  pinMode(RLY2, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
}

//==================================================
void loop() {
  speedMotor = 255;
  cahaya     = 20;
  hujan      = 30;
  spd        = map(speedMotor, 0, 255, 0, 100);                 // 0 s/d 100  persen

  DEBUG();

  if (digitalRead(BUP) == LOW) speedMotor++;                // Tombol + Kecepatan Motor
  if (digitalRead(BDN) == LOW) speedMotor--;                // Tombol - Kecepatan Motor
  EEPROM.write(0, speedMotor);

  // Kondisi CAHAYA = Mendung / Malam dan HUJAN = BASAH
  if (cahaya < (batasCahaya - toleransi) and hujan > (batasHujan + toleransi) and jemuran == BUKA) {
    motorTutup();
    stats = "TUTUP, MENDUNG, BASAH";
  }
  // Kondisi CAHAYA = Siang dan HUJAN = BASAH
  else if (cahaya > (batasCahaya + toleransi) and hujan > (batasHujan + toleransi) and jemuran == BUKA) {
    motorTutup();
    stats = "TUTUP, SIANG, BASAH";
  }
  // Kondisi CAHAYA = Mendung dan HUJAN = KERING
  else if (cahaya < (batasCahaya - toleransi) and hujan < (batasHujan - toleransi) and jemuran == BUKA) {
    motorTutup();
    stats = "TUTUP, MENDUNG, KERING";
  }
  // Kondisi CAHAYA = Siang dan HUJAN = KERING
  else if (cahaya > (batasCahaya + toleransi) and hujan < (batasHujan - toleransi) and jemuran == TUTUP) {
    motorBuka();
    stats = "BUKA, SIANG, KERING";
  }
  delay(100);
}

//==================================================
void DEBUG() {
  Serial.print("CAHAYA : ");
  Serial.print(cahaya);
  Serial.print(" %   , ");
  Serial.print("HUJAN : ");
  Serial.print(hujan);
  Serial.print(" %   , ");
  Serial.print("SPEED : ");
  Serial.print(spd);
  Serial.print(" %   , ");
  Serial.print("STATUS : ");
  Serial.println(stats);
}

//==================================================
void motorBuka() {
  digitalWrite(RLY1, BUKA);            // Relay Aktif = Arah motor Buka
  digitalWrite(RLY2, BUKA);
  analogWrite(PWM1, speedMotor);       // Sinyal PWM untuk kecepatan motor
  analogWrite(PWM2, speedMotor);
  delay(delayMotor);                   // Jeda motor untuk menyala sampai jemuran terbuka
  digitalWrite(RLY1, LOW);             // Matikan Relay
  digitalWrite(RLY2, LOW);
  analogWrite(PWM1, 0);                // Matikan Sinyal PWM = Motor Berhenti
  analogWrite(PWM2, 0);
  jemuran = BUKA;
}

//==================================================
void motorTutup() {
  digitalWrite(RLY1, TUTUP);            // Relay Mati = Arah motor TUTUP
  digitalWrite(RLY2, TUTUP);
  analogWrite(PWM1, speedMotor);       // Sinyal PWM untuk kecepatan motor
  analogWrite(PWM2, speedMotor);
  delay(delayMotor);                   // Jeda motor untuk menyala sampai jemuran terbuka
  digitalWrite(RLY1, LOW);             // Matikan Relay
  digitalWrite(RLY2, LOW);
  analogWrite(PWM1, 0);                // Matikan Sinyal PWM = Motor Berhenti
  analogWrite(PWM2, 0);
  jemuran = TUTUP;
}
