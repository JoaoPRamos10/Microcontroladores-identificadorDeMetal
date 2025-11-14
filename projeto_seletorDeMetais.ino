/*
 * CÓDIGO PARA LIXEIRA SELETIVA AUTOMATIZADA
 * * Este código implementa a lógica descrita no README.md para separar
 * resíduos em duas categorias: METAL e NÃO-METAL.
 * * Componentes:
 * - Arduino Uno
 * - Sensor Ultrassônico (HC-SR04)
 * - Sensor Indutivo (CHE12-4N)
 * - Motor de Passo (28BYJ-48) + Driver (ULN2003)
 * * Lógica de Funcionamento:
 * 1. ESPERAR: Monitora o sensor HC-SR04.
 * 2. DETECTAR: Se a distância for menor que o normal, um objeto caiu.
 * 3. AGUARDAR: Espera um tempo fixo (CALIBRATION_DELAY_MS) para o objeto chegar nos sensores.
 * 4. LER: Lê o sensor indutivo.
 * 5. DECIDIR: 
 * - Se for metal, move o motor para a Posição B.
 * - Se não for metal, o motor fica na Posição A (padrão).
 * 6. RESETAR: Após um tempo (SORT_DELAY_MS), move o motor de volta para a Posição A.
 */

// --------------------------------------------------------------------
// BIBLIOTECAS
// --------------------------------------------------------------------
#include <Stepper.h> // Biblioteca para o motor de passo

// --------------------------------------------------------------------
// CONFIGURAÇÕES E PINOS (AJUSTE ESTES VALORES)
// --------------------------------------------------------------------

// --- Sensor Ultrassônico (HC-SR04) ---
#define TRIG_PIN 9
#define ECHO_PIN 10

// Distância (em cm) que o sensor lê quando o túnel está vazio.
// Se ler MENOS que isso, significa que um objeto passou.
#define TRIGGER_DISTANCE_CM 15 

// --- Sensor Indutivo (CHE12-4N) ---
#define INDUCTIVE_PIN 7

// ATENÇÃO: Configure o estado de detecção do seu sensor.
// - Se o sensor envia LOW (0V) ao detectar metal (comum em NPN), use LOW.
// - Se o sensor envia HIGH (5V) ao detectar metal (comum em PNP), use HIGH.
#define METAL_DETECTED_STATE LOW 

// --- Motor de Passo (28BYJ-48 com Driver ULN2003) ---
#define STEPS_PER_REV 2048 // Nº de passos para uma volta completa (comum para 28BYJ-48)
#define MOTOR_PIN_1 2
#define MOTOR_PIN_2 3
#define MOTOR_PIN_3 4
#define MOTOR_PIN_4 5

// Nº de passos para mover a rampa da Posição A (Não-Metal) para B (Metal).
// 512 = 90 graus (2048 / 4)
#define STEPS_TO_METAL 512 

// --- Calibração de Tempo ---
// (Passo 3) Tempo (ms) que o objeto leva para cair do HC-SR04 até o sensor indutivo.
// VOCÊ PRECISARÁ CALIBRAR ISSO EXPERIMENTALMENTE.
#define CALIBRATION_DELAY_MS 100 

// (Passo 6) Tempo (ms) para esperar o objeto cair no compartimento antes de resetar a rampa.
#define SORT_DELAY_MS 1500 

// --------------------------------------------------------------------
// INICIALIZAÇÃO DOS OBJETOS
// --------------------------------------------------------------------

// Inicializa o motor de passo (na sequência 1-3-2-4 para a biblioteca Stepper.h)
Stepper myStepper(STEPS_PER_REV, MOTOR_PIN_1, MOTOR_PIN_3, MOTOR_PIN_2, MOTOR_PIN_4);

// --------------------------------------------------------------------
// FUNÇÃO SETUP (Executa uma vez)
// --------------------------------------------------------------------
void setup() {
  Serial.begin(9600); // Inicia o monitor serial para debugging
  Serial.println("Lixeira Seletiva Iniciada.");

  // Configura pinos do HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Configura pino do Sensor Indutivo
  // Usamos INPUT_PULLUP para garantir um estado estável (HIGH) se o sensor
  // for do tipo NPN (open-collector), que é o mais comum.
  pinMode(INDUCTIVE_PIN, INPUT_PULLUP);

  // Configura pinos do motor como saída (necessário para desenergizar)
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_PIN_3, OUTPUT);
  pinMode(MOTOR_PIN_4, OUTPUT);

  // Configura a velocidade do motor (RPM - 10-15 é seguro para o 28BYJ-48)
  myStepper.setSpeed(12);

  Serial.println("Sistema pronto. Aguardando objeto...");
}

// --------------------------------------------------------------------
// FUNÇÃO LOOP (Executa continuamente)
// --------------------------------------------------------------------
void loop() {
  
  // Passo 1: Monitorar o sensor HC-SR04
  long distance = getDistance();

  // Passo 2: Detectar o objeto
  // Se a distância for menor que o "gatilho", um objeto entrou.
  if (distance < TRIGGER_DISTANCE_CM && distance > 0) {
    
    Serial.println("-------------------------");
    Serial.print("Objeto detectado! Distancia: ");
    Serial.println(distance);

    // Passo 3: Aguardar (Delay de Calibração)
    // Espera o objeto cair até a estação de identificação
    delay(CALIBRATION_DELAY_MS);

    // Passo 4: Ler Sensor Indutivo
    int metalState = digitalRead(INDUCTIVE_PIN);

    // Passo 5: Tomar Decisão
    if (metalState == METAL_DETECTED_STATE) {
      
      // É METAL
      Serial.println("--> RESULTADO: METAL");
      Serial.println("Movendo motor para Posicao B (Metal)...");
      moveMotor(STEPS_TO_METAL);

    } else {
      
      // NÃO É METAL
      Serial.println("--> RESULTADO: NAO-METAL");
      Serial.println("Mantendo motor na Posicao A (Nao-Metal).");
      // Não fazemos nada, pois o motor já está na posição padrão A.
    }

    // Passo 6: Resetar
    Serial.println("Aguardando objeto cair...");
    delay(SORT_DELAY_MS); // Espera o objeto cair no compartimento

    // Se o motor foi movido (era metal), move de volta
    if (metalState == METAL_DETECTED_STATE) {
      Serial.println("Resetando motor para Posicao A...");
      moveMotor(-STEPS_TO_METAL); // Move o número de passos ao contrário
    }

    Serial.println("Sistema pronto. Aguardando proximo objeto...");
    Serial.println("-------------------------");
    
    // Pequena pausa para evitar leitura dupla do mesmo objeto
    delay(500); 
  }
}

// --------------------------------------------------------------------
// FUNÇÕES AUXILIARES
// --------------------------------------------------------------------

/**
 * @brief Mede a distância usando o sensor HC-SR04.
 * @return A distância em centímetros (cm).
 */
long getDistance() {
  // Limpa o pino de trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Envia um pulso de 10 microsegundos
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Lê o tempo de retorno do pulso
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // Converte o tempo em distância (cm)
  // Fórmula: Distância = (Tempo * Velocidade do Som) / 2
  long distance = duration * 0.034 / 2;
  
  return distance;
}


/**
 * @brief Move o motor de passo e depois desliga as bobinas.
 * Desligar as bobinas (desenergizar) é CRUCIAL para evitar que
 * o motor esquente e gaste energia desnecessariamente.
 * @param steps O número de passos a mover (positivo ou negativo).
 */
void moveMotor(int steps) {
  myStepper.step(steps); // Move o motor
  
  // Desliga todas as bobinas para economizar energia e evitar aquecimento
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  digitalWrite(MOTOR_PIN_3, LOW);
  digitalWrite(MOTOR_PIN_4, LOW);
}
