#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

//**************************************************
// Typedefs
//**************************************************

// Define a estrutura de dados que será enviada/recebida pela fila
typedef struct
{
  int counter; // Um simples contador para rastrear a ordem dos pacotes
} package_t;

//**************************************************
// Function Prototypes
//**************************************************

static void producer_task(void *args);     // Task que envia dados para a fila (Produtor)
static void consumer_one_task(void *args); // Primeira Task que recebe dados da fila (Consumidor)
static void consumer_two_task(void *args); // Segunda Task que recebe dados da fila (Consumidor)

//**************************************************
// Globals
//**************************************************

static const char TAG[] = "queue_example";

static QueueHandle_t s_queue_handler = NULL;

//**************************************************
// Public Functions
//**************************************************

void app_main()
{
  // Cria a Fila
  if ((s_queue_handler = xQueueCreate(10, sizeof(package_t))) == NULL)
  {
    ESP_LOGE(TAG, "%s:Fail to create queue", __func__);
    return;
  }
  ESP_LOGI(TAG, "Queue created successfully.");

  // Cria a Task Produtora
  if (xTaskCreate(producer_task, "producer_task", 2048, NULL, 1, NULL) != pdPASS)
  {
    ESP_LOGE(TAG, "%s:Fail to create producer task", __func__);
    return;
  }

  // Cria a primeira Task Consumidora
  if (xTaskCreate(consumer_one_task, "consumer_one_task", 2048, NULL, 1, NULL) != pdPASS)
  {
    ESP_LOGE(TAG, "%s:Fail to create consumer one task", __func__);
    return;
  }

  // Cria a segunda Task Consumidora
  if (xTaskCreate(consumer_two_task, "consumer_two_task", 2048, NULL, 1, NULL) != pdPASS)
  {
    ESP_LOGE(TAG, "%s:Fail to create consumer two task", __func__);
    return;
  }
}

//**************************************************
// Static Functions
//**************************************************

// Task Produtora: Envia um pacote de dados para a fila periodicamente
static void producer_task(void *args)
{
  // Inicializa o pacote de dados
  package_t package = {
      .counter = 0,
  };

  while (1)
  {
    // Tenta enviar o pacote para a fila.
    if (xQueueSend(s_queue_handler, &package, pdMS_TO_TICKS(250)) != pdTRUE)
    {
      ESP_LOGE(TAG, "%s:Fail to send package to queue (Queue Full)", __func__);
      continue; // Se falhar, tenta novamente no próximo loop
    }

    ESP_LOGI(TAG, "%s:Sent counter:%d", __func__, package.counter);

    package.counter++;

    // Atraso de 500ms antes de enviar o próximo item.
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  // Esta linha nunca será alcançada em um loop infinito, mas é boa prática
  vTaskDelete(NULL);
}

// Task Consumidora 1: Recebe dados da fila
static void consumer_one_task(void *args)
{

  while (1)
  {
    package_t package; // Variável para armazenar o dado recebido

    // Tenta receber um item da fila.
    // O timeout é portMAX_DELAY, o que significa que a task ficará
    // BLOQUEADA INDEFINIDAMENTE esperando por dados na fila.
    // Isso garante que o Consumidor só consuma ciclos de CPU quando houver dados.
    if (xQueueReceive(s_queue_handler, &package, portMAX_DELAY) != pdTRUE)
    {
      // Se xQueueReceive retornar false com portMAX_DELAY, algo está muito errado,
      // mas a lógica simplesmente continua para a próxima iteração.
      continue;
    }

    ESP_LOGI(TAG, "%s:Received counter:%d", __func__, package.counter);

    // Simula um processamento longo, atrasando por 1 segundo.
    // A outra task Consumidora (consumer_two_task) terá a chance de receber o próximo item.
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  vTaskDelete(NULL);
}

// Task Consumidora 2: Recebe dados da fila (IDÊNTICA à Consumer 1)
static void consumer_two_task(void *args)
{

  while (1)
  {
    package_t package;

    // Recebe o item da fila (bloqueia indefinidamente até que haja dados)
    if (xQueueReceive(s_queue_handler, &package, portMAX_DELAY) != pdTRUE)
    {
      continue;
    }

    ESP_LOGI(TAG, "%s:Received counter:%d", __func__, package.counter);

    // Simula um processamento longo.
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  vTaskDelete(NULL);
}