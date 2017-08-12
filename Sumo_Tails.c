/*
 Código de controle do robô de sumô Tails de 500g da equipe FEG-Robótica
 Data de início do código: 10/05/2015
 Data da última atualização: 01/06/2015
 Desenvolvedor:         Silas label_address ()()
 */

#include <18F2431.h> //Informa qual o PIC utilizado

#device adc=10  //Informa qual o range do conversor analógico-digital através do número de bits no chip conversor analógico-digital

#fuses HS, NOWDT, NOPROTECT, NOBROWNOUT, PUT, NOLVP //Fusíveis para configuração de uso do PIC

#define POWER_PWM_PERIOD 2000

#use delay(clock=20000000) //Estabelece que o cristal é de 20MHz para calibragem do clock


/*
//HS -> Cristal de alta velocidade
//NOWDT -> Watch Dog desligado (protege contra travamento como em laços infinitos, por exemplo)
//NOPROTECT -> Código não protegido contra cópias
//NOBROWNOUT -> Sem reset por Brownout em caso de baixa tensão (inferior à 4V)
//PUT -> Com power-up (retardo de 72ms após o Reset para evitar ruídos na alimentação quando o PIC for ligado)
//NOLVP -> Desabilita programação em baixa tensão no pino PGM, garantindo-o como entrada e saída convencional
*/

int16 leitura; //Variável genérica que armazena a leitura de sensores infravermelho ou de linha
int8 linhaBits = 0; //Inicialmente carrega o valor 0x00 que equivale a não ter acionado nenhum sensor de linha
int8 infraBits = 0; //Inicialmente carrega o valor 0x00 que equivale a não ter encontrado nenhum adversário
char lado; //Variável que define de qual lado está o nosso oponente

//Início das declarações de funções que serão usadas na main() do programa

void mover_motor1(int8 velocidade, char sentido) // Função que move os dois motores em um sentido arbitrário
{

switch(sentido)
{
case 'f': output_high(PIN_C5); output_low(PIN_C4); break;
case 'a': output_low(PIN_C5); output_high(PIN_C4); break;
 //Define juntamente com o pino C5 o sentido que o motor1 irá girar, neste caso ele envia nível lógico alto para a saída do pino C4
 //Define juntamente com o pino C4 o sentido que o motor1 irá girar, neste caso ele envia nível lógico baixo para a saída do pino C5
}
set_power_pwm0_duty((int16)((POWER_PWM_PERIOD *4) * (1 -(velocidade*0.01)))); // Define o ciclo de trabalho da onda PWM, onde a variável velocidade indica a porcentagem da potência máxima que irá para o motor

}

void mover_motor2(int8 velocidade, char sentido) // Função que move os dois motores em um sentido arbitrário
{
switch(sentido)
{
case 'a': output_high(PIN_C6); output_low(PIN_C7); break;
case 'f': output_low(PIN_C6); output_high(PIN_C7); break;
}
 //Define juntamente com o pino C7 o sentido que o motor2 irá girar, neste caso ele envia nível lógico baixo para a saída do pino C6
//Define juntamente com o pino C6 o sentido que o motor2 irá girar, neste caso ele envia nível lógico alto para a saída do pino C7
set_power_pwm2_duty((int16)((POWER_PWM_PERIOD *4) * (1 - (velocidade*0.01)))); // Define o ciclo de trabalho da onda PWM, onde a variável velocidade indica a porcentagem da potência máxima que irá para o motor
}

void parar_motor1()
{
   output_low(PIN_C5); 
   output_low(PIN_C4);
   set_power_pwm0_duty((int16)((POWER_PWM_PERIOD *4) * (0)));
   //Manda nível lógico baixo para as duas portas que definem o sentido do motor, fazendo o mesmo parar
}

void parar_motor2()
{
   output_low(PIN_C6); 
   output_low(PIN_C7);
   set_power_pwm2_duty((int16)((POWER_PWM_PERIOD *4) * (0)));
   //Manda nivel lógico baixo para as duas portas que definem o sentido do motor, fazendo o mesmo parar
}


void  leituraSensorLinha(int8 canal, int8 bit)  //Função que faz a leitura de um determinado sensor de linha, dependendo dos parâmetros da mesma.Nem sempre o canal vai ser o mesmo valor do bit da palavra, dái a escolha de usar sempre 2 parâmetros
{

switch(canal) //Verifica qual o canal selecionado para ativá-lo.
{
     case 0: set_adc_channel(PIN_A0); break; //Prepara o pino A0 para fazer uma conversão A/D
     case 1: set_adc_channel(PIN_A1); break; //Prepara o puno A1 para fazer uma conversão A/D
     case 2: set_adc_channel(PIN_A2); break; //Prepara o pino A2 para fazer uma conversão A/D
     case 3: set_adc_channel(PIN_A3); break; //Prepara o pino A3 para fazer uma conversão A/D
     case 4: set_adc_channel(PIN_A4); break; //Prepara o pino A4 para fazer uma conversão A/D
}
   leitura = read_adc();           //realiza a leitura e conversão analógica-digital
   if (leitura < 400)               //******TESTAR ESTE VALOR*****
      bit_set(linhaBits,bit);      //Estabelece valor 1 para o bit "bit" da palavra de 8 bits linhaBits
   else
      bit_clear(linhaBits,bit); //Estabelece valor 0 para o bit "bit" da palavra de 8 bits linhaBits
}

void sensoresLinha() //Função de leitura dos sensores de linha, é prático ter uma função como essa para agrupar todas as leituras de uma só vez. 
{
   leituraSensorLinha(0,0);//Envio de parâmetros 8 referente ao AN8 do pino B2, e parâmetro 0 para escrita no bit   0 da palavra linhaBits
  // leituraSensorLinha(1,1);//Envio de parâmetros 9 referente ao AN9 do pino B3, e parâmetro 1 para escrita no bit   1 da palavra linhaBits
}

void trataLinha() //Função para o tratamento dos valores dos sensores de linha
{
   switch (linhaBits)
    {                                                                                                  //                                   A0 A1 
      case 0:  parar_motor1(); break;                                                                                               // Nenhum sensor acionado            0  0                                                                                         
      case 1:  mover_motor1(60,'f');/* mover_motor2(60,'a');*/  break;                                 //Sensor Direita acionado            0  1  
      case 2:  mover_motor1(60,'a'); mover_motor2(100,'a');  break;                                 //Sensor Esquerda acionado           1  0  
      case 3:  mover_motor1(100,'a'); mover_motor2(100,'a');  break;                                //Ambos Sensores acionados           1  1  
   default: break;
   }
linhaBits = 0; //Reinicializa a  variável linhaBits
delay_us(10); //Delay ínfimo similar ao do trataInfra
}

void leituraSensorInfra(int8 canal, int8 bit) //Parâmetro com 8 bits (menor compatível)
{
   switch(canal)
   {
     case 0: set_adc_channel(PIN_A0); break; //Prepara o pino A0 para fazer uma conversão A/D
     case 1: set_adc_channel(PIN_A1); break; //Prepara o puno A1 para fazer uma conversão A/D
     case 2: set_adc_channel(PIN_A2); break; //Prepara o pino A2 para fazer uma conversão A/D
     case 3: set_adc_channel(PIN_A3); break; //Prepara o pino A3 para fazer uma conversão A/D
     case 4: set_adc_channel(PIN_A4); break; //Prepara o pino A4 para fazer uma conversão A/D
   }
   delay_us(10);                    //delay de 10 us (valor suscetível a erro devido ao uso da interrupção do Timer1)
                                   //Importante apenas ser um valor ínfimo
   leitura = read_adc();          //Realiza a leitura do canal analógico-digital
   if (leitura < 65)     {       //De acordo com cálculos baseados no datasheet do sensor, 65 seria o valor que representa o limite de algo ser enxergado pelo mesmo
      bit_clear(infraBits,bit);  //Estabelece valor 0 para o bit "canal" da palavra de 8 bits infraBits
   }
   else
      bit_set(infraBits,bit);   //Estabelece valor 1 para o bit "canal" da palavra de 8 bits infraBits
}

void sensoresInfra() //Função de leitura dos sensores de distância
{
   leituraSensorInfra(2,0); //Sensor da Esquerda
   leituraSensorInfra(3,1); //Sensor da Direita
   leituraSensorInfra(4,2); //Sensor Central com tempo de resposta melhor e alcance menor(para compensar a limitação para distâncias curtas dos outros sensores)
}

void trataInfra() //Função para o tratamento dos valores dos sensores infravermelho
{
   switch (infraBits)
   {                                                                                             //                                 A0 A1 A2 
      case 0: 
      if(lado == 'e'){mover_motor1(50,'a');  mover_motor2(50,'f');}
      else mover_motor1(50,'f');  mover_motor2(50,'a');  
      break;                                                                                // Nenhum sensor acionado          0  0  0 robô continua em sua busca
      case 1:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //Sensor Direita acionado            0  0  1  robô vai para frente atacando o adversário
      case 2:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //Sensor Esquerda acionado           0  1  0  robô vai para frente atacando o adversário
      case 3:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //Ambos Sensores acionados           0  1  1  robô vai para frente atacando o adversário
      case 4:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                          //                                  1  0  0
      case 5:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //                                   1  0  1    
      case 6:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                         //                                   1  1  0
      case 7:  mover_motor1(90, 'f'); mover_motor2(90, 'f'); break;                          //                                  1  1  1  
  
   default: break; //Caso ele não encontre nenhuma das possibiliades listadas acima ele faz nada.
   }
   
infraBits = 0x00; //Reinicializa a  variável linhaBits
delay_us(10); //Delay ínfimo similar ao do trataInfra
}
int8 leituraEstrategia() //Função que lê os pinos necessário para definir qual estratégia será utiliada na partida
{
   int8 estrategia;
   
   if(input(PIN_C0)) bit_set(estrategia,0);
   else bit_clear(estrategia,0);
   
   if(input(PIN_C3)) bit_set(estrategia,1);
   else bit_clear(estrategia,1);
   
   return estrategia;
}


void arco(char direcao) //função que executa estratégia de arco
{
if (direcao == 'd'){ mover_motor1(100, 'f'); mover_motor2(60, 'f'); delay_ms(3000);}
else mover_motor1(60,'f'); mover_motor2(100, 'f'); delay_ms(3000);
//define um motor mais rápido que o outro para fazer um arco em torno do oponente 
}

char escolhaLado()
{
  set_adc_channel(PIN_A2);
  delay_us(10);
  leitura = read_adc();          //Realiza a leitura do canal analógico-digital
   if (leitura < 65)     {       //De acordo com cálculos baseados no datasheet do sensor, 65 seria o valor que representa o limite de algo ser enxergado pelo mesmo
      output_high(PIN_C1);       //Acende o led que indica que o lado direito foi selecionado
      return 'd';               //Estabelece valor 0 para o bit "canal" da palavra de 8 bits infraBits
      break;
   }
   else
      output_high(PIN_C2);
      return 'e';   //Estabelece valor 1 para o bit "canal" da palavra de 8 bits infraBits
}

void costas()
{
   mover_motor1(100,'f');
   delay_ms(1000);
   //vai pra frente com toda a velocidade
   
}

// Fim das declarações das funções que serão chamadas na main() do programa

void main() //Função principal do programa que efetivamente é executada quando o robô é ligado
{
/*A função main é dividida em três partes: 
Configurações, onde os pinos são definidos como entrada/saída e os pinos PWM são configurados
Estratégia, é a rotina responsável por ser executada primeiro que é a estratégia inicial do robô
Loop, que é a parte responsável por dar continuidade ao combate lidando com os sensores disponíveis até que o robô adversário seja empurrado para fora
*/
//delay_ms(5000);

//////////CONFIGURAÇÕES//////////////////////////////////////////////
   //Definição das portas como entrada ou saída 
   set_tris_a(0b11111111);       //Port A I/O Configuração das portas como entrada ou saída, isso pode ser chamado novamente no programa caso necessário  
   set_tris_b(0b00000011);       //Port B I/O Configuração das portas como entrada ou saída, isso pode ser chamado novamente no programa caso necessário  
   set_tris_c(0b10010000);       //Port C I/O Configuração das portas como entrada ou saída, isso pode ser chamado novamente no programa caso necessário  
   
   //Configuração dos módulos PWM do PIC18F2431
   setup_power_pwm_pins(PWM_COMPLEMENTARY,PWM_COMPLEMENTARY,PWM_COMPLEMENTARY,PWM_COMPLEMENTARY); // Configura os 3 módulos PWM como não complementares.
   setup_power_pwm(PWM_FREE_RUN, 1, 0, POWER_PWM_PERIOD, 0, 1,30);
   
   //Configuração dos canais ADC
   //setup_adc(ADC_CLOCK_INTERNAL); //Utiliza o mesmo clock do PIC para o conversor A/D
  // setup_adc_ports(NO_ANALOGS); //Configura todas as portas A/D como analógicas, logo o sinal recebido será convertido em um valor analógico de 0 a 1023
    
 //  lado = escolhaLado(); //
   
   //delay_ms(1000); // Espera do robô antes do início da partida (Regra)
   
/////////FIM CONFIGURAÇÕES//////////////////////////////////////////////
   
////////INICIO ESTRATEGIAS////////////////////////////////////////////

/*switch(leituraEstrategia) //switch que escolhe a estratégia para início do round 
{
   case 1: arco(lado); break;
   default: break;
}
*/
////////FIM DAS ESTRATÉGIAS///////////////////////////////////////////
  
//////////LOOP/////////////////////////////////////////
   while(1)
   {
   if(input(pin_A0) == 0){
   mover_motor1(60, 'f');
   mover_motor2(60, 'f');  
   }
   else parar_motor1(); parar_motor2();
   }
  //sensoresLinha();
  //trataLinha();
  // sensoresLinha();
  // trataLinha();
   
   
///////////FIM DO LOOP////////////////////////////////
}


