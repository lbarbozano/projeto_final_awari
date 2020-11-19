/*-------------------------------------
Objetivo: Realizar a leitura e temperatura e umidade de um ambiente e enviar para um banco de dados 

Estrutura do Código:

1- Conexão wifi
2- Conexão online com o servidor do banco de dados MySQL
3- Leitura dos valores obtidos pelo sensor DHT11
4- Envio dos dados para o servidor

Configurações:
Na IDE Arduino, ir em prefências e depois colar no campo de add board o link: 
http://arduino.esp8266.com/stable/package_esp8266com_index.json

Depois é só baixar a placa ESP8266 e iniciar os testes. 

CREATE TABLE iot_caio.temp_umid ( 
  id integer PRIMARY KEY AUTO_INCREMENT, 
  Temperatura float, 
  Umidade float, 
  Horario timestamp 
  );

*/
//-----------------------------------
#include <ESP8266WiFi.h>
#include <dht.h>
#include<MySQL_Connection.h>
#include<MySQL_Cursor.h>


#define DHT11_PIN D0 //define o pino de dados do sensor
dht DHT;             //instância da biblioteca do sensor

//Display OLED
 #include <Wire.h>
 #include "SSD1306Wire.h"
 SSD1306Wire display (0x3c, SDA, SCL);

//Instancia do cliente wifi
WiFiClient client;

//Inicio da conexão com o banco de dados baseado no client wifi
MySQL_Connection  cbd(&client);

//Variáveis que recebem os dados do sensor
float temp, umid;


void conexao(){
  
	IPAddress server_addr;
  
	const char* ssid     = "PATRICIA 2.4G";
	const char* password = "78451225";

	
	 display.clear();
	 display.setTextAlignment(TEXT_ALIGN_CENTER);
	 display.setFont(ArialMT_Plain_16);
	 display.drawString(63, 0, "Conectando a rede");
	 display.setFont(ArialMT_Plain_10);
	 display.drawString(63, 40, String(ssid));
	 display.display();

	WiFi.begin(ssid,password);
  	
	while(WiFi.status() != WL_CONNECTED){
		delay(500);
  }
	Serial.println("conexão feita com suscesso");
 
	 display.clear();
	 display.setTextAlignment(TEXT_ALIGN_CENTER);
	 display.setFont(ArialMT_Plain_16);
	 display.drawString(63, 0, "Conectado a rede");
	 display.setFont(ArialMT_Plain_10);
	 display.drawString(63, 40, String(ssid));
	 display.display();
	 delay(1200);
	
	 display.clear();
	 delay(10);
	 display.setTextAlignment(TEXT_ALIGN_CENTER);
	 display.setFont(ArialMT_Plain_16);
	 display.drawString(63,10,"macAdress: ");
	 display.setFont(ArialMT_Plain_10);
	 display.drawString(63,36,String(WiFi.macAddress()));
	 display.display();
	 delay(2000);

	
	// display.clear();
	// display.setTextAlignment(TEXT_ALIGN_CENTER);
	// display.setFont(ArialMT_Plain_16);
	// display.drawString(63, 0, "IP NodeMCU");
	// display.setFont(ArialMT_Plain_10);
	// display.drawString(63, 40, String(WiFi.localIP()));
	// display.display();
	// delay(1200);
 
	//A conexão não aceita nomes, então esse método busca o IP no site
	WiFi.hostByName("db4free.net",server_addr);
	
	Serial.print("IP do banco de dados");
	Serial.println(server_addr);
	
	Serial.println("Conectando no banco de dados");
	
	// display.clear();
	// display.setTextAlignment(TEXT_ALIGN_CENTER);
	// display.setFont(ArialMT_Plain_16);
	// display.drawString(63, 0, "IP Banco de Dados");
	// display.setFont(ArialMT_Plain_10);
	// display.drawString(63, 40, String(server_addr));
	// display.display();
	// delay(1200);
	
	// display.clear();
	// display.setTextAlignment(TEXT_ALIGN_CENTER);
	// display.setFont(ArialMT_Plain_16);
	// display.drawString(63, 0, "Conectando no BD");
	// display.setFont(ArialMT_Plain_10);
	// display.drawString(63, 40, String(ssid));
	// display.display();
	// delay(1200);
	

	//configuração do BD que está na internet com o arduino
	//end servidor; porta(padrão); usuário; senha
	if(cbd.connect(server_addr,3306,"rodrigosantos","78451225")){
		
		Serial.println("Conectado no banco de dados");
		// display.clear();
		// display.setTextAlignment(TEXT_ALIGN_CENTER);
		// display.setFont(ArialMT_Plain_16);
		// display.drawString(63, 0, "Conectado com sucesso");
		// display.setFont(ArialMT_Plain_10);
		// display.drawString(63, 40, String(ssid));
		// display.display();
		// delay(1200);
	}
	else{
		Serial.println("Erro na conexão");
		// display.clear();
		// display.setTextAlignment(TEXT_ALIGN_CENTER);
		// display.setFont(ArialMT_Plain_16);
		// display.drawString(63, 0, "Erro de Conexao");
		// display.setFont(ArialMT_Plain_10);
		// display.drawString(63, 40, String(ssid));
		// display.display();
		// delay(1200);
	}
}

void cloud_sql(){


  MySQL_Cursor *cur_mem = new MySQL_Cursor(&cbd);

  char INSERT_DATA[]   = "INSERT INTO dbrodrigosantos3.atividadecaio (temperatura,umidade) VALUES ('%s','%s')";
  
  char tempStr[10];
  char umidStr[10];
  char query[128];

  //float temp  = 22.50
  //float umid  = 90 
  dtostrf(temp,5,2,tempStr);
  dtostrf(umid,5,2,umidStr);

  sprintf(query,INSERT_DATA,tempStr,umidStr);
  
  cur_mem->execute(query);

  delete cur_mem;
  delay(2000);
}

void leitura(){
	
	delay(1000);
	int c = DHT.read11(DHT11_PIN);
		  
	if(c == DHTLIB_OK){
		temp = DHT.temperature;
		umid = DHT.humidity;
		
		 display.clear();
		 delay(10);
		 display.setTextAlignment(TEXT_ALIGN_CENTER);
		 display.setFont(ArialMT_Plain_16);
		 display.drawString(60, 0, "House Rodrigo");
		 display.drawString(60, 20, "Temp.: " + String(temp)  + "˚C");
		 display.drawString(60, 40, "Umid.: " + String(umid) + "%");
		 display.display();
	}
	cloud_sql();
  
  

}

void setup(){
  display.init();                     //inicia o display OLED
  display.flipScreenVertically();     //configura a orientacao
  delay(10);
  conexao();
  Serial.begin(9600);
}

void loop(){
  leitura();
  delay(5000);
}
