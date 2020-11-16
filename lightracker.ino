#include <Servo.h> //Inclusão da biblioteca necessária para a manipulação dos servo motores.

Servo servo_horizontal; //Criando uma variável do tipo Servo (oriunda da biblioteca acima) para o servo horizontal.
int servo_h = 0; //Criando uma variável global que será utilizada para armazenar o valor do grau para onde o servo horizontal irá se movimentar.

//Adicionando limite na rotação dos motores, para evitar o direcionamento para um ângulo onde os jumpers conectados possam se desconectar ou causar algum mal contato.
int limite_max_horizontal = 160;
int limite_min_horizontal = 90;

Servo servo_vertical; //Criando uma variável do tipo Servo (oriunda da biblioteca acima) para o servo vertical.
int servo_v = 0; //Criando uma variável global que será utilizada para armazenar o valor do grau para onde o servo vertical irá se movimentar.

int limite_max_vertical = 160;
int limite_min_vertical = 90;

//Criando variáveis que serão utilizadas para atribuir os LDR's aos pinos analógicos do Arduino.
int ldr_superior_esq = 1; // Valor do pino do LDR superior esquerdo
int ldr_superior_dir = 0; // Valor do pino do LDR superior direito
int ldr_inferior_esq = 2; // Valor do pino do LDR inferior esquerdo
int ldr_inferior_dir = 3; // Valor do pino do LDR inferior direito

void setup () 
{
  servo_horizontal.attach(9); // Atribui o servo motor horizontal para o pino digital 9.
  servo_horizontal.write(90); //Configura a posição inicial do motor horizontal para 90 graus.
  
  servo_vertical.attach(10); // Atribui o servo motor vertical para o pino digital 10.
  servo_vertical.write(90); // Configura a posição inicial do motor horizontal para 180 graus.
  Serial.begin(9600);
}

void loop()
{

  // Efetua a leitura do ângulo atual em que os servo motores estão posicionados.
  servo_h = servo_horizontal.read();
  servo_v = servo_vertical.read();
  
  // Grava a leitura individual do valor que está sendo lido por cada LDR em uma variável (passando como parâmetro no analogRead o valor de cada pino que foi setado anteriormente).
  int superior_esq = analogRead(ldr_superior_esq);
  int superior_dir = analogRead(ldr_superior_dir);
  int inferior_esq = analogRead(ldr_inferior_esq);
  int inferior_dir = analogRead(ldr_inferior_dir);
  
  // Cálculo as médias. Para poder definir para qual ângulo os motores devem rotacionar, é feita uma média de cada "parte". Isto é, uma média para cada lado, uma média para a parte superior e outra para a parte inferior.
  int media_superior = (superior_esq + superior_dir)/2; 
  int media_inferior = (inferior_esq + inferior_dir)/2; 
  int media_esquerda = (superior_esq + inferior_esq)/2; 
  int media_direita = (superior_dir + inferior_dir)/2; 
  
  Serial.print("Valor LDR superior esquerdo: ");
  Serial.println(superior_esq);
  Serial.print("Valor LDR superior direito: ");
  Serial.println(superior_dir);
  Serial.print("Valor LDR inferior esquerdo: ");
  Serial.println(inferior_esq);
  Serial.print("Valor LDR inferior direito: ");
  Serial.println(inferior_dir);
  // Trecho utilizado para debug no monitor serial.
  Serial.print("Media superior: ");
  Serial.println(media_superior);
  Serial.print("Media inferior: ");
  Serial.println(media_inferior);
  Serial.print("Media esquerda: ");
  Serial.println(media_esquerda);
  Serial.print("Media direita: ");
  Serial.println(media_direita);
  
  // Soma de todas as médias para definir se o sistema está recebendo uma alta taxa de incidência de luz ou muito baixa. Nesses dois casos, o sistema permanecerá parado, pois estará recebendo o máximo de luz possível ou não luz disponível no ambiente.
  int soma = media_superior + media_inferior + media_esquerda + media_direita;
  Serial.print("Soma das médias: ");
  Serial.println(soma);

  // Condição que irá analisar se a soma das médias (quantidade total de luz) é maior que 2000 e maior que 30. Se ambas condições forem verdadeiras, o sistema continuará seguindo a luz, caso contrário o sistema se manterá estático.
  // O valor 2000 e 30 foram calculados através de leituras manuais no monitor Serial, onde foi possível analisar que quando temos alta incidência de luz nos quatro LDR's, o valor é aproximadamente 2000. E quando não temos luz alguma, a soma sempre é menor que 30.
  
  if(soma < 2000 && soma > 30){
    if (media_superior < media_inferior){ // Caso a parte de baixo (media inferior) receba mais luz do que a parte de cima (media superior)              
      servo_vertical.write(servo_v + 1); // o ângulo do servo vertical se reajustará aumentando um grau, reposicionando para baixo.
      
      if (servo_v > limite_max_vertical){ // Para evitar o deslocamento em lugares que possam dar conflito com os jumpers, caso exceda o ângulo atual do servo vertical, a posição é setada para o limite.
        servo_v = limite_max_vertical;
      }
    }
    else if (media_inferior < media_superior){ // Caso a parte de cima receba mais luz do que a parte de baixo      
      servo_vertical.write(servo_v - 1);     // o ângulo do servo vertical se reajustará diminuindo um grau, reposicionando para cima.
    
      if (servo_v < limite_min_vertical){ // Mesmo princípio do caso acima, caso exceda o limite mínimo, o servo é reposicionado para o limite.
        servo_v = limite_min_vertical; 
      }
    }
    else{
      servo_vertical.write(servo_v); // Caso não entre nas condições acima, o servo vertical se manterá na última posição gravada
    }
  
    if (media_esquerda > media_direita){ // Caso o lado esquerdo receba mais luz do que o lado direito
      servo_horizontal.write(servo_h - 1);  // o ângulo do servo horizontal se reajustará diminuindo um grau, reposicionando para o lado esquerdo.
      
      if (servo_h > limite_max_horizontal){ // Caso exceda o limite máximo configurado para o servo horizontal, o mesmo será reposicionado para o limite.
        servo_h = limite_max_horizontal;
      }
    }
    else if (media_direita > media_esquerda){// Caso o lado direito receba mais luz do que o lado esquerdo
      servo_horizontal.write(servo_h + 1); // o ângulo do servo horizontal se reajustará aumentando um grau, reposicionando para o lado direito.
      
      if (servo_h < limite_min_horizontal) { // Caso exceda o limite mínimo configurado para o servo horizontal, o mesmo será reposicionado para o limite.
        servo_h = limite_min_horizontal;
      }
    }
    else{
      servo_horizontal.write(servo_h); // Caso não entre nas condições acima, o servo horizontal se manterá na última posição gravada
    }
   
    delay(50); 
  }
}
