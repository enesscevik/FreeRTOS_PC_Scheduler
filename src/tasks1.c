#include "scheduler.h"

/*
 * FreeRTOS Worker Task fonksiyonu.
 * Bu fonksiyon, simülasyonun yönettiği her bir iş görevini temsil eder.
 * Görevlendirici tarafından başlatıldığında işlemci zamanını tüketir
 * ve durdurulduğunda beklemede kalır.
 *
 * pvParameters: FreeRTOS tarafından göreve geçirilen parametre.
 * Bu simülasyonda TaskParams* tipindedir ancak doğrudan kullanılmaz
 * çünkü görevden bağımsız işlemci tüketimi simüle edilir.
 */
void generic_task(void* pvParameters) {
    // pvParameters: Aslında TaskParams* tipinde bir parametre alırız.
    // Ancak, bu worker task'lar sadece işlemci tüketimini simüle ettiği için,
    // kendi ID'lerini veya diğer bilgilerini kullanmalarına gerek yoktur.
    // Tüm loglama ve durum yönetimi Scheduler (simulation_task) tarafından yapılır.
    // TaskParams* params = (TaskParams*)pvParameters; // Eğer ihtiyaç olsaydı böyle kullanılabilirdi.

    for (;;) {
        // Bu sonsuz döngü, görevin sürekli olarak işlemci kaynaklarını kullandığını simüle eder.
        // FreeRTOS Scheduler (simulation_task) bu görevi 'vTaskSuspend()' ile durdurana kadar
        // döngü çalışmaya devam eder ve işlemciyi meşgul eder.
    }
}
