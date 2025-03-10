// prove per diminuire rumore di quantizzazione

//ho applicato un filtro FIIR  di ordine n, in base a quello calcolo lo span al 99%(span = 6*sqrt(var) = max-min).
// calcolo quindi il valore del treshold in base all'ordine del filtro e viceversa.
// ovviamente span minore => ordine maggiore filtro => frequenza di campionamento effettiva minore


//tabella t obsv = 60'
// n|span
// 1|536
// 2|460
// 4|285
// 8|174
//16|118
//32|70
//64|45
//128|30
//256|17
//512|11
// l'opzione migliore è dunque n = 64  con un treshold di 100



#define n 1024
float k;
float y, Sum;
float minimo = 65535;
float massimo = 0;
float span;


void setup() {
  Serial.begin(9600);
  k = (n+1);
  Sum=0;
}

void loop() {
  analogReadResolution(16);
  for (int i = 0; i < n+1; i++){
    Sum += analogRead(A0);
  }
  y = Sum/k;
  Sum = 0;
  if (y > massimo ){massimo = y;}
  if (y < minimo ){minimo = y;}
  span = massimo-minimo;

  Serial.println(span);

  

}
