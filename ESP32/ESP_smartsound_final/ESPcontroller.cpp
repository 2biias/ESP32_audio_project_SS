
#include "ESPcontroller.h"

ESPcontroller::ESPcontroller(){
  //constructor
}

esp_err_t ESPcontroller::InitFlash(){

  //Flash storage is initialised before starting blueooth controller
  esp_err_t err = nvs_flash_init();
  if(err == ESP_ERR_NVS_NO_FREE_PAGES){
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  return err;
}

esp_err_t ESPcontroller::WriteCodecI2C(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t data){

  esp_err_t res = ESP_OK;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	res |= i2c_master_start(cmd);
	res |= i2c_master_write_byte(cmd, SlaveAddress, 1 /*ACK_CHECK_EN*/);
	res |= i2c_master_write_byte(cmd, RegisterAddress, 1 /*ACK_CHECK_EN*/);
	res |= i2c_master_write_byte(cmd, data, 1 /*ACK_CHECK_EN*/);
	res |= i2c_master_stop(cmd);
	res |= i2c_master_cmd_begin((i2c_port_t)0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return res;
}

esp_err_t ESPcontroller::CodecInit(){

  esp_err_t res = ESP_OK;

  // Enabling pin 15 for debugging
  gpio_reset_pin(GPIO_NUM_15);
  gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT);

  // Setting mode, sda_io_num, scl_io_num, sda_pullup_en, scl_pullup_en and master.clock_speed
  i2c_config_t i2c_config = {I2C_MODE_MASTER, GPIO_NUM_18, (gpio_pullup_t)true, GPIO_NUM_23, (gpio_pullup_t)1, 100000};

	res |= i2c_param_config(I2C_NUM, &i2c_config);
	res |= i2c_driver_install(I2C_NUM, i2c_config.mode, 0, 0, 0);


  /* Power up and basic setup */

  // Muting DAC in setup
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL3, 0b00000100);
  // Low power enable
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_CONTROL2, 0b01010000);
  //Power up all by default
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_CHIPPOWER, 0x00);
  // Setting Codec as I2S slave, default config
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_MASTERMODE, 0x00);

  /* DAC setup */

  //disable DAC and disable Lout/Rout/1/2 when setting up DAC controls
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACPOWER, 0xC0);
  //ADC Fs same as DAC Fs, 100kOhm divider
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_CONTROL1, 0b00010010);

  //Order of data, data timing (mode A), 16-bit resolution, DSP/PCM mode.
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL1, 0b00011000); // I2S seriel
  //Normal speed, MCLK to sampling frequency ratio = 256
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL2, 0b00000010);
  // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL16, 0x09);
  //Connects Left DAC to Left Mixer, LIN gain 0dB 0b10010000
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL17, 0b10010000);
	//Connects Right DAC to Right Mixer, RIN gain 0dB 0b10010000
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL20, 0b10010000);
  //Internal ADC and DAC use the same LRCK clock, ADC LRCK as internal LRCK
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL21, 0b10000000);
  //Left DAC volume 0dB
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL4, 0b00000000);
	//Right DAC volume 0dB
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL5, 0b00000000);
  //Left ADC volume 0dB
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL8, 0b00000000);
  //Left ADC volume 0dB
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL9, 0b00000000);
  //Turn DAC on, enable L/ROUT1 and L/ROUT2
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACPOWER, 0b00111110);

  /* ADC setup */

  // Both ADC power down, microphone power down.
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCPOWER, 0xFF);
  // MIC Left and Right channel PGA gain
  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL1, 0xbb);
  // LINPUT2 & RINPUT2 as input. Differential = ch1
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL2, 0b01010000);
  // Stereo. Differential = ch1
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL3, 0b00000010);
  //Sets ADC to sample I2S PCM DSP mode
  //res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL4, 0b00001111); // DSP/PCM
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL4, 0b00001101); //Seriel left justified
  // Normal speed, MCLK to sampling frequency ratio = 256
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL5, 0b00000010);
  // Power on ADC, Enable LIN&RIN, Power off MICBIAS, set int1lp to low power mode
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCPOWER, 0b00001001);

  /* Adjusting volume LOUT1/ROUT1 0dB */
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL24, 0b00011110);
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL25, 0b00011110);

  /* Starting DAC to run in decode mode */
  //enable dac
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL21, 0x80);
  // Power up and line out. Enable all outputs
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACPOWER, 0b00111100);
  // DAC unmute
  res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL3, 0b00000000);

  std::cout << "ES8388 setup done" << std::endl;
	return res;
}

esp_err_t ESPcontroller::I2SInit() {

  i2s_config_t i2s_read_config = {
		.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
		.sample_rate = 48000, //
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, //16 bits, signed
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB), //Data format MSB (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB)
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL2, // default interrupt priority
		.dma_buf_count = 32, // number of DMA buffer
		.dma_buf_len = 256, // length of DMA buffers
		.use_apll = 0, // using accurate clock (APLL) as main I2S clock
		.tx_desc_auto_clear = 1, // Clear tx descriptors in underflow conditions
		.fixed_mclk = 0 // non fixed MCLK
	};

  //do something
	i2s_pin_config_t i2s_read_pin_config = {
		.bck_io_num = GPIO_NUM_5,
		.ws_io_num = GPIO_NUM_25,
		.data_out_num = GPIO_NUM_26,
		.data_in_num = GPIO_NUM_35
	};

	i2s_driver_install(I2S_NUM, &i2s_read_config, 0, NULL);
	i2s_set_pin(I2S_NUM, &i2s_read_pin_config);

	/*******************/

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
	WRITE_PERI_REG(PIN_CTRL, 0xFFF0);

  std::cout << "I2S setup done" << std::endl;

  return ESP_OK;
}

ESPcontroller::~ESPcontroller(){
  //destructor
}
