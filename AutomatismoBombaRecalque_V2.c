//recalque V11

//DEFINICION DE VARIABLES
int comunic = 0;
bool control_por_presion_salida = 0;
bool prender;//para moniobras
bool apagar;//para maniobras
bool apagar_rapido;//para emergencias
bool apagar_rapido_scada;
bool apagar_scada;
int salida_analogica = 0;
int i;
float presion_salida_aux;
alias presion_salida_cruda_int as AInEscalado[0];
float presion_salida_filtrada;

float presion_entrada_aux;
alias presion_entrada_cruda_int as AInEscalado[1];
float presion_entrada_filtrada;

alias encender_variador as Out.0;
alias led_apagado_rapido as Out.1;
alias led_rampa_apagado as Out.2;
alias led_rampa_encendido as Out.3;

alias btn_rampa_encendido as In.1;
alias btn_rampa_apagado as In.2;
alias btn_apagado_rapido as In.3;

//HORARIOS
ulong inicio_rampa_ascendente_viejo;		// Se guarda para no alterar mapa
ulong inicio_rampa_descendente_viejo;		// Se guarda para no alterar mapa
float presion_salida_maxima_del_sistema;
ulong hora_actual;
float frecuencia;

int TemperaturaR10;//si se usa modbus.
int TemperaturaR11;//si se usa modbus.
int CaudalR10;//si se usa modbus.
int CaudalR11;//si se usa modbus.
int TotalizadorR10;//si se usa modbus.
int TotalizadorR11;//si se usa modbus.
int TotalizadorR12;//si se usa modbus.
int TotalizadorR13;//si se usa modbus.
int Bateria1;//si se usa modbus.
int inic_rampa_ascendente;
int inic_rampa_descendente;
bool encender_scada = 0;
alias tension_fuente as AInEscalado[2];
bool falla_ute = 0;
bool apagado_por_falla;
int j;//indice para ciclo for

ulong inicio_rampa_ascendente[7];
ulong inicio_rampa_descendente[7];
EncenderModem=1;
Timer[0] = 0;

while(1)
{
	// chequeo seguridad
	//Para prevenir fallos de memoria y valores que impidan arranque o causen problemas
	if(inicio_rampa_ascendente[DiaSemana]<60000 || 80000<inicio_rampa_ascendente[DiaSemana])
	{
		inicio_rampa_ascendente[DiaSemana]=63000;
	}
	if(inicio_rampa_descendente[DiaSemana]<230000 || 235900<inicio_rampa_descendente[DiaSemana])
	{
		inicio_rampa_descendente[DiaSemana]=233000;
	}
	if (presion_salida_maxima_del_sistema<300 || 400<presion_salida_maxima_del_sistema)
	{
		presion_salida_maxima_del_sistema=350;
	}
	//Fin chequeo seguridad
	inic_rampa_ascendente = inicio_rampa_ascendente[DiaSemana] / 100;		//si se deja con segundos puede pasar que no haga lo que debe
	inic_rampa_descendente = inicio_rampa_descendente[DiaSemana] / 100;		//por estar ocupado en otra cosa tipo delay.

	hora_actual = Horas * 100 + Minutos;
	
	falla_ute = (tension_fuente < 2000);
	
	if (falla_ute == 1)
	{
		apagado_por_falla = 1;
	}
		
	if (control_por_presion_salida != 1)
	{	
		if (presion_salida_cruda_int < 0) 	//si hay valores negativos muestra cero.
		{	
			presion_salida_filtrada = 0;
		}
		else
		{
			presion_salida_filtrada = presion_salida_cruda_int;
		}
		if (presion_entrada_cruda_int < 0)
		{	
			presion_entrada_filtrada = 0;
		}
		else
		{
			presion_entrada_filtrada = presion_entrada_cruda_int;
		}
	}
	
	if ((hora_actual == inic_rampa_ascendente) || btn_rampa_encendido == 1 || encender_scada == 1)
	{
		prender = 1;
		apagar = 0;
		apagar_rapido = 0;
		apagar_scada = 0;
		apagar_rapido_scada = 0;
		control_por_presion_salida = 0;
		Delay(5000);	//a pedido de Pablo
		encender_scada = 0;
	}
	
	if ((hora_actual == inic_rampa_descendente) || apagar_scada == 1 || btn_rampa_apagado == 1)
	{
		prender = 0;
		apagar = 1;
		apagar_rapido = 0;
		apagar_rapido_scada = 0;
		control_por_presion_salida = 0;
	}
	
	if (btn_apagado_rapido == 1 || apagar_rapido_scada == 1 || falla_ute == 1)
	{
		prender = 0;
		apagar = 0;
		apagar_rapido = 1;
		apagar_rapido_scada = 0;
		control_por_presion_salida = 0;
	}

	if (salida_analogica > 4095)	//por si supera las 4095 cuentas
	{
		salida_analogica = 4095;
	}
	
	if (salida_analogica < 0)		//por si quedó en valores negativos
	{
		salida_analogica=0;
	}
	
	AOut[0] = salida_analogica;
				
	//CONTROL POR PRESION
	if (control_por_presion_salida == 1)	//no está en ninguna de las rampas entonces controla por presión.
	{
		led_rampa_apagado = 0;
		led_apagado_rapido = 0;
		led_rampa_encendido = 0;

		if (presion_salida_filtrada > presion_salida_maxima_del_sistema)
		{
			salida_analogica = salida_analogica - 4;
		}
		
		if (presion_salida_filtrada < presion_salida_maxima_del_sistema)
		{
			salida_analogica = salida_analogica + 1;
		}
		
		for (i = 0; i < 500; i = i + 1)	//filtrado de 5 segundos
		{
			presion_salida_aux = ((presion_salida_aux) + presion_salida_cruda_int);
			presion_entrada_aux = ((presion_entrada_aux) + presion_entrada_cruda_int);
			Delay(10);
		}			
		
		if (presion_salida_aux < 0) 	//si hay valores negativos muestra cero.
		{	
			presion_salida_filtrada = 0 ;
		}
		else
		{
			presion_salida_filtrada = (presion_salida_aux / 500);
		}
		
		if (presion_entrada_aux < 0)
		{	
			presion_entrada_filtrada = 0;
		}
		else
		{
			presion_entrada_filtrada = (presion_entrada_aux / 500);
		}
		
		presion_salida_aux = 0;
		presion_entrada_aux = 0;
		i = 0;

		Delay(5000);	//para que se estabilice el sistema hidraulico antes de tomar otra accion de control, junto con el for y los delay
						//de la medida de presion_salida meten en total un retraso de 10 segundos.
	}
	
	//FIN DEL CONTROL POR PRESION

	//APAGADO CON RAMPA RAPIDA
	if (apagar_rapido == 1)// 
	{	
		encender_variador = 1;
		salida_analogica = salida_analogica - 2;
		led_rampa_apagado = 0;
		led_apagado_rapido = 1;
		led_rampa_encendido = 0;

		if (0 >= salida_analogica)
		{
			led_rampa_apagado = 0;
			led_apagado_rapido = 0;
			led_rampa_encendido = 0;
			encender_variador = 0;
			apagar_rapido = 0;
			apagado_por_falla = 0;
		}
		
		Delay(100);		//para ir generando la rampa de bajada rapida.
	}

	if (prender == 1)	// SUBIDA
	{
		encender_variador = 1;
		salida_analogica = salida_analogica + 2;
		control_por_presion_salida = 0;
		led_rampa_apagado = 0;
		led_apagado_rapido = 0;
		led_rampa_encendido = 1;

		if (presion_salida_filtrada >= presion_salida_maxima_del_sistema)	//termina la rampa por presion y pasa a regular.
		{
			led_rampa_apagado = 0;
			led_apagado_rapido = 0;
			led_rampa_encendido = 0;
			prender = 0;
			control_por_presion_salida = 1;
		}
		
		Delay(1000);	//para ir generando la rampa suave de subida
	}
	
	//APAGADO CON RAMPA LENTA	
	if (apagar == 1)	// BAJADA LENTA
	{
		led_rampa_apagado = 1;
		led_apagado_rapido = 0;
		led_rampa_encendido = 0;

		encender_variador = 1;
		salida_analogica = salida_analogica - 2;
		control_por_presion_salida = 0;
		
		if (0 >= salida_analogica)
		{
			led_rampa_apagado = 0;
			led_apagado_rapido = 0;
			led_rampa_encendido = 0;
			apagar = 0;
			encender_variador = 0;
		}
		
		Delay(1000);	//para ir generando la rampa suave de bajada.
	}
	
	//calculo de frecuencia del variador
	if (encender_variador == 1)
	{
		frecuencia = AInEscalado[3]/1000*1475/30;//(26.0 * salida_analogica / 4095) + 24.0;	//variador con frecuencia de arranque 24Hz y tope 50Hz
	}
	else
	{
		frecuencia = 0;
	}
	
	//CHEQUEO DE COMUNICACIONES
	//el SCADA escribe un 1 y el NETTRA espera 3 minutos y lo borra, 
	//si no llega el 1 se resetea el equipo por problemas de comunicaciones.

	if (Timer[0] > 1800)
	{
		if (comunic > 0)
		{
			comunic = 0;
			Timer[0] = 0;
		}
		else
		{
			EncenderModem = 0;
			Delay(1000);
			EncenderModem = 1;
			Timer[0] = 0;
		}
	}

	//CAUDALIMETRO 1 CON MODBUS
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	TemperaturaR10 = TemperR1[1];
	TemperaturaR11 = TemperR1[0];
	CaudalR10 = CaudalR1[1];
	CaudalR11 = CaudalR1[0];
	TotalizadorR10 = TotalizadorR1[1];
	TotalizadorR11 = TotalizadorR1[0];
	TotalizadorR12 = TotalizadorR1[3];
	TotalizadorR13 = TotalizadorR1[2];
	Bateria1 = BateriaR1;

	//******************************************************************************************
	//VARIABLES A LEVANTAR POR MODBUS DE LOS CAUDALIMETROS SIEMENS
	//CaudalR1		offset 3002 largo 2 consulta cada 10 segundos.
	//TotalizadorR1	offset 3017 largo 4 consulta cada 21600 segundos.
	//BateriaR1		offset 3030 largo 1 consulta cada 21600 segundos.
	//TemperR1		offset 3042 largo 2 consulta cada 21600 segundos.
}

 