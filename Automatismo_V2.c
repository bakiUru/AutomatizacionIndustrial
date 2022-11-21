// CONSTANTES -------------------------------------------------------
const V_SCRIPT 	= 200928;
const CONSIGNA_POR_DEFECTO = 20;
const COEF_HIST_ALTA_MAX = 1.1;
const COEF_HIST_ALTA = 1.06;
const COEF_HIST_BAJA = 0.94;
const COEF_HIST_BAJA_MIN = 0.90;

// ALIAS ------------------------------------------------------------
// Entradas digitales
alias caudalimetro0 as In.0;
alias caudalimetro1 as In.1; 
alias flotador 		as In.2;

// Entradas analógicas
alias medidaPSalida0 	as AInEscalado[0];
alias medidaPEntrada0 	as AInEscalado[1];
alias medidaPSalida1 	as AInEscalado[2];
alias medidaPEntrada1 	as AInEscalado[3];

// Salidas digtales
alias apagarPiloto0 as Out.0;
alias bajarPresion0 as Out.1;
alias subirPresion0 as Out.2;
alias apagarPiloto1 as Out.3; 	
alias bajarPresion1 as Out.4; 
alias subirPresion1 as Out.5;
       
               
// VARIABLES --------------------------------------------------------
attribute uint modoFuncionamiento[2];	// 0-Escalonado, 1-Modo Piloto, 2-Cierre total, 3-Apertura Segura, 4-Apertura total
attribute uint caudalimetro[2];			// 0-Sin caudalimetro, 1-Caudalimetro por pulsos, 2-Caudalimetro MODBUS
attribute float setpoint[8];
attribute uint tLog[2], tAlarmas[2], horarios[12], pMinimaEntrada[2], pMinimaServ[2], pMaximaServ[2], caudalMaximo[2], caudalMinimo1[2], caudalMinimo2[2], kCavit[2], litrosPorPulso[2], prescaler[2];
attribute ulong vScript;
attribute long habAlarmas[2];

loggable float presionEntrada[2], presionSalida[2], caudal[2], volumenAgua[2], acumuladoLocal[2], consignaActual[2], bateria[2];
loggable long alarmas[2];

uint horaActual, i, errorEsclavos[2], modoFuncAux[2];
float caudalMinimo, Totalizador[2], Caudal[2];
ulong timerReporte[2], tAlarmasAux;
long copiaAlarmas[2]; 


// INICIALIZACION DE VARIABLES --------------------------------------
// Primera ejecucion o perdida de RAM
if (InicializarVariables)
{	
	vScript = V_SCRIPT;
	Log(vScript);
	for (i=0;i<8;i=i+1)
	{
		setpoint[i] = 20;
	}
	for(i=0;i<2;i=i+1)
	{
		modoFuncionamiento[i] = 0; 	// Escalonado
		caudalimetro[i] = 2; 		// Modbus
		tLog[i] = 300; 				// 5 minutos [seg]
		horarios[0+i*6] = 2100;
		horarios[1+i*6] = 2200;
		horarios[2+i*6] = 2300;
		horarios[3+i*6] = 500;
		horarios[4+i*6] = 600;
		horarios[5+i*6] = 700;
		pMaximaServ[i] = 50;
		prescaler[i] = 2;
		litrosPorPulso[i] = 100;
		Pulsos[i] = 0;
		alarmas[i] = 0;
		caudal[i] = 0;
		acumuladoLocal[i] = 0;
		volumenAgua[i] = 0;
		bateria[i] = 0;
	}
	InicializarVariables = 0;
}

Delay(20000); // Inicialización sensores (20 seg)
timerReporte[0] = ConfigurarTimeout(10000);
timerReporte[1] = ConfigurarTimeout(10000);

// CODIGO -----------------------------------------------------------
while(1)
{	
	for(i=0;i<2;i=i+1)
	{
		tAlarmasAux = tAlarmas[i] * 1000;
		
		// TOMO DATOS DE CAUDAL ------------------------------------------
		if (caudalimetro[i] == 1)
		{
			// Caudalímetro por pulsos
			caudal[i] = (CalcularCaudal(i, 1, litrosPorPulso[i], prescaler[i]))*3.6; // 3.6 para pasar de l/s a m2/h
			volumenAgua[i] = volumenAgua[i] + (litrosPorPulso[i] * Pulsos[i]);
			acumuladoLocal[i] = acumuladoLocal[i] + (litrosPorPulso[i] * Pulsos[i]);
			Pulsos[i] = 0;
		}
		else if (caudalimetro[i] == 2)
		{
			// Asigno valores que no puedo según i
			errorEsclavos[0] = ErrorEsclavos.0;
			errorEsclavos[1] = ErrorEsclavos.1;
			
			// Caudalímetro Modbus
			alarmas[i].11 = Alarma(11, errorEsclavos[i] == 1, tAlarmasAux);
			if (errorEsclavos[i] != 1)
			{
				Totalizador[0] = Totalizador0;
				Totalizador[1] = Totalizador1;
				Caudal[0] = Caudal0;
				Caudal[1] = Caudal1;
				bateria[0] = Bateria0;
				bateria[1] = Bateria1;
				caudal[i] = Caudal[i];
				if(acumuladoLocal[i] == 0) // Para cuando se reinicia el totalizador o para que la primera vez volumen agua no loguee cualquier cosa
					acumuladoLocal[i] = Totalizador[i];
				volumenAgua[i] = volumenAgua[i] + (Totalizador[i] - acumuladoLocal[i]);
				acumuladoLocal[i] = Totalizador[i];
			}
		}
		
		// TOMO DATOS DE PRESIONES ---------------------------------------
		presionEntrada[i] = AInEscalado[i*3]*0.1; 
		if (presionEntrada[i] < 0)
			presionEntrada[i] = 0;
		presionSalida[i] = AInEscalado[i*3+1]*0.1; 
		if (presionSalida[i] < 0) 
			presionSalida[i] = 0;

		// REGULACIÓN ----------------------------------------------------
		// MODO ESCALONADO O APERTURA SEGURA
		if (modoFuncionamiento[i] == 0 || modoFuncionamiento[i] == 3) 
		{
			// Determinación de la consigna actual por horarios
			horaActual = Horas*100 + Minutos;

			if (Intervalo(horaActual, horarios[5+i*6], horarios[0+i*6])) // Setpoint 1 entre [T6-T1]
				consignaActual[i] = setpoint[0+i*4];
			else if (Intervalo(horaActual, horarios[0+i*6], horarios[1+i*6]) || Intervalo(horaActual, horarios[4+i*6], horarios[5+i*6])) // Setpoint 2 entre [T1-T2] y entre [T5-T6]
				consignaActual[i] = setpoint[1+i*4];
			else if (Intervalo(horaActual, horarios[1+i*6], horarios[2+i*6]) || Intervalo(horaActual, horarios[3+i*6], horarios[4+i*6])) // Setpoint 3 entre [T2-T3] y entre [T4-T5]
				consignaActual[i] = setpoint[2+i*4];
			else if (Intervalo(horaActual, horarios[2+i*6], horarios[3+i*6])) // Setpoint 4 entre [T3-T4]
				consignaActual[i] = setpoint[3+i*4];
			else
				consignaActual[i] = CONSIGNA_POR_DEFECTO; // Backup por las dudas que no funcione horario
			
			if (consignaActual[i] > pMaximaServ[i]*0.9 || (modoFuncionamiento[i] == 3))
				consignaActual[i] = pMaximaServ[i]*0.9;
			
			// Desactivo piloto antes de empezar a regular
			if(i == 0)
				apagarPiloto0 = 1;	
			else
				apagarPiloto1 = 1;	
			
			// Bajo presion si esta alta
			if (presionSalida[i] >= consignaActual[i]*COEF_HIST_ALTA_MAX)
			{
				if (i == 0)
					{subirPresion0 = 0; bajarPresion0 = 1;}
				else
					{subirPresion1 = 0; bajarPresion1 = 1;}
			}
			// Subo presion si esta baja
			else if (presionSalida[i] <= consignaActual[i]*COEF_HIST_BAJA_MIN )
			{
				if (i == 0)
					{bajarPresion0 = 0; subirPresion0 = 1;}
				else
					{bajarPresion1 = 0; subirPresion1 = 1;}
			}
			// Cerca de la consigna no hago nada
			else if (consignaActual[i]*COEF_HIST_BAJA <= presionSalida[i] && presionSalida[i] <= consignaActual[i]*COEF_HIST_ALTA)
			{
				if (i == 0)
					{bajarPresion0 = 0; subirPresion0 = 0;}
				else
					{bajarPresion1 = 0; subirPresion1 = 0;}
			}
		}
		// CONTROL POR PILOTO
		else if (modoFuncionamiento[i] == 1) 
		{
			if (i == 0)
				{bajarPresion0 = 0; subirPresion0 = 0; apagarPiloto0 = 0;}
			else
				{bajarPresion1 = 0; subirPresion1 = 0; apagarPiloto1 = 0;}
		}
		// CIERRE FORZADO
		else if (modoFuncionamiento[i] == 2)
		{
			if (i == 0)
				{apagarPiloto0 = 1; subirPresion0 = 0; bajarPresion0 = ((caudal[i] > 0)&&(presionSalida[i] > 3));}
			else
				{apagarPiloto1 = 1; subirPresion1 = 0; bajarPresion1 = ((caudal[i] > 0)&&(presionSalida[i] > 3));}
		}
		// APERTURA TOTAL
		else if (modoFuncionamiento[i] == 4)
		{
			if (i == 0)
				{apagarPiloto0 = 1; bajarPresion0 = 0; subirPresion0 = (presionSalida[i] <= presionEntrada[i]*0.95);}
			else
				{apagarPiloto1 = 1; bajarPresion1 = 0; subirPresion1 = (presionSalida[i] <= presionEntrada[i]*0.95);}
		}

		// ALARMAS -------------------------------------------------------
		// Problema de abduccion
		alarmas[i].0 = Alarma(0,(presionEntrada[i] < pMinimaEntrada[i]) && (caudal[i] < caudalMaximo[i]), tAlarmasAux);
		// Cavitación
		alarmas[i].1 = Alarma(1,(presionEntrada[i] >= kCavit[i] * presionSalida[i]) && (caudal[i] < caudalMaximo[i]), tAlarmasAux);
		// Baja presión de entrada
		alarmas[i].2 = Alarma(2,(presionEntrada[i] < consignaActual[i]) && (presionSalida[i] < consignaActual[i]), tAlarmasAux);
		// Presion salida mayor a la de entrada
		alarmas[i].3 = Alarma(3, presionEntrada[i] < presionSalida[i], tAlarmasAux);
		// Posible rotura de caño
		alarmas[i].4 = Alarma(4, caudal[i] > caudalMaximo[i], tAlarmasAux);
		// Prevision de camara inundada
		alarmas[i].5 = Alarma(5, flotador == 1, tAlarmasAux);
		// Desobedece consigna hacia abajo
		alarmas[i].6 = Alarma(6, presionSalida[i] >= 1.2*consignaActual[i], tAlarmasAux);
		// Desobedece consigna hacia arriba
		alarmas[i].7 = Alarma(7, (presionSalida[i] <= 0.8*consignaActual[i]) && (presionEntrada[i] > consignaActual[i]), tAlarmasAux);
		// Cierre de valvula reguladora
		alarmas[i].8 = Alarma(8, (presionSalida[i] < 3) && (presionEntrada[i] > pMinimaEntrada[i]), tAlarmasAux);
		// Calculo caudal mínimo
		if (consignaActual[i] == setpoint[0+i*4])
			caudalMinimo = caudalMinimo1[i];
		else
			caudalMinimo = (caudalMinimo1[i]-caudalMinimo2[i])/((horarios[5+i*6] - horarios[0+i*6])*(horarios[5+i*6] - horarios[0+i*6]))*((2*horaActual- horarios[5+i*6] - horarios[0+i*6])*(2*horaActual- horarios[5+i*6] - horarios[0+i*6])) + caudalMinimo2[i];
		// Caudal muy bajo 
		alarmas[i].9 = Alarma(9, caudal[i] < caudalMinimo, tAlarmasAux);
		// Sector abierto
		alarmas[i].10 = Alarma(10, (caudal[i] < 1) && (presionSalida[i] > 1.2*consignaActual[i]), tAlarmasAux);
		
		alarmas[i] = alarmas[i] & habAlarmas[i];
		
		// REPORTES -----------------------------------------------------
		// MODOS
		if (modoFuncionamiento[i] != modoFuncAux[i])
		{
			modoFuncAux[i] = modoFuncionamiento[i];
			Report(modoFuncionamiento[i]);
		}	
		// ALARMAS
		if (copiaAlarmas[i] != alarmas[i])
		{
			copiaAlarmas[i] = alarmas[i];
			Log(alarmas[i], presionSalida[i], presionEntrada[i], consignaActual[i], caudal[i], volumenAgua[i], acumuladoLocal[i]);
			volumenAgua[i] = 0;
		}
		// DATOS
		if(Timeout(timerReporte[i]))
		{
			timerReporte[i] = ConfigurarTimeout(tLog[i]*1000);
			Log(presionEntrada[i], presionSalida[i], consignaActual[i], caudal[i], volumenAgua[i], acumuladoLocal[i]);
			volumenAgua[i] = 0;
			if (caudalimetro[i] == 2)
				Log(bateria[i]);
		}
	}
} 


