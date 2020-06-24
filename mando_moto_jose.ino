
const int rafaga_pin = 3;    // the pin that the pushbutton is attached to
const int mando_pin = 4;       // the pin that the LED is attached to
const unsigned long tiempo_total_ciclo = 2000; //tiempo para completar las rafagas
const unsigned long tiempo_total_mando = 3000; //tiempo para completar las rafagas
const int rafagas_esperadas = 3;
const int ON = 1;
const int OFF = 0;
const int LOG = 1;

// Variables will change:
int rafagas_consecutivas = 0;
int estado_actual = 0;
int estado_previo = 0;
unsigned long tiempo_ciclo = 0;
unsigned long tiempo_mando_on = 0;
int mando = OFF;
int ciclo_terminado = OFF;
unsigned long my_time;

void setup() {
  // initialize the button pin as a input:
  pinMode(rafaga_pin, INPUT);
  // initialize the LED as an output:
  pinMode(mando_pin, OUTPUT);
  // initialize serial communication:
  Serial.begin(9600);
  Serial.println("Listo!");
}

void loop() {
  estado_actual = digitalRead(rafaga_pin);
  my_time = millis();
  
  if (estado_actual != estado_previo) {
    estado_previo = estado_actual;
    
    if (estado_actual == HIGH) {
      //Hemos pulsado una rafaga
      if (rafagas_consecutivas == 0) {
        logger("Primera rafaga detectada de serie");
        tiempo_ciclo = my_time;
      } else {
        logger((String)"Rafaga " + rafagas_consecutivas + " rafaga detectadas");
      }
      rafagas_consecutivas ++;

      if (rafagas_consecutivas >= rafagas_esperadas){
        ciclo_terminado = ON;
        if (mando == OFF) {
          logger((String)"Encendiendo el mando!"+tiempo_ciclo+" - " + my_time+" - "+ (my_time > (tiempo_ciclo + tiempo_total_ciclo)));
          tiempo_mando_on = my_time;
          mando = ON;
          digitalWrite(mando_pin, HIGH);
        } else {
          logger((String)"Abortando encendido de mando! Rafaga " + rafagas_consecutivas + " detectada");
          logger((String)"El ciclo empezo a: " +tiempo_ciclo+" Ahora es: " + my_time+" Fuera de Ciclo: "+ (my_time > (tiempo_ciclo + tiempo_total_ciclo))+" ciclo_terminado: "+ ciclo_terminado);
          mando = OFF;
          digitalWrite(mando_pin, LOW);
        }
      }
    }
  }

  if (rafagas_consecutivas > 0 && ciclo_terminado == OFF 
    && (my_time > (tiempo_ciclo + tiempo_total_ciclo))) {
    //Se ha terminado el ciclo por tiempo.
    logger((String)"Ciclo abortado por caducidad de tiempo: "+tiempo_ciclo+" - " + my_time);

    rafagas_consecutivas = 0;
  }

  if (ciclo_terminado == ON 
    && my_time > (tiempo_mando_on + tiempo_total_mando)) {
    //Apago el mando
    logger((String)"Apagando mando. Puerta abierta.");
    digitalWrite(mando_pin, LOW);
    mando = OFF;
    ciclo_terminado = OFF;
  }

  //Evitar rebotes
  delay(100);
}

void logger (String msg){
  if (LOG)
    Serial.println(msg);
  }
