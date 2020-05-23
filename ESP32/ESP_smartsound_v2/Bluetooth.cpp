#include "Bluetooth.h"

const char* Bluetooth::TAG = "Bluetooth device";
Bluetooth* Bluetooth::this_ = nullptr;

Bluetooth::Bluetooth(Ringbuffer<uint8_t>* writebuffer)
{
  //this_ will be the newest BT object. Only one should be created.
  Bluetooth::this_ = this;

  //Bluetooth doesnt use any readbuffer
  bool res;
  setReadBuffer(nullptr);
  res = setWriteBuffer(writebuffer);
  std::cout << "Bluetooth setWriteBuffer=" << res << std::endl;

}

esp_err_t Bluetooth::Bluetooth_a2dp_sink_init(const char* device_name)
{
  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set(TAG, ESP_LOG_DEBUG);

  //Initializing and starting Bluetooth service by given configations
  //Configures bt-controller. Enables controller. Init/Enables bluedroid.
  //Initialises controller as A2DP-sink, sets stream type to AUDIO_STREAM_READER.

  // Making sure Bluetooth low energy is not enabled
  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

  // Configures and enables the bluetooth controller, see function at esp_bt.h
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
  ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
  // Enable bluedroid
  ESP_ERROR_CHECK(esp_bluedroid_init());
  ESP_ERROR_CHECK(esp_bluedroid_enable());
  // Set name of discoverable Bluetooth device
  esp_bt_dev_set_device_name(device_name);
  // Setting up a2dp sink, passing reference to callbacks
  esp_a2d_sink_init();
  esp_a2d_sink_register_data_callback(&Bluetooth::bt_a2d_sink_data_cb);
  esp_a2d_register_callback(&Bluetooth::bt_a2d_sink_cb);

  // Discoverable/connectable bluetooth device
  esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);

  std::cout << "Bluetooth setup done" << std::endl;

  return ESP_OK;
}

void Bluetooth::bt_a2d_sink_data_cb(const uint8_t *data, uint32_t bytes)
{

  if(bytes > 0) {
    gpio_set_level(GPIO_NUM_15, 1);
    // Write data to ringbuffer
    this_->write(data, bytes);
    // Signal next element to start processing
    this_->getWriteBuffer()->GiveBinarySemaphore();
  }
}

void Bluetooth::bt_a2d_sink_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
  ESP_LOGI(TAG, "a2d callback");
  /*
  // Getting a2d sample rate from bluedroid SBC codec
  esp_a2d_cb_param_t* a2d_param = (esp_a2d_cb_param_t*)(param);
  esp_a2d_cb_event_t a2d_event = (esp_a2d_cb_event_t)(event);
  uint8_t sbc0 = a2d_param->audio_cfg.mcc.cie.sbc[0];

  if (a2d_event == ESP_A2D_CONNECTION_STATE_EVT) std::cout << "New device connected, setting samplerate\n";
  std::cout << "Event: " << a2d_event << "\n";
  if (sbc0 & 0x80) ESP_LOGI(TAG, "fs=16000");
  if (sbc0 & 0x40) ESP_LOGI(TAG, "fs=32000");
  if (sbc0 & 0x20) ESP_LOGI(TAG, "fs=44100");
  if (sbc0 & 0x10) ESP_LOGI(TAG, "fs=48000");
  */
}

Bluetooth::~Bluetooth()
{
  //Disabling bluetooth
  esp_a2d_sink_deinit();
  esp_bluedroid_disable();
  esp_bluedroid_deinit();
  esp_bt_controller_disable();
  esp_bt_controller_deinit();
  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
  if(Bluetooth::this_ == this)
    Bluetooth::this_ = nullptr;
}
