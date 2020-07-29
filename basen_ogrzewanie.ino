#include <dht11.h>  // biblioteka czujnika wilgotności i temperatury

const int PIN_IN1 = 7;
const int PIN_IN2 = 6;
const int PIN_DHT11 = 5;
const int LED = 13;

dht11 DHT11;
int status_czujnika = 5;
float minTemp = 5.00; // <- (tu zmień) temperatura przy której załączy halogeny do ogrzewania
float maxTemp = 10.00; // <- (tu zmień) temperatura przy której wyłączy ogrzewanie halogenami
int i = 0;
float sredniaTemp = 0;
unsigned long czas_do_pomiaru = 60; // <- (tu zmień)odstęp między pomiarami w [s]
boolean halogen_on = false;
unsigned int t0, t1,tLED;


void setup() {
  // put your setup code here, to run once:

  // ustawiam piny IN1,IN2 do sterowania przekaźnikami
  pinMode(PIN_IN1 , OUTPUT);
  digitalWrite(PIN_IN1 , HIGH);
  pinMode(PIN_IN2 , OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(PIN_IN2 , HIGH);
  digitalWrite(LED, LOW); // nie świeci

  Serial.begin(9600);
  Serial.println("Dzień dobry :-), tu prezent dla Siorki i Waldka pod choinkę 2017");
  Serial.print(" Ogrzewam utrzymując ");
  Serial.print(minTemp);
  Serial.print(" - ");
  Serial.print(maxTemp);
  Serial.println("°C sterując włączaniem 2 halogenów. ");
  Serial.print(" Pomiary co [s]: ");
  Serial.println( czas_do_pomiaru );
  Serial.println(" Cyklicznie: 10 pomiarów, uśredniam temperaturę i wł/wył. halogeny.");
  t0 = millis();
  tLED = millis();  
}

void loop() {
  // put your main code here, to run repeatedly:

t1 = millis();

if ( ( t1 - tLED ) > 2000 )
  {
    tLED = millis();
    
    if ( digitalRead( LED ) == LOW )
      {
        digitalWrite( LED , HIGH ); 
        delay(250);
        digitalWrite( LED , LOW );
      }
      else digitalWrite( LED , LOW );
      
  }

if ( ( t1 - t0 ) > ( czas_do_pomiaru * 1000 ) )
  {
  t0 = millis();
  status_czujnika = DHT11.read( PIN_DHT11 );

  switch ( status_czujnika )
  {
    case DHTLIB_OK:
      Serial.print("Pomiar nr ");
      Serial.print(i + 1);
      Serial.print(" -> temperatura:  ");
      Serial.print(DHT11.temperature);
      Serial.print("°C,");
      Serial.print(" wilgotność:  ");
      Serial.print(DHT11.humidity);
      Serial.println("%");
      sredniaTemp += DHT11.temperature;
      digitalWrite(LED,HIGH);
      delay(2000);  // zapal LED na sekundy po odczycie
      digitalWrite(LED,LOW);
      i++;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.println("Błąd odczytu danych z czujnika");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.println("Przekroczony limit czasu lub błąd");
      break;
    default:
      Serial.println("Nieznany błąd");
      break;
  }

if ( i > 9 ) // sprawdź czy 10ty pomiar
{
  sredniaTemp = sredniaTemp / i;
  Serial.print("Średnia temperatura: ");
  Serial.print(sredniaTemp);
  Serial.println("°C");
  
  if ( sredniaTemp <= minTemp )
  {
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
    Serial.println(" Regulacja: halogeny są włączone ");
  }
  else if ( sredniaTemp >= maxTemp)
  {
    digitalWrite(PIN_IN1, HIGH);
    digitalWrite(PIN_IN2, HIGH);
    Serial.println(" Regulacja: halogeny są wyłączone ");
  }
   i = 0;
   sredniaTemp = 0;
}  
  
//  delay( czas_do_pomiaru * 1000 );
  }
}
