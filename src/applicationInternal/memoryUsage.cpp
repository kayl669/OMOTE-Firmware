#include "applicationInternal/memoryUsage.h"
#include "applicationInternal/hardware/hardwarePresenter.h"
#include "applicationInternal/omote_log.h"

static unsigned long updateSerialLogTimer = 0;

void doLogMemoryUsage() {
  omote_log_v("inside doLogMemoryUsage\r\n");
  unsigned long systemHeapSize;
  unsigned long freeSystemHeap;
  unsigned long maxAllocSystemHeap;
  unsigned long minfreeSystemHeap;
  get_heapUsage(&systemHeapSize, &freeSystemHeap, &maxAllocSystemHeap, &minfreeSystemHeap);

  int thresholdForESP32HeapFreeWarning;  // in bytes free in heap
  #if (ENABLE_WIFI_AND_MQTT == 1) && (ENABLE_BLUETOOTH == 1)
    thresholdForESP32HeapFreeWarning = 15000;
  #elif ENABLE_WIFI_AND_MQTT == 1
    thresholdForESP32HeapFreeWarning = 11000;
  #elif ENABLE_BLUETOOTH == 1
    thresholdForESP32HeapFreeWarning = 5000;
  #elif ENABLE_WIFI_AND_MQTT == 1
    thresholdForESP32HeapFreeWarning = 5000;
  #endif
  bool doESPHeapWarning = (freeSystemHeap < thresholdForESP32HeapFreeWarning);

  #if defined(SHOW_LOG_ON_SERIAL)
  // Serial log every 5 sec
  if(millis() - updateSerialLogTimer >= 5000) {
    omote_log_v("inside doLogMemoryUsage: will do serial log\r\n");
    updateSerialLogTimer = millis();

    if (doESPHeapWarning) {
      omote_log_w("WARNING: ESP heap is getting low. You might encounter weird behaviour of your OMOTE, especially when using WiFi and/or BLE.\r\n");
    }
    omote_log_d(
      "ESP32 heap:  size: %6lu, used: %6lu (%2.0f%%), free: %6lu (%2.0f%%), heapMax:  %6lu, heapMin:      %6lu\r\n",
      systemHeapSize,
      systemHeapSize - freeSystemHeap, float(systemHeapSize - freeSystemHeap) / systemHeapSize * 100,
      freeSystemHeap,                     float(freeSystemHeap)                     / systemHeapSize * 100,
      maxAllocSystemHeap, minfreeSystemHeap);

  } else {
    omote_log_v("inside doLogMemoryUsage: serial log skipped\r\n");
  }
  #endif
}
