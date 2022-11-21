Codigo de Prtueba mejora para reconexion de Nettra
//Necesario cambiar TIMER 0 y 1 por ---> 2 y 3, para evitar conflicto con los tiempos en los contadores de pulsos

//variables para monitorear en el debbug
reserva0=Timer[0];
reserva1=EstadoModem;


if (Timer[0]>1800)// 
{
reserva0=Timer[0];
	
if(EstadoModem == MODEM_OK)
	{	
		if(comunic > 0)
	

	{
			
			comunic=0;
			
			Timer[0]=0;

				
	 
	}else
	{
		Timer[1]=0;
		reserva2=reserva2+1;
		EncenderModem = 0;
		
		reserva1=EstadoModem;
		while(Timer[1] < 100 || EstadoModem == MODEM_APAGADO)
		{
		reserva1=EstadoModem;
		EncenderModem = 1;
		reserva1=EstadoModem;
		}
		Timer[1]=0;
		
		if ( Timer[0] > 500 && comunic <= 0)
			{
			reserva2=0;
			Reset();
			}
		}

}

//VERSION FINAL

if (Timer[0]>1800 )// 
{
	if(EstadoModem == MODEM_OK)
	{	
		if(comunic > 0)
	

	{
			
			comunic=0;
			
			Timer[0]=0;

				
	 
	}else
	{
		Timer[1]=0;
//APAGO MODEM
		EncenderModem = 0;

		while(Timer[1] < 100 || EstadoModem == MODEM_APAGADO)
//Enciendo MODEM me mantengo en el bloque while hasta asegurarme que este prendido el MODEM
		EncenderModem = 1;

//Reinicio el contador
		Timer[1]=0;
		
//RESETEO
		if ( Timer[0] > 2500 && comunic <= 0)
			Reset();
			
	}

}