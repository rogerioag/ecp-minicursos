#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

//**************************************************
// Function Prototypes
//**************************************************

static void task(void *args); // Função genérica para ambas as tasks (Task 1 e Task 2)

//**************************************************
// Globals
//**************************************************

static const char TAG[] = "mutex_example";

// Handle do Mutex que será usado para proteger o recurso compartilhado.
static SemaphoreHandle_t s_mutex_handler = NULL;

// Variável compartilhada (recurso crítico) que será acessada por ambas as tasks.
static int s_shared_counter = 0;

//**************************************************
// Public Functions
//**************************************************

void app_main()
{
  // Cria o Mutex
  if ((s_mutex_handler = xSemaphoreCreateMutex()) == NULL)

  {
    ESP_LOGE(TAG, "%s:Fail to create mutex", __func__);
    return;
  }
  ESP_LOGI(TAG, "Mutex created successfully.");

  // Cria a Task Um
  int *id_one = malloc(sizeof(int)); // Aloca memória para o ID da Task
  if (id_one == NULL)

  {
    ESP_LOGE(TAG, "%s:Fail to alloc", __func__);
    return;
  }

  // Define o ID como 1
  *id_one = 1;

  // Cria a task "task_one" com Prioridade 1
  if (xTaskCreate(task, "task_one", 2048, (void *)id_one, 1, NULL) != pdPASS)

  {
    ESP_LOGE(TAG, "%s:Fail to create task one", __func__);
    return;
  }

  // Cria a Task Dois
  int *id_two = malloc(sizeof(int)); // Aloca memória para o ID da Task
  if (id_two == NULL)

  {
    ESP_LOGE(TAG, "%s:Fail to alloc", __func__);
    return;
  }

  // Define o ID como 2
  *id_two = 2;

  // Cria a task "task_two" com Prioridade 1
  if (xTaskCreate(task, "task_two", 2048, (void *)id_two, 1, NULL) != pdPASS)

  {
    ESP_LOGE(TAG, "%s:Fail to create task two", __func__);
    return;
  }
}

//**************************************************
// Static Functions
//**************************************************

// Função executada por ambas as tasks
static void task(void *args)
{
  int id = *(int *)args; // Recupera o ID da task (1 ou 2)

  while (1)

  {
    // Tenta "tomar" (adquirir) o Mutex
    // O timeout é de 250ms. Se o Mutex estiver ocupado por outra task,
    // esta task espera por no máximo 250ms.
    if (xSemaphoreTake(s_mutex_handler, pdMS_TO_TICKS(250)) != pdTRUE)

    {
      ESP_LOGE(TAG, "[%d] %s:Fail to take mutex (Timeout)", id, __func__);
      continue; // Tenta novamente no próximo loop
    }

    // --- INÍCIO DA REGIÃO CRÍTICA (Protegida pelo Mutex) ---

    // Modifica o recurso compartilhado (s_shared_counter)
    s_shared_counter++;

    ESP_LOGI(TAG, "[%d] %s:Shared counter:%d", id, __func__, s_shared_counter);

    // --- FIM DA REGIÃO CRÍTICA ---

    // Libera o Mutex
    // Permite que outra task (esperando por ele) possa adquiri-lo.
    xSemaphoreGive(s_mutex_handler);

    // Simula um processamento longo antes de tentar acessar o recurso novamente
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  // Esta linha nunca será alcançada em um loop infinito, mas é boa prática
  vTaskDelete(NULL);
}