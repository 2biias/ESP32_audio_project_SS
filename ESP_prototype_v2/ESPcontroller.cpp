
#include "ESPcontroller.h"

ESPcontroller::ESPcontroller(){
  //contructor
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

  // Setting mode, sda_io_num, scl_io_num, sda_pullup_en, scl_pullup_en and master.clock_speed
  i2c_config_t i2c_config = {I2C_MODE_MASTER, GPIO_NUM_18, (gpio_pullup_t)true, GPIO_NUM_23, (gpio_pullup_t)1, 100000};

	res |= i2c_param_config(I2C_NUM, &i2c_config);
	res |= i2c_driver_install(I2C_NUM, i2c_config.mode, 0, 0, 0);

	/* mute DAC during setup, power up all systems, slave mode */
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL3, 0x04);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_CONTROL2, 0x50);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_CHIPPOWER, 0x00);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_MASTERMODE, 0x00);

	/* power up DAC and enable only LOUT1 / ROUT1, ADC sample rate = DAC sample rate */
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACPOWER, 0x30);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_CONTROL1, 0x12);

	/* DAC I2S setup: 16 bit word length, I2S format; MCLK / Fs = 256*/
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL1, 0x18);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL2, 0x02);

	/* DAC to output route mixer configuration */
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL16, 0x00);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL17, 0x90);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL20, 0x90);

	/* DAC and ADC use same LRCK, enable MCLK input; output resistance setup */
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL21, 0x80);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL23, 0x00);

	/* DAC volume control: 0dB (maximum, unattenuated)  */
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL5, 0x00);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL4, 0x00);

	/* power down ADC while configuring; volume: +9dB for both channels */
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCPOWER, 0xff);
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL1, 0x33);

	/* select LINPUT2 / RINPUT2 as ADC input; stereo; 16 bit word length, format right-justified, MCLK / Fs = 256 */
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL2, 0x50);
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL3, 0x00);
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL4, 0x0e);
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL5, 0x02);

	/* set ADC volume */
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL8, 0x20);
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCCONTROL9, 0x20);

	/* set LOUT1 / ROUT1 volume: 0dB (unattenuated) */
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL24, 0x1e);
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL25, 0x1e);

	/* power up and enable DAC; power up ADC (no MIC bias) */
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACPOWER, 0x3c);
	res |= WriteCodecI2C(ES8388_ADDR, ES8388_DACCONTROL3, 0x00);
	//res |= WriteCodecI2C(ES8388_ADDR, ES8388_ADCPOWER, 0x09);

	return res;
}

esp_err_t ESPcontroller::I2SInit(){
  i2s_config_t i2s_read_config = {
		.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
		.sample_rate = 44100,
		.bits_per_sample = (i2s_bits_per_sample_t)16,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = I2S_COMM_FORMAT_I2S,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,
		.dma_buf_count = 2,
		.dma_buf_len = 1024,
		.use_apll = 1,
		.tx_desc_auto_clear = 1,
		.fixed_mclk = 0
	};

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

  return ESP_OK;
}

ESPcontroller::~ESPcontroller(){
  //destructor
}
