/*
 * main.c
 *
 *  Created on: 22 sie 2017
 *      Author: Mariusz
 */

#define STOP_TIMER TCCR1B &= ~((1 << CS11) | (1 << CS10))
#define START_TIMER TCCR1B |= (1 << CS11) | (1 << CS10)
#define RESET_TIMER_1 TCNT1 = 0
#define Leg_1 0
#define Leg_2 1
#define Leg_3 2
#define Leg_4 3
#define Leg_5 4
#define Leg_6 5
#define Quantity_of_Legs 6
#define x_Zero_Coordinate 128
#define y_Zero_Coordinate 102
#define z_Zero_Coordinate 128
#define Speed_Zero_Coordinate 100

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

volatile uint8_t Interrupt_Buffer = 0;
volatile uint8_t Moving_Indicator = 0;
volatile uint8_t Controller_x = 128;
volatile uint8_t Controller_y = 102;
volatile uint8_t Controller_z = 128;
volatile uint8_t Controller_Speed = 100;
volatile uint8_t Controller_Buttons = 0;
volatile uint8_t Permission_for_Copying_Coordinates = 1;
volatile uint8_t Voltage_of_Battery = 0;
volatile uint8_t Reset_Command_Protocol_State_Machine = 0;
volatile uint8_t State = 0;

typedef struct T_Variables
{
	float Coxa_Length;
	float Femur_Length;
	float Tibia_Length;
	float Femur_Length_Squared;
	float Tibia_Length_Squared;
	float Tibia_Femur_Length_Squared;
	float x_Zero_Offset;
	float y_Zero_Offset;
	float z_Zero_Offset;
	float Sine_Cosine_45;
	float Gamma_Const_Nominator;
	float Gamma_Const_Denominator;
	float Vertical_Line;
	float Rect_Triangle_Radius_FemTib;
	float Mean_Servo_Resolution;
	float Rad_to_Deg;
	uint8_t *Sync_5_Step;
	uint8_t *Queue_5_Step;
} Variables;

// Struktura przetrzymujaca nowe i stare wartosci katow serw dla kazdej nogi
typedef struct T_Leg_Angles
{
	float Alfa_New;
	float Beta_New;
	float Gamma_New;

	float Alfa_Old;
	float Beta_Old;
	float Gamma_Old;
} Leg_Angles;

// Actual position and speed for all servos in leg
typedef struct T_Leg_Variables_MiniMaestro
{
	int Coxa;
	int Femur;
	int Tibia;

	int Coxa_Speed;
	int Femur_Speed;
	int Tibia_Speed;
} Leg_Variables_MiniMaestro;

// Boundary values for each servo and their resolutions
typedef struct T_Leg_Initial
{
	// Wartosci startowe
	float Coxa_Bound;
	float Femur_Bound;
	float Tibia_Bound;

	// Rozdzielczosc serw
//	float Coxa_Resolution;
//	float Femur_Resolution;
//	float Tibia_Resolution;
} Leg_Initial;

typedef struct T_Motion_Global_Vector
{
	float x;
	float y;
	float z;

	float Speed;
} Motion_Global_Vector;

typedef struct T_Math_Coordinates
{
	float x;
	float y;
	float z;
} Math_Coordinates;

// Struktura glowna programu
typedef struct T_Legs
{
	Leg_Angles Software_Leg;
	Leg_Variables_MiniMaestro MiniMaestro_Leg;
	Leg_Initial Initial_Values;
//	Math_Coordinates Coordinate;
} Legs;

void Initialize_uC (void);
void Initialize_MiniMaestro_UART(long int baud);
void Initialize_CC3100_UART(long int baud);
void MiniMaestro_Transmit(unsigned char data);
unsigned char MiniMaestro_Receive( void );
void CC3100_Transmit(unsigned char data);
unsigned char CC3100_Receive( void );
void MiniMaestro_Get_Moving_State(void);
void MiniMaestro_Set_Speed(char serwo, int predkosc);
void MiniMaestro_Set_Multiple_Targets(Legs *Leg);
void MiniMaestro_Set_Speed_Multiple_Targets(Legs *Leg);
void Set_Global_Vector(Motion_Global_Vector *Vector);
void Calculate_Leg(uint8_t No_Leg, Motion_Global_Vector Vector, Variables *Var, Legs *Leg);
void Rotate_Coordinate_System(uint8_t i, Motion_Global_Vector *Coordinate, Variables *Var);
uint8_t Solve_Inverse_Kinematics(Legs *Leg, Variables *Var, Motion_Global_Vector *Coordinate);
void Motion_5_Step(Legs *Leg, Motion_Global_Vector *Vector, Variables *Var);
float Solve_Gamma_Angle(Motion_Global_Vector *Coordinate, Variables *Var);
float Solve_Alfa_Angle_Area_1(Motion_Global_Vector *Coordinate, Variables *Var, float *Gamma);
float Solve_Alfa_Angle_Area_2(Motion_Global_Vector *Coordinate, Variables *Var, float *Gamma);
float Solve_Alfa_Angle_Area_3(Motion_Global_Vector *Coordinate, Variables *Var, float *Gamma);
float Solve_Alfa_Angle_Area_4(Motion_Global_Vector *Coordinate, Variables *Var, float *Gamma);
void Modify_Vector_For_5_Step(uint8_t Position_Type, uint8_t Number_of_Leg, uint8_t Sequence_Progress, Motion_Global_Vector *Vector);
void Set_All_Servos(Legs *Leg);
void Initialize_ADC (void);
void Update_Battery_Voltage (void);
void Initialize_Timers(void);

int main(void)
{
	Legs Leg[Quantity_of_Legs];
	Motion_Global_Vector Vector;
	Variables Kinematic_Vars;

	uint8_t Sync_5_Step_Table[6][12]={	{0,1,2,3,3,3,3,3,3,3,3,3},
										{3,3,3,3,3,3,3,3,0,1,2,3},
										{3,3,3,3,0,1,2,3,3,3,3,3},
										{2,3,3,3,3,3,3,3,3,3,0,1},
										{3,3,0,1,2,3,3,3,3,3,3,3},
										{3,3,3,3,3,3,0,1,2,3,3,3} };

	// --------------------------------------------
	// ---- KINEMATIC VARIABLES INITIALIZATION ----
	// --------------------------------------------
	Kinematic_Vars.Coxa_Length = 17.5;
	Kinematic_Vars.Femur_Length = 45;
	Kinematic_Vars.Tibia_Length = sqrt((2.194 * 2.194) + (67.481 * 67.481)); // 67,51665718176515290857357754045
	Kinematic_Vars.Femur_Length_Squared = pow(Kinematic_Vars.Femur_Length, 2);
	Kinematic_Vars.Tibia_Length_Squared = pow(Kinematic_Vars.Tibia_Length, 2);
	Kinematic_Vars.Tibia_Femur_Length_Squared = pow(Kinematic_Vars.Femur_Length + Kinematic_Vars.Tibia_Length, 2);
	Kinematic_Vars.x_Zero_Offset = 44.194173824159220275052772631553;
	Kinematic_Vars.y_Zero_Offset = Kinematic_Vars.Femur_Length + Kinematic_Vars.Tibia_Length;
	Kinematic_Vars.z_Zero_Offset = Kinematic_Vars.x_Zero_Offset;
	Kinematic_Vars.Sine_Cosine_45 = 0.7071067811865475244;
	Kinematic_Vars.Gamma_Const_Nominator = pow(Kinematic_Vars.Femur_Length, 2) + pow(Kinematic_Vars.Tibia_Length, 2);
	Kinematic_Vars.Gamma_Const_Denominator = 2 * Kinematic_Vars.Femur_Length * Kinematic_Vars.Tibia_Length;
	Kinematic_Vars.Vertical_Line = 6;
	Kinematic_Vars.Rect_Triangle_Radius_FemTib = sqrt(pow(Kinematic_Vars.Tibia_Length, 2) - pow(Kinematic_Vars.Femur_Length, 2));
	Kinematic_Vars.Mean_Servo_Resolution = (180/M_PI) * 10.46379823;
	Kinematic_Vars.Rad_to_Deg = 180/M_PI;
	Kinematic_Vars.Sync_5_Step = &Sync_5_Step_Table[0][0];

	// --------------------------------------------
	// ------------- SERVO BOUNDARIES -------------
	// --------------------------------------------
	Leg[Leg_1].Initial_Values.Coxa_Bound = 2076.37092035;
	Leg[Leg_1].Initial_Values.Femur_Bound = 523.0081593;
	Leg[Leg_1].Initial_Values.Tibia_Bound = 2350.7418407;

	Leg[Leg_2].Initial_Values.Coxa_Bound = 1979.37092035;
	Leg[Leg_2].Initial_Values.Femur_Bound = 484.0081593;
	Leg[Leg_2].Initial_Values.Tibia_Bound = 2464.7418407;

	Leg[Leg_3].Initial_Values.Coxa_Bound = 1879.37092035;
	Leg[Leg_3].Initial_Values.Femur_Bound = 465.0081593;
	Leg[Leg_3].Initial_Values.Tibia_Bound = 2323.7418407;

	Leg[Leg_4].Initial_Values.Coxa_Bound = 864.37907965;
	Leg[Leg_4].Initial_Values.Femur_Bound = 2247.7418407;
	Leg[Leg_4].Initial_Values.Tibia_Bound = 613.2581593;

	Leg[Leg_5].Initial_Values.Coxa_Bound = 895.12907965;
	Leg[Leg_5].Initial_Values.Femur_Bound = 2371.2418407;
	Leg[Leg_5].Initial_Values.Tibia_Bound = 545.0081593;

	Leg[Leg_6].Initial_Values.Coxa_Bound = 772.62907965;
	Leg[Leg_6].Initial_Values.Femur_Bound = 2461.7418407;
	Leg[Leg_6].Initial_Values.Tibia_Bound = 640.2581593;


	Initialize_uC();

	while(1)
	{
		Motion_5_Step(Leg, &Vector, &Kinematic_Vars);

//		Set_Global_Vector(&Vector);
//
//		for(uint8_t i=0; i<6; i++)
//		{
//			Calculate_Leg(i, Vector, &Kinematic_Vars, Leg);
//		}
//
//		Set_All_Servos(Leg);
	}
}

void Initialize_ADC (void)
{

}

void Initialize_Timers(void)
{
	TCCR1B |= (1 << WGM12);
	STOP_TIMER;
	OCR1A = 6250;
	TIMSK1 |= (1 << OCIE1A);
}

void Update_Battery_Voltage (void)
{
	// Œci¹gniecie danych z rejestru

	CC3100_Transmit(0x44);
	CC3100_Transmit(0x20);
	CC3100_Transmit(Voltage_of_Battery);
}

void Calculate_Leg(uint8_t No_Leg, Motion_Global_Vector Vector, Variables *Var, Legs *Leg)
{
	Rotate_Coordinate_System(No_Leg, &Vector, Var);
	if(Solve_Inverse_Kinematics(&Leg[No_Leg], Var, &Vector) != 0) return;

	if(No_Leg < 3)
	{
		Leg[No_Leg].MiniMaestro_Leg.Coxa = 4 * (Leg[No_Leg].Initial_Values.Coxa_Bound - (Leg[No_Leg].Software_Leg.Beta_New * Var->Mean_Servo_Resolution));
		Leg[No_Leg].MiniMaestro_Leg.Femur = 4 * (Leg[No_Leg].Initial_Values.Femur_Bound + (Leg[No_Leg].Software_Leg.Alfa_New * Var->Mean_Servo_Resolution));
		Leg[No_Leg].MiniMaestro_Leg.Tibia = 4 * (Leg[No_Leg].Initial_Values.Tibia_Bound - (Leg[No_Leg].Software_Leg.Gamma_New * Var->Mean_Servo_Resolution));
	}
	else
	{
		Leg[No_Leg].MiniMaestro_Leg.Coxa = 4 * (Leg[No_Leg].Initial_Values.Coxa_Bound + (Leg[No_Leg].Software_Leg.Beta_New * Var->Mean_Servo_Resolution));
		Leg[No_Leg].MiniMaestro_Leg.Femur = 4 * (Leg[No_Leg].Initial_Values.Femur_Bound - (Leg[No_Leg].Software_Leg.Alfa_New * Var->Mean_Servo_Resolution));
		Leg[No_Leg].MiniMaestro_Leg.Tibia = 4 * (Leg[No_Leg].Initial_Values.Tibia_Bound + (Leg[No_Leg].Software_Leg.Gamma_New * Var->Mean_Servo_Resolution));
	}


	Leg[No_Leg].MiniMaestro_Leg.Coxa_Speed = lround(fabsf(Leg[No_Leg].Software_Leg.Beta_Old - Leg[No_Leg].Software_Leg.Beta_New) * Vector.Speed * Var->Rad_to_Deg);
	Leg[No_Leg].MiniMaestro_Leg.Femur_Speed = lround(fabsf(Leg[No_Leg].Software_Leg.Alfa_Old - Leg[No_Leg].Software_Leg.Alfa_New) * Vector.Speed * Var->Rad_to_Deg);
	Leg[No_Leg].MiniMaestro_Leg.Tibia_Speed = lround(fabsf(Leg[No_Leg].Software_Leg.Gamma_Old - Leg[No_Leg].Software_Leg.Gamma_New) * Vector.Speed * Var->Rad_to_Deg);
}

void Rotate_Coordinate_System(uint8_t i, Motion_Global_Vector *Coordinate, Variables *Var)
{
	float x_Buffer = Coordinate->x;
	float z_Buffer = Coordinate->z;

	switch(i)
	{
		case Leg_1:
			Coordinate->x = Var->x_Zero_Offset - x_Buffer;
			Coordinate->z = Var->z_Zero_Offset + z_Buffer;
		break;

		case Leg_2:
			Coordinate->x = Var->x_Zero_Offset - (Var->Sine_Cosine_45 * (x_Buffer + z_Buffer));
			Coordinate->z = Var->z_Zero_Offset + (Var->Sine_Cosine_45 * (z_Buffer - x_Buffer));
		break;

		case Leg_3:
			Coordinate->x = Var->x_Zero_Offset - z_Buffer;
			Coordinate->z = Var->z_Zero_Offset - x_Buffer;
		break;

		case Leg_4:
			Coordinate->x = Var->x_Zero_Offset - z_Buffer;
			Coordinate->z = Var->z_Zero_Offset + x_Buffer;
		break;

		case Leg_5:
			Coordinate->x = Var->x_Zero_Offset + (Var->Sine_Cosine_45 * (x_Buffer - z_Buffer));
			Coordinate->z = Var->z_Zero_Offset + (Var->Sine_Cosine_45 * (x_Buffer + z_Buffer));
		break;

		case Leg_6:
			Coordinate->x = Var->x_Zero_Offset + x_Buffer;
			Coordinate->z = Var->z_Zero_Offset + z_Buffer;
		break;
	}
}

uint8_t Solve_Inverse_Kinematics(Legs *Leg, Variables *Var, Motion_Global_Vector *Coordinate)
{
	float Alfa, Beta, Gamma;

	Coordinate->y = Var->y_Zero_Offset - Coordinate->y;

	// Calculate Beta and x'
	Beta = atan(Coordinate->z / Coordinate->x);
	Coordinate->x = ((Coordinate->z)/(sin(Beta))) - Var->Coxa_Length;

	if(pow(Coordinate->x, 2) + pow((Coordinate->y - Var->y_Zero_Offset), 2) <= Var->Tibia_Femur_Length_Squared)
	{
		if((pow(Coordinate->x, 2) + pow((Coordinate->y - Var->y_Zero_Offset - Var->Femur_Length), 2)) >= Var->Tibia_Length_Squared)
		{
			if(Coordinate->x >= Var->Vertical_Line)
			{
				if(Coordinate->y < 0)
				{
					Gamma = Solve_Gamma_Angle(Coordinate, Var);
					Alfa = Solve_Alfa_Angle_Area_1(Coordinate, Var, &Gamma);
				}
				else
				{
					if(pow((Coordinate->x - Var->Femur_Length), 2) + pow(Coordinate->y - Var->y_Zero_Offset, 2) > Var->Tibia_Length_Squared)
					{
						Gamma = Solve_Gamma_Angle(Coordinate, Var);
						Alfa = Solve_Alfa_Angle_Area_4(Coordinate, Var, &Gamma);
					}
					else
					{
						if(pow(Coordinate->x, 2) + pow(Coordinate->y - Var->y_Zero_Offset, 2) > Var->Rect_Triangle_Radius_FemTib)
						{
							Gamma = Solve_Gamma_Angle(Coordinate, Var);
							Alfa = Solve_Alfa_Angle_Area_3(Coordinate, Var, &Gamma);
						}
						else
						{
							Gamma = Solve_Gamma_Angle(Coordinate, Var);
							Alfa = Solve_Alfa_Angle_Area_2(Coordinate, Var, &Gamma);
						}
					}
				}
			}
			else return 1;
		}
		else return 1;
	}
	else return 1;

	Leg->Software_Leg.Alfa_Old = Leg->Software_Leg.Alfa_New;
	Leg->Software_Leg.Beta_Old = Leg->Software_Leg.Beta_New;
	Leg->Software_Leg.Gamma_Old = Leg->Software_Leg.Gamma_New;

	Leg->Software_Leg.Alfa_New = Alfa;
	Leg->Software_Leg.Beta_New = Beta;
	Leg->Software_Leg.Gamma_New = Gamma;

	return 0;
}

float Solve_Gamma_Angle(Motion_Global_Vector *Coordinate, Variables *Var)
{
	float Gamma;

	Gamma = acos((pow(Coordinate->x, 2) + pow(Coordinate->y, 2) - Var->Gamma_Const_Nominator) / (Var->Gamma_Const_Denominator));

	return Gamma;
}

float Solve_Alfa_Angle_Area_1(Motion_Global_Vector *Coordinate, Variables *Var, float *Gamma)
{
	float Alfa;

	Alfa = acos((Var->Tibia_Length * sin(*Gamma))/(sqrt(pow(Coordinate->x, 2) + pow(Coordinate->y, 2)))) - atan(fabsf(Coordinate->y)/(Coordinate->x));

	return Alfa;
}

float Solve_Alfa_Angle_Area_2(Motion_Global_Vector *Coordinate, Variables *Var, float *Gamma)
{
	float Alfa;

	Alfa = -acos((Var->Tibia_Length * sin(*Gamma))/(sqrt(pow(Coordinate->x, 2) + pow(Coordinate->y, 2)))) + atan((Coordinate->y)/(Coordinate->x));

	return Alfa;
}

float Solve_Alfa_Angle_Area_3(Motion_Global_Vector *Coordinate, Variables *Var, float *Gamma)
{
	float Alfa;

	Alfa = acos((Var->Tibia_Length * sin(*Gamma))/(sqrt(pow(Coordinate->x, 2) + pow(Coordinate->y, 2)))) + atan((Coordinate->y)/(Coordinate->x));

	return Alfa;
}

float Solve_Alfa_Angle_Area_4(Motion_Global_Vector *Coordinate, Variables *Var, float *Gamma)
{
	float Alfa;

	Alfa = 90 + acos((Var->Tibia_Length * sin(*Gamma))/(sqrt(pow(Coordinate->x, 2) + pow(Coordinate->y, 2)))) - atan((Coordinate->y)/(Coordinate->x));

	return Alfa;
}

void Motion_5_Step(Legs *Leg, Motion_Global_Vector *Vector, Variables *Var)
{
	uint8_t i, j, k;
	Motion_Global_Vector Temp_Vector;

	for(i=0; i<12; i++)
	{
		Set_Global_Vector(Vector);

		if(Vector->x > -1 && Vector->x < 1 && Vector->z > -1 && Vector->z < 1)
		{
			Controller_x = x_Zero_Coordinate;
			Controller_y = y_Zero_Coordinate;
			Controller_z = z_Zero_Coordinate;
			Controller_Speed = Speed_Zero_Coordinate;
			Set_Global_Vector(Vector);

			for(k=0; k<6; k++)
			{
				Calculate_Leg(k, *Vector, Var, Leg);
			}

			Set_All_Servos(Leg);
		}
		else
		{

			for(j=0; j<6; j++)
			{
				Temp_Vector.x = Vector->x;
				Temp_Vector.y = Vector->y;
				Temp_Vector.z = Vector->z;
				Temp_Vector.Speed = Vector->Speed;

				Modify_Vector_For_5_Step(*((Var->Sync_5_Step) + 12*j + i), j, i, &Temp_Vector);
				Calculate_Leg(j, Temp_Vector, Var, Leg);
			}

			Set_All_Servos(Leg);
		}
	}
}

void Modify_Vector_For_5_Step(uint8_t Position_Type, uint8_t Number_of_Leg, uint8_t Sequence_Progress, Motion_Global_Vector *Vector)
{
	float x, y;

	x = Vector->x / 10;
	y = Vector->z / 10;

	switch(Number_of_Leg)
	{
		case 0:
			if(Sequence_Progress > 2) Sequence_Progress -= 1;
		break;

		case 1:
			if(Sequence_Progress > 10) Sequence_Progress -= 10;
			else if(Sequence_Progress < 8) Sequence_Progress += 2;
		break;

		case 2:
			if(Sequence_Progress > 6) Sequence_Progress -= 6;
			else if(Sequence_Progress < 4) Sequence_Progress += 6;
		break;

		case 3:
		break;

		case 4:
			if(Sequence_Progress > 4) Sequence_Progress -= 4;
			else if(Sequence_Progress < 2) Sequence_Progress += 8;
		break;

		case 5:
			if(Sequence_Progress > 8) Sequence_Progress -= 8;
			else if(Sequence_Progress < 6) Sequence_Progress += 4;
		break;
	}

	switch(Position_Type)
	{
		case 0: //Podnies noge 1
			Vector->x = -x * 5.0;
			Vector->z = -y * 5.0;
			Vector->y += 20.0;
		break;

		case 1: //Podnies noge 2
			Vector->x = x * 5.0;
			Vector->z = y * 5.0;
			Vector->y += 20.0;
		break;

		case 2: //Pozycja skrajna
			Vector->x = x * 5.0;
			Vector->z = y * 5.0;
		break;

		case 3: //Pozycja posrednia
			Vector->x = 5.0 * x - (float)Sequence_Progress * 10.0 * x / 9.0;
			Vector->z = 5.0 * y - (float)Sequence_Progress * 10.0 * y / 9.0;
		break;
	}
}

void Set_All_Servos(Legs *Leg)
{
	MiniMaestro_Get_Moving_State();
	MiniMaestro_Set_Speed_Multiple_Targets(Leg);
	MiniMaestro_Set_Multiple_Targets(Leg);
}

void Set_Global_Vector(Motion_Global_Vector *Vector)
{
	if(Permission_for_Copying_Coordinates == 1)
	{
		Vector->x = (float)(Controller_x - 128) * (20.0/255.0);
		Vector->y = (float)Controller_y * (112.51665718176515290857357754045/255.0);
		Vector->z = (float)(Controller_z - 128) * (-20.0/255.0);
		Vector->Speed = (float)Controller_Speed * (5.0/255.0);
	}
}

void Initialize_uC (void)
{
	Initialize_MiniMaestro_UART(50000); // UART for MiniMaestro
	Initialize_CC3100_UART(3125); // UART for CC3100
	Initialize_ADC();
	Initialize_Timers();

	DDRC |= 0x10;
	PORTC |= 0x10;

	sei(); // Set global interrupts
}

void Initialize_MiniMaestro_UART(long int baud)
{
	// Wyliczenie UBRR dla trybu asynchronicznego (U2X=0)
	uint16_t _ubr = (F_CPU/16/baud-1);
	// Ustawienie predkosci
	UBRR0H |= (uint8_t)(_ubr>>8);
	UBRR0L |= (uint8_t) _ubr;
	// Zalaczenie nadajnika i odbiornika, ustawienie przerwan dla rxc
	UCSR0B |= (1 << RXEN0)|(1 << TXEN0)|(1 << RXCIE0);
	// Ustawienie formatu ramki: 8 bitow danych, 1 bit stopu
	UCSR0C |= (3 << UCSZ00);
	UCSR0C &= ~(1 << USBS0);
}

void Initialize_CC3100_UART(long int baud)
{
	// Wyliczenie UBRR dla trybu asynchronicznego (U2X=0)
	uint16_t _ubr = (F_CPU/16/baud-1);
	// Ustawienie predkosci
	UBRR1H = (uint8_t)(_ubr>>8);
	UBRR1L = (uint8_t) _ubr;
	// Zalaczenie nadajnika i odbiornika, ustawienie przerwan dla rxc
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	// Ustawienie formatu ramki: 8 bitow danych, 1 bit stopu
	UCSR1C = (3 << UCSZ10);
	UCSR1C &= ~(1 << USBS1);
}

void MiniMaestro_Transmit (unsigned char data)
{
	// Czekaj az bufor nadawczy bedzie pusty
	while(!(UCSR0A & (1 << UDRE0)));
	// Wrzuc dane do bufora nadawczego -> start transmisji
	UDR0 = data;
}

unsigned char MiniMaestro_Receive (void)
{
   // Czekaj na dane do odebrania
   while (!(UCSR0A & (1<<RXC0)));
   // Zwroc dane z bufora
   return UDR0;
}

void CC3100_Transmit (unsigned char data)
{
	// Czekaj az bufor nadawczy bedzie pusty
	while(!(UCSR1A & (1 << UDRE1)));
	// Wrzuc dane do bufora nadawczego -> start transmisji
	UDR1 = data;
}

unsigned char CC3100_Receive (void)
{
   // Czekaj na dane do odebrania
   while (!(UCSR1A & (1<<RXC1)));
   // Zwroc dane z bufora
   return UDR1;
}

void MiniMaestro_Get_Moving_State(void)
{
//	while(!(UCSR0A & (1 << UDRE0))); // Odczekaj az bufor nadawczy bedzie pusty

	//Odczekaj az ostatni bajt zostanie wyslany - ustawienie flagi
//	UCSR0B |= 1 << TXCIE0;
//	while(TX_EN_0);
//	UCSR0B &= ~(1 << TXCIE0);
//
//	TX_EN_0 = 1;
//	move_EN = 1;

	Moving_Indicator = 1;
	while(Moving_Indicator) MiniMaestro_Transmit(0x93);
}

void MiniMaestro_Set_Speed(char serwo, int predkosc)
{
	// Odczekaj aby przelaczyc odbieranie/nadawanie
	while(!(UCSR0A & (1 << UDRE0)));

	// Naglowek protokolu Set_Speed
	MiniMaestro_Transmit(0x87);
	MiniMaestro_Transmit(serwo);

	// Ustaw predkosc
	MiniMaestro_Transmit(predkosc & 0x7F);
	MiniMaestro_Transmit((predkosc >> 7) & 0x7F);
}

void MiniMaestro_Set_Speed_Multiple_Targets(Legs *Leg)
{
	uint8_t i;
	uint8_t Sequence[6] = {Leg_1, Leg_6, Leg_2, Leg_5, Leg_3, Leg_4};

	for(i=0; i<6; i++)
	{
		MiniMaestro_Set_Speed(3*i, Leg[Sequence[i]].MiniMaestro_Leg.Coxa_Speed);
		MiniMaestro_Set_Speed(3*i+1, Leg[Sequence[i]].MiniMaestro_Leg.Femur_Speed);
		MiniMaestro_Set_Speed(3*i+2, Leg[Sequence[i]].MiniMaestro_Leg.Tibia_Speed);
	}

}

void MiniMaestro_Set_Multiple_Targets(Legs *Leg)
{
	uint8_t i;
	uint8_t Sequence[6] = {Leg_1, Leg_6, Leg_2, Leg_5, Leg_3, Leg_4};

	// Header of Set_Multiple_Servos
	MiniMaestro_Transmit(0x9f); // Constant
	MiniMaestro_Transmit(0x12); // No. of servos -> 12 hex == 18 dec
	MiniMaestro_Transmit(0x00); // No. of first channel

	// Set positions
	for(i=0; i<6; i++)
	{
		MiniMaestro_Transmit(Leg[Sequence[i]].MiniMaestro_Leg.Coxa & 0x7F);
		MiniMaestro_Transmit((Leg[Sequence[i]].MiniMaestro_Leg.Coxa >> 7) & 0x7F);

		MiniMaestro_Transmit(Leg[Sequence[i]].MiniMaestro_Leg.Femur & 0x7F);
		MiniMaestro_Transmit((Leg[Sequence[i]].MiniMaestro_Leg.Femur >> 7) & 0x7F);

		MiniMaestro_Transmit(Leg[Sequence[i]].MiniMaestro_Leg.Tibia & 0x7F);
		MiniMaestro_Transmit((Leg[Sequence[i]].MiniMaestro_Leg.Tibia >> 7) & 0x7F);
	}
}

// --------------------------------------------
// ---------------- INTERRUPTS ----------------
// --------------------------------------------

ISR(TIMER1_COMPA_vect)
{
	Reset_Command_Protocol_State_Machine = 1;
	Permission_for_Copying_Coordinates = 1;
	STOP_TIMER;
}

ISR(USART0_RX_vect)
{
	Interrupt_Buffer = UDR0; //Zapamietaj dane z bufora UDR

	if(Moving_Indicator == 1 && Interrupt_Buffer == 0x00) //Sprawdz warunek odbierania z MiniMaestro
	{
		Moving_Indicator = 0; //Warunek opuszczenia petli
	}
}

ISR(USART1_RX_vect)
{
	Interrupt_Buffer = UDR1;

//	if(Interrupt_Buffer == 0x55)
//	{
//		Controller_x = 255;
//		Controller_y = 102;
//		Controller_z = 128;
//	}

	if(Reset_Command_Protocol_State_Machine == 1)
	{
		State = 0;
		Reset_Command_Protocol_State_Machine = 0;
	}

	switch(State)
	{
		// Beginning of command protocol
		case 0:
			if(Interrupt_Buffer == 0x55) State = 1;
			RESET_TIMER_1;
//			START_TIMER;
		break;

		// Commands recognition
		case 1:
			if(Interrupt_Buffer == 0x01)
			{
				State = 10;
//				Permission_for_Copying_Coordinates = 0;
			}
			if(Interrupt_Buffer == 0x02) State = 20;
		break;

		// Motion
		case 10:
			Controller_x = Interrupt_Buffer;
			State = 11;
		break;

		case 11:
			Controller_y = Interrupt_Buffer;
			State = 12;
		break;

		case 12:
			Controller_z = Interrupt_Buffer;
			State = 13;
		break;

		case 13:
			Controller_Speed = Interrupt_Buffer;
			State = 0;
		break;

		case 14:
			Controller_Buttons = Interrupt_Buffer;
			State = 0;
			Permission_for_Copying_Coordinates = 1;
			STOP_TIMER;
		break;

		// Measurements
		case 20:
			// Zezwolenie na wys³anie danych pomiarowych
			State = 0;
			STOP_TIMER;
		break;
	}
}
