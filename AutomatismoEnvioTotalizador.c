/*****
Cambiar dos variables de reserva tipo int. smsEnviado y dia. 
*****/



//Envio de Totalizador por SMS
//capturo el dia 
dia = Dia;

//control de dia y estado del envio 
if (dia == 1 && smsEnviado == 0 ) 
	{
		smsEnviado = EnviarSMS(0,0,TotalizadorR1);
		//tiempo de espera para el envio del sms
		//Formazomos el estado de la variable sms para que no envie mas mensjaes 
		smsEnviado = 1;
		Delay(15000);

	}
if (dia == 30)
	//reset del envio para el primer dia del mes. 
	smsEnviado = 0;
	