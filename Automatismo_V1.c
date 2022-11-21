//Variables de seguridad de presiones para cada sector
const max_de_seguridad_sect_1=25;//para evitar roturas.
const max_de_seguridad_sect_2=25;//para evitar roturas.
//variable para chequeo de comunicaciones que escribe el SCADA y borra el NETTRA
int comunic;
//Variables REGULADORA 1 que escribe el SCADA;
int p11;
int p12;
int p13;
int p14;
int t11;
int t12;
int t13;
int t14;
int t15;
int t16;
int autom_pilotoR1; //desde el scada se selecciona si se regula automatico o a piloto. 
							//autom_pilotoR1=0 automatico,autom_pilotoR1=1 piloto.
							
//Variables REGULADORA 1 que lee el SCADA;
int a_scada_p11;
int a_scada_p12;
int a_scada_p13;
int a_scada_p14;
int consigna_actual1;
int a_scada_alta1;
int a_scada_baja1;
int a_scada_t11;
int a_scada_t12;
int a_scada_t13;
int a_scada_t14;
int a_scada_t15;
int a_scada_t16;
int TemperaturaR10;//si se usa modbus.
int TemperaturaR11;//si se usa modbus.
int CaudalR10;//si se usa modbus.
int CaudalR11;//si se usa modbus.
int TotalizadorR10;//si se usa modbus.
int TotalizadorR11;//si se usa modbus.
int TotalizadorR12;//si se usa modbus.
int TotalizadorR13;//si se usa modbus.
int Bateria1;//si se usa modbus.
int baja1x10;//para uso interno del programa de control, no se ve en el SCADA.
ulong totalizador_pulsos_1;//si se usan pulsos.
ulong litros_x_pulso_caudalim_1;//si se usan pulsos.
float caudal_1;//si se usan pulsos.
float litros_x_pulso_caudalim_1_en_reales;//si se usan pulsos.

//variable prueba
int indice;
float tiempo_entre_pulsos;
int reserva2;

//salidas para la REGULADORA 1
alias elim_piloto1 as Out.0;
alias bajar_pres1 as Out.1;
alias subir_pres1 as Out.2;

int histR11;//histeresis para la presion de la reguladora 1
int histR12;
int histR13;
int histR14;

//Variables REGULADORA 2 que escribe el SCADA;
int p21;
int p22;
int p23;
int p24;
int t21;
int t22;
int t23;
int t24;
int t25;
int t26;
int autom_pilotoR2; //desde el scada se selecciona si se regula automatico o a piloto. 
							//autom_pilotoR1=0 automatico,autom_pilotoR1=1 piloto.

//Variables reguladora 2 que lee el SCADA;
int a_scada_p21;
int a_scada_p22;
int a_scada_p23;
int a_scada_p24;
int consigna_actual2;
int a_scada_alta2;
int a_scada_baja2;
int a_scada_t21;
int a_scada_t22;
int a_scada_t23;
int a_scada_t24;
int a_scada_t25;
int a_scada_t26;
int TemperaturaR20;//si se usa modbus.
int TemperaturaR21;//si se usa modbus.
int CaudalR20;//si se usa modbus.
int CaudalR21;//si se usa modbus.
int TotalizadorR20;//si se usa modbus.
int TotalizadorR21;//si se usa modbus.
int TotalizadorR22;//si se usa modbus.
int TotalizadorR23;//si se usa modbus.
int Bateria2;//si se usa modbus.							
int baja2x10;//para uso interno del programa de control, no se ve en el SCADA.;
ulong totalizador_pulsos_2;//si se usan pulsos.
ulong litros_x_pulso_caudalim_2;//si se usan pulsos.
float caudal_2;//si se usan pulsos.
float litros_x_pulso_caudalim_2_en_reales;//si se usan pulsos.
float Bat_Alim;
int reserva6;
int reserva7;
int reserva8;
int reserva9;
//salidas para la REGULADORA 2
alias elim_piloto2 as Out.4;
alias bajar_pres2 as Out.5;
alias subir_pres2 as Out.6;

int histR21;//histeresis para la presion de la reguladora 2
int histR22;
int histR23;
int histR24;

// variables internas de proceso;
int H_M;
// VARIABLES PARA CAUDALIMETROS DE PULSOS
bool estado1;
bool estado2;
float tiempo1;
float tiempo2;



// Variables para reset en estado conocido
comunic=0;
Timer[0]=0;

// Variables para selección automático o piloto se inicializan en modo automatico.

autom_pilotoR1=0; 
autom_pilotoR2=0;

if (p11<0 || p11>max_de_seguridad_sect_1)
{
	p11=max_de_seguridad_sect_1;
	}
if (p12<0 || p12>max_de_seguridad_sect_1)
{
	p12=max_de_seguridad_sect_1;	
	}
if (p13<0 || p13>max_de_seguridad_sect_1)
{
	p13=max_de_seguridad_sect_1;
	}
if (p14<0 || p14>max_de_seguridad_sect_1)
{
	p14=max_de_seguridad_sect_1;
	}
if (p21<0 || p21>max_de_seguridad_sect_2)
{
	p21=max_de_seguridad_sect_2;
	}
if (p22<0 || p22>max_de_seguridad_sect_2)
{
	p22=max_de_seguridad_sect_2;	
	}
if (p23<0 || p23>max_de_seguridad_sect_2)
{
	p23=max_de_seguridad_sect_2;
	}
if (p24<0 || p24>max_de_seguridad_sect_2)
{
	p24=max_de_seguridad_sect_2;
	}
while(1)
{//escritura de variables para lectura desde Scada;
litros_x_pulso_caudalim_1_en_reales=litros_x_pulso_caudalim_1;//conversion de reales a ulong.
litros_x_pulso_caudalim_2_en_reales=litros_x_pulso_caudalim_2;//cnversion de reales a ulong.
baja1x10=a_scada_baja1*10;
baja2x10=a_scada_baja2*10;
 a_scada_p11=p11;
 a_scada_p12=p12;
 a_scada_p13=p13;
 a_scada_p14=p14;
 a_scada_p21=p21;
 a_scada_p22=p22;
 a_scada_p23=p23;
 a_scada_p24=p24;
 a_scada_t11=t11;
 a_scada_t12=t12;
 a_scada_t13=t13;
 a_scada_t14=t14;
 a_scada_t15=t15;
 a_scada_t16=t16;
 a_scada_t21=t21;
 a_scada_t22=t22;
 a_scada_t23=t23;
 a_scada_t24=t24;
 a_scada_t25=t25;
 a_scada_t26=t26;
 
 a_scada_baja1=AInEscalado[0];
 a_scada_alta1=AInEscalado[1];
 a_scada_baja2=AInEscalado[2];
 a_scada_alta2=AInEscalado[3];
H_M=(Horas*100)+Minutos;//horas y minutos actuales

//Variable para control de bateria
Bat_Alim = AInEscalado[4];


//CAUDALIMETRO 1 CON MODBUS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
TemperaturaR10=TemperR1[1];
TemperaturaR11=TemperR1[0];
CaudalR10=CaudalR1[1];
CaudalR11=CaudalR1[0];
TotalizadorR10=TotalizadorR1[1];
TotalizadorR11=TotalizadorR1[0];
TotalizadorR12=TotalizadorR1[3];
TotalizadorR13=TotalizadorR1[2];
Bateria1=BateriaR1;
//******************************************************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//CAUDALIMETRO 2 CON MODBUS
TemperaturaR20=TemperR2[1];
TemperaturaR21=TemperR2[0];
CaudalR20=CaudalR2[1];
CaudalR21=CaudalR2[0];
TotalizadorR20=TotalizadorR2[1];
TotalizadorR21=TotalizadorR2[0];
TotalizadorR22=TotalizadorR2[3];
TotalizadorR23=TotalizadorR2[2];
Bateria2=BateriaR2;
//************************************************************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//REGULADORA 1 Determinacion consigna a utilizar segun horario e histeresis.
if (t16<H_M | H_M<=t11)
{
		consigna_actual1=p11;
}
	else if (t11<H_M & H_M<=t12)
{
		consigna_actual1=p12;
}
	else if (t12<H_M & H_M<=t13)
{
		consigna_actual1=p13;
}	
	else if (t13<H_M & H_M<=t14)
{
		consigna_actual1=p14;
}
	else if (t14<H_M & H_M<=t15)
{
		consigna_actual1=p13;
}
	else if (t15<H_M & H_M<=t16)
{
		consigna_actual1=p12;
}

histR11=consigna_actual1*110;
histR12=consigna_actual1*106;
histR13=consigna_actual1*94;
histR14=consigna_actual1*90;

//****************************************************************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//REGULADORA 2 determinacion consigna a utilizar segun horario e histeresis.
if (t26<H_M | H_M<=t21)
{
		consigna_actual2=p21;
}
	else if (t21<H_M & H_M<=t22)
{
		consigna_actual2=p22;
}
	else if (t22<H_M & H_M<=t23)
{
		consigna_actual2=p23;
}	
	else if (t23<H_M & H_M<=t24)
{
		consigna_actual2=p24;
}
	else if (t24<H_M & H_M<=t25)
{
		consigna_actual2=p23;
}
	else if (t25<H_M & H_M<=t26)
{
		consigna_actual2=p22;
}

histR21=consigna_actual2*110;
histR22=consigna_actual2*106;
histR23=consigna_actual2*94;
histR24=consigna_actual2*90;

//*************************************************************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//REGULADORA 1 CONTROL;
if (autom_pilotoR1==0) //si es 0 el control se realiza con el equipo. Se elimina el piloto.
{
elim_piloto1=1;
if (baja1x10>histR11)
{
	bajar_pres1=1;	
	subir_pres1=0;
}
	else if (histR13<baja1x10 & baja1x10<histR12)
{
		bajar_pres1=0;
		subir_pres1=0;
}
	else if (histR14>baja1x10)
{
		subir_pres1=1;
		bajar_pres1=0;
}
}
if (autom_pilotoR1==1) // Si es 1 el control se realiza con piloto, se cierran las 2 electroválvulas de control.
{
	elim_piloto1=0;
	bajar_pres1=0;
	subir_pres1=0;
}
//********************************************************************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//REGULADORA 2 CONTROL;
if (autom_pilotoR2==0) //si es 0 el control se realiza con el equipo. Se elimina el piloto.
{
elim_piloto2=1;
if (baja2x10>=histR21)
{
	bajar_pres2=1;	
	subir_pres2=0;
}
	else if (histR23<baja2x10 & baja2x10<histR22)
{
		bajar_pres2=0;
		subir_pres2=0;
}
	else if (histR24>baja2x10)
{
		subir_pres2=1;
		bajar_pres2=0;
}
}
if (autom_pilotoR2==1) // Si es 1 el control se realiza con piloto, se cierran las 2 electroválvulas de control.
{
	elim_piloto2=0;
	bajar_pres2=0;
	subir_pres2=0;
}
//*********************************************************************************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//CAUDALIMETRO 1 POR PULSOS. Maquina de estados para medir tiempo entre pulsos.
	//Cuando se carga el totalizador del caudalimetro en el programa se debe hacer hasta los litros.
	//Por cada pulso detectado, se agregan al totalizador la cantidad de litros correspondientes.
	if (estado1==0 && In.0==1)
	{
		tiempo1=Timer[0];
		estado1=1;
		
		totalizador_pulsos_1=totalizador_pulsos_1+litros_x_pulso_caudalim_1;//calculo del totalizador 1 en ulong.

		Timer[0]=0;
	}
	if(estado1==1 && In.0==0)
	{
		estado1=0;
	}
	// Fin de Maquina de estados
//calculo del caudal 1
caudal_1=36*litros_x_pulso_caudalim_1_en_reales/tiempo1;//Calculo en reales. 
//El caudal expresado en m^3/h se calcula como 3600/tiempo entre dos pulsos consecutivos (tiempo1)*m^3 por pulso.
//Como el tiempo entre dos pulsos se mide en decimas de segundos, se debe dividir entre 10.
//Se tienen los litros por pulso para pasar a m^3 por pulso se divide entre 1000.
//Con esto y simplificando se llega a la fórmula aplicada para calcular el caudal.
//*********************************************************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//CAUDALIMETRO 2 POR PULSOS. Maquina de estados para medir tiempo entre pulsos.
	//Cuando se carga el totalizador del caudalimetro en el programa se debe hacer hasta los litros.
	//Por cada pulso detectado, se agregan al totalizador la cantidad de litros correspondientes.
	/*if (estado2==0 && In.1==1)
	{
		tiempo2=Timer[1];
		estado2=1;
		
		totalizador_pulsos_2=totalizador_pulsos_2+litros_x_pulso_caudalim_2;//calculo del totalizador 2 en ulong.

		Timer[1]=0;
	}
	if(estado2==1 && In.1==0)
	{
		estado2=0;
	}
	// Fin de Maquina de estados
//calculo del caudal 2
caudal_2=36*litros_x_pulso_caudalim_2_en_reales/tiempo2;//Calculo en reales. 
*/
//El caudal expresado en m^3/h se calcula como 3600/tiempo entre dos pulsos consecutivos (tiempo1)*m^3 por pulso.
//Como el tiempo entre dos pulsos se mide en decimas de segundos, se debe dividir entre 10.
//Se tienen los litros por pulso para pasar a m^3 por pulso se divide entre 1000.
//Con esto y simplificando se llega a la fórmula aplicada para calcular el caudal.
//*************************************************************************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//CHEQUEO DE COMUNICACIONES
//el SCADA escribe un 1 y el NETTRA espera 3 minutos y lo borra, 
//si no llega el 1 se resetea el equipo por problemas de comunicaciones.

if (Timer[0]>1800)
{
	if(comunic > 0)
	{
		comunic=0;
		Timer[0]=0;
	} else
	{
	Reset();
	}
}


}

//*******************************************************************************************
//VARIABLES A LEVANTAR POR MODBUS DE LOS CAUDALIMETROS SIEMENS
//CaudalR1 	    offset 3002 largo 2 consulta cada 10 segundos.
//TotalizadorR1 offset 3017 largo 4 consulta cada 21600 segundos.
//BateriaR1     offset 3030 largo 1 consulta cada 21600 segundos.
//TemperR1		 offset 3042 largo 2 consulta cada 21600 segundos.
//Los pulsos del caudalimetro 1 se conenctan en la entrada digital 0.
//Los pulsos del caudalimetro 2 se conenctan en la entrada digital 1.

//CONSIDERACIONES CAUDALIMETRO POR PULSO
//litros_x_pulso_caudalim_2 valor modificado a 1000 
//Asignar a totalizador_pulsos_2 el valor real del Caudalimetro por pulsos