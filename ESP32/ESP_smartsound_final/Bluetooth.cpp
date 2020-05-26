#include "Bluetooth.h"

const char* Bluetooth::TAG = "ESP Bluetooth";
Bluetooth* Bluetooth::this_ = nullptr;

Bluetooth::Bluetooth(Ringbuffer<uint8_t>* writebuffer)
{
  // this_ will be the newest BT object. Only one should be created.
  Bluetooth::this_ = this;

  //Bluetooth doesnt use any readbuffer
  bool res;
  setReadBuffer(nullptr);
  res = setWriteBuffer(writebuffer);
  std::cout << "Bluetooth setWriteBuffer=" << res << std::endl;
}

esp_err_t Bluetooth::Bluetooth_a2dp_sink_init(const char* device_name)
{
  /*
  Initializing and starting Bluetooth service by given configations
  Configures bt-controller. Enables controller. Init/Enables bluedroid.
  Initialises controller as A2DP-sink, sets stream type to AUDIO_STREAM_READER.
  */

  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set(TAG, ESP_LOG_DEBUG);

  // Making sure Bluetooth low energy is not enabled
  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

  // Configures and enables the bluetooth controller, see function at esp_bt.h
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
  ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));

  // Enable bluedroid (running on pro core)
  ESP_ERROR_CHECK(esp_bluedroid_init());
  ESP_ERROR_CHECK(esp_bluedroid_enable());

  // Set name of discoverable Bluetooth device
  esp_bt_dev_set_device_name(device_name);

  // Setting up a2dp sink, passing reference to user defined callbacks
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
    // Write data to ringbuffer
    gpio_set_level(GPIO_NUM_15, 1);
    this_->write(data, bytes);
    // Signal next element to start processing. Buffer up 3 packets before processing
    if(this_->prebuffered > 2){
      this_->getWriteBuffer()->GiveBinarySemaphore();
    } else {
      // Increase prebuffered packets
      this_->prebuffered++;
    }
  }
}

void Bluetooth::bt_a2d_sink_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{

  // Getting event type and passed parameter
  esp_a2d_cb_param_t* a2d_param = (esp_a2d_cb_param_t*)(param);
  esp_a2d_cb_event_t a2d_event = (esp_a2d_cb_event_t)(event);

  // Handle and dispatch bluetooth event
  switch (a2d_event) {
    // Connection event
    case ESP_A2D_CONNECTION_STATE_EVT: {
      if (a2d_param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        ESP_LOGI(TAG, "Device disconnected");
        esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);

      } else if (a2d_param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED){
          ESP_LOGI(TAG, "Device connected");
          esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_NONE);
          // Reset prebuffering
          this_->prebuffered = 0;
      }
      break;
    }
    // Audio codec configures as A2DP sink
    case ESP_A2D_AUDIO_CFG_EVT: {
      // Getting SBC codec information from bluedroid
      uint8_t sbc0 = a2d_param->audio_cfg.mcc.cie.sbc[0];
      uint8_t sbc1 = a2d_param->audio_cfg.mcc.cie.sbc[1];
      uint8_t sbc2 = a2d_param->audio_cfg.mcc.cie.sbc[2];
      uint8_t sbc3 = a2d_param->audio_cfg.mcc.cie.sbc[3];

      // Treasure : https://osdn.net/projects/android-x86/scm/git/system-bt/commits/a8f34abbeaee6b0765184f2baa2b5992ddec95c5

      uint32_t samplerate = 16000;
      if (sbc0 & 0x80) {
        samplerate = 16000;
      } else if (sbc0 & 0x40) {
        samplerate = 32000;
      } else if (sbc0 & 0x20) {
        samplerate = 44100;
      } else if (sbc0 & 0x10) {
        samplerate = 48000;
      }

      char* chmode = nullptr;
      if (sbc0 & 0x08) {
        //ESP_LOGI(TAG, "Channel mode: mono");
        chmode = (char*)(malloc(strlen("Mono") + 1));
        strcpy(chmode, "Mono");
      } else if (sbc0 & 0x04) {
        chmode = (char*)malloc(strlen("Dual") + 1);
        strcpy(chmode, "Dual");
      } else if (sbc0 & 0x02) {
        chmode = (char*)malloc(strlen("Stereo") + 1);
        strcpy(chmode, "Stereo");
      } else if (sbc0 & 0x01) {
        chmode = (char*)malloc(strlen("Joint Stereo") + 1);
        strcpy(chmode, "Joint Stereo");
      }

      uint8_t blocks = 0;
      if (sbc1 & 0x80) {
        blocks = 4;
      } else if (sbc1 & 0x40) {
        blocks = 8;
      } else if (sbc1 & 0x20) {
        blocks = 12;
      } else if (sbc1 & 0x10) {
        blocks = 16;
      }

      uint8_t subbands = 0;
      if (sbc1 & 0x08) {
        subbands = 4;
      } else if (sbc1 & 0x04) {
        subbands = 8;
      }

      char* allocation = nullptr;
      if (sbc1 & 0x02) {
        //ESP_LOGI(TAG, "Allocation: SNR");
        allocation = (char*)malloc(strlen("SNR") + 1);
        strcpy(allocation, "SNR");
      } else if (sbc1 & 0x01) {
        allocation = (char*)malloc(strlen("loudness") + 1);
        strcpy(allocation, "loudness");
      }
      uint8_t bitpool_min = sbc2;
      uint8_t bitpool_max = sbc3;

      ESP_LOGI(TAG, "SBC codec: Samplerate: %i, Channel mode: %s, Blocks: %i, Subbands: %i, Allocation: %s, Bitpool min: %i Bitpool max: %i"
                    ,samplerate, chmode, blocks, subbands, allocation, bitpool_min, bitpool_max);

      this_->setSampleRate(samplerate);

      free(chmode);
      free(allocation);

      break;
    }

    // Audio transmission state changed event
    case ESP_A2D_AUDIO_STATE_EVT: {
      if (a2d_param->audio_stat.state == ESP_A2D_AUDIO_STATE_STOPPED) {
        ESP_LOGI(TAG, "Stream stopped");

      } else if(a2d_param->audio_stat.state == ESP_A2D_AUDIO_STATE_STARTED) {
        ESP_LOGI(TAG, "Stream started");
        // Reset prebuffering count
        this_->prebuffered = 0;
      }
      break;
    }
    // Couldn't handle event
    default:
      ESP_LOGI(TAG, "Bluetooth event unhandled");
      break;
  }
}

void Bluetooth::setSampleRate(uint32_t samplerate) {
  std::cout << "Setting i2s samplerate: " << samplerate << std::endl;
  i2s_stop(I2S_NUM);
  i2s_set_clk(I2S_NUM, samplerate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
  i2s_start(I2S_NUM);
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
