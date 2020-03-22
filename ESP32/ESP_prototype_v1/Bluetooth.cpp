#include "Bluetooth.h"

const char* Bluetooth::TAG = "Bluetooth device";

Bluetooth::Bluetooth()
{
}

esp_err_t Bluetooth::Bluetooth_a2dp_sink_init(const char* device_name){

  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set(TAG, ESP_LOG_DEBUG);

  //Making sure bt_ble is not enabled
  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

  //Configres and enables the bluetooth controller, see marco at esp_bt.h
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
  ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
  //Enable bluedroid
  ESP_ERROR_CHECK(esp_bluedroid_init());
  ESP_ERROR_CHECK(esp_bluedroid_enable());
  //Set name of bt device
  esp_bt_dev_set_device_name(device_name);
  //Setting up a2dp sink, passing reference to callbacks
  esp_a2d_sink_init();
  esp_a2d_sink_register_data_callback(&Bluetooth::bt_a2d_sink_data_cb);
  esp_a2d_register_callback(&Bluetooth::bt_a2d_sink_cb);

  //Discoverable/connectable bluetooth device
  esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);

  return ESP_OK;
}

void Bluetooth::bt_a2d_sink_data_cb(const uint8_t *data, uint32_t len){

  uint32_t i2s_bytes_written  = len;
  i2s_write(I2S_NUM_0, (char*)data, len, &i2s_bytes_written, 100);
  //ESP_LOGI(TAG, "data rec: %ul", i2s_bytes_written);
}
void Bluetooth::bt_a2d_sink_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param){

  ESP_LOGI(TAG, "cb 2");
}

Bluetooth::~Bluetooth(){
  //Disabling bluetooth
  esp_a2d_sink_deinit();
  esp_bluedroid_disable();
  esp_bluedroid_deinit();
  esp_bt_controller_disable();
  esp_bt_controller_deinit();
  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
}
