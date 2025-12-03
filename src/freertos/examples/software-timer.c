#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"

//**************************************************
// Defines
//**************************************************

#define WARMING_UP_MS_TIME 1000 * 5 // Tempo de Pré-aquecimento: 5 segundos (5000ms)
#define BAKING_MS_TIME 1000 * 10    // Tempo de Cozimento/Assar: 10 segundos (10000ms)

//**************************************************
// Enums
//**************************************************

// Define os possíveis estados da máquina de estados do forno
typedef enum
{
  OVEN_STATE_START = 0,  // Estado inicial
  OVEN_STATE_WARMING_UP, // Estado de pré-aquecimento
  OVEN_STATE_BAKING,     // Estado de cozimento
  OVEN_STATE_DONE,       // Estado final (concluído)
} oven_state_t;

//**************************************************
// Globals
//**************************************************

static const char TAG[] = "oven_example";

static TaskHandle_t s_oven_task_handle = NULL; // Handle (identificador) da task principal do forno. Usado para notificar a task.

//**************************************************
// Function Prototypes
//**************************************************

static void oven_task(void *_);               // Função principal da task que gerencia os estados do forno
static void oven_timer(TimerHandle_t xTimer); // Função de callback do Timer (chamada quando o timer expira)
static void get_temperature();                // Função simulada para ler a temperatura
static void set_heat_element();               // Função simulada para controlar a resistência (elemento de aquecimento)

//**************************************************
// Public Functions
//**************************************************

void app_main()
{
  // Cria a task principal do forno, onde a lógica de estado é executada
  if (xTaskCreate(oven_task,           // Função a ser executada pela task
                  "oven_task",         // Nome amigável da task
                  2048,                // Tamanho da pilha (stack) em bytes
                  NULL,                // Parâmetros a serem passados para a função (nenhum)
                  1,                   // Prioridade da task (ajustável)
                  &s_oven_task_handle) // Handle onde a referência da task será armazenada
      != pdPASS)
  {
    ESP_LOGE(TAG, "%s:Fail to create task", __func__); // Log de erro se a criação falhar
    return;
  }
}

//**************************************************
// Static Functions
//**************************************************

// Task principal do forno
static void oven_task(void *_)
{
  oven_state_t current_state = OVEN_STATE_START; // Inicializa o estado com START

  while (1) // Loop infinito (padrão de tasks do FreeRTOS)
  {
    // Máquina de estados: a lógica muda conforme o estado atual
    switch (current_state)
    {
    case OVEN_STATE_START:
      ESP_LOGI(TAG, "%s:Start", __func__);
      current_state = OVEN_STATE_WARMING_UP; // Transiciona para o próximo estado
      break;

    case OVEN_STATE_WARMING_UP:
    {
      ESP_LOGI(TAG, "%s:Warming Up", __func__);

      // Cria um timer one-shot (pdFALSE) que dispara após 5s
      TimerHandle_t warming_up_timer = xTimerCreate(
          "oven_timer",                      // Nome do Timer
          pdMS_TO_TICKS(WARMING_UP_MS_TIME), // Período do Timer (converte ms para Ticks do FreeRTOS)
          pdFALSE,                           // Tipo: pdFALSE = one-shot (dispara uma vez)
          NULL,                              // ID do Timer (não usado neste exemplo)
          oven_timer);                       // Função de callback a ser chamada

      // Inicia o timer. portMAX_DELAY aguarda indefinidamente se o queue do timer estiver cheio (deve sempre funcionar)
      BaseType_t res = xTimerStart(warming_up_timer, portMAX_DELAY);
      if (res != pdTRUE)
      {
        ESP_LOGE(TAG, "%s:Fail to start timer", __func__);
      }

      while (1) // Loop interno para monitorar o pré-aquecimento
      {
        // Espera notificação do timer. O `pdTRUE` limpa o contador de notificações após receber.
        // O timeout de 500ms permite que a task saia da espera periodicamente para fazer outras ações.
        if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)))
        {
          ESP_LOGI(TAG, "%s:Warming up done", __func__);
          current_state = OVEN_STATE_BAKING; // O timer disparou, muda para o estado de cozimento

          // Apaga o timer depois de usar para liberar memória
          res = xTimerDelete(warming_up_timer, portMAX_DELAY);
          if (res != pdTRUE)
          {
            ESP_LOGE(TAG, "%s:Fail to delete timer", __func__);
          }
          break; // Sai do loop interno e volta para o switch principal
        }

        // Ações executadas a cada 500ms enquanto espera o timer
        get_temperature();  // Simula leitura de temperatura (ação de controle)
        set_heat_element(); // Simula controle da resistência (ação de controle)
      }
    }
    break;

    case OVEN_STATE_BAKING:
    {
      ESP_LOGI(TAG, "%s:Baking", __func__);

      // Cria um timer one-shot que dispara após 10s (lógica similar ao WARMING_UP)
      TimerHandle_t baking_timer = xTimerCreate(
          "oven_timer",
          pdMS_TO_TICKS(BAKING_MS_TIME),
          pdFALSE, // one-shot
          NULL,
          oven_timer);

      // Inicia o timer
      BaseType_t res = xTimerStart(baking_timer, portMAX_DELAY);
      if (res != pdTRUE)
      {
        ESP_LOGE(TAG, "%s:Fail to start timer", __func__);
      }

      while (1) // Loop interno para monitorar o cozimento
      {
        // Espera notificação do timer (timeout de 500ms para controle)
        if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)))
        {
          ESP_LOGI(TAG, "%s:Baking done", __func__);
          current_state = OVEN_STATE_DONE; // O timer disparou, muda para o estado DONE

          // Apaga o timer para liberar memória
          res = xTimerDelete(baking_timer, portMAX_DELAY);
          if (res != pdTRUE)
          {
            ESP_LOGE(TAG, "%s:Fail to delete timer", __func__);
          }
          break; // Sai do loop interno
        }

        // Ações executadas a cada 500ms enquanto espera o timer
        get_temperature();
        set_heat_element();
      }
    }
    break;

    case OVEN_STATE_DONE:
      ESP_LOGI(TAG, "%s:Done", __func__);
      vTaskDelete(NULL); // Encerra a task principal (passando NULL deleta a task atual)
      return;            // O return é importante para sair da função após deletar a task

    default:
      break;
    }
  }

  vTaskDelete(NULL); // Linha de segurança (teoricamente inalcançável se o DONE for atingido)
}

// Callback chamado quando o timer (criado no WARMING_UP ou BAKING) expira
static void oven_timer(TimerHandle_t xTimer)
{
  // Verifica se o handle da task principal existe (deve ser != NULL)
  if (s_oven_task_handle != NULL)
  {
    // Notifica a task principal do forno (s_oven_task_handle).
    // Isso tira a task da função ulTaskNotifyTake() e permite que ela continue.
    xTaskNotifyGive(s_oven_task_handle);
  }
}

// Funções simuladas para controle de hardware
static void get_temperature()
{
  // Simulação: aqui seria a leitura de um sensor de temperatura (ex: termopar/termistor)
  ESP_LOGI(TAG, "%s:Read temperature", __func__);
}

static void set_heat_element()
{
  // Simulação: aqui seria o acionamento de um relé/triac para ligar/desligar a resistência
  ESP_LOGI(TAG, "%s:Set heat element", __func__);
}