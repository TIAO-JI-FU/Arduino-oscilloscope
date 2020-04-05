#include <iostream>
#include <math.h>
#include <graphics.h>
#include "SerialPort.h"
#define display_width 1200
#define display_height 700
using namespace std;
char *port_name = "\\\\.\\COM3";


class Function_value
{	
	public:	
		char incomingData[MAX_DATA_LENGTH];
		unsigned count,old_data,old_x;
		unsigned int delay_time;
		unsigned long delay_count,num_delay;
		int back_ground_data[display_width][display_height];
};

class back_ground: public Function_value
{
	public:
		void Grid(int width, int height) //print grid
		{
			for(int col=0;col<width;col+=(width/10))     
			{
				for(int row=0;row<height;row+=(height/40))
				{
					 putpixel(col,row,15); 
					 back_ground_data[col][row]=1;
				}
			}
	
			for(int row=0;row<height;row+=(height/8)-2)
			{
				for(int col=0;col<width;col+=(width/60))
				{
					 putpixel(col,row,15); 
					 back_ground_data[col][row]=1;
				}
			}
	
			for(int col=0;col<width;col+=(width/60))
			{
				for(int length=-3;length<=3;length++)
				{
					putpixel(col,(height/2)-10+length,15);
					back_ground_data[col][(height/2)-10+length]=1; 
				}
			}
	
			for(int row=0;row<height;row+=(height/40))
			{
				for(int length=-3;length<=3;length++)
				{
					putpixel((width/2)+length,row,15); 
					back_ground_data[(width/2)+length][row]=1;
				}
			}
		}
	
		void Word()
		{
			settextstyle(4, HORIZ_DIR, 2);   //type
  			outtextxy(0, 700, "Frequent :");
  			outtextxy(0, 725, "Period :");
  			outtextxy(500, 700, "Virtical :");
  			outtextxy(500, 725, "Horizontal :");
  			outtextxy(250, 700, "Hz");
 			outtextxy(250, 725, "ms");
 			outtextxy(705, 700, "div");
  			outtextxy(705, 725, "ms");
		}
};
back_ground print_back_ground;

class display: public Function_value
{
	public:
		unsigned data;
    	char frequent_num[8];
    	char period_num[7];
    
		void get_data()
    	{
    		data = (incomingData[0]%0x30)*1000 + 
		   			(incomingData[1]%0x30)*100 + 
				   (incomingData[2]%0x30)*10 + 
				   (incomingData[3]%0x30); //collect adc_data
 			data = 349-(data*350/1024); //turn into screen size
		}
		
		void get_string()//chang string
		{
			if(incomingData[4]=='0')
			{ frequent_num[0] = ' ';
			    if(incomingData[5]=='0')
				{ frequent_num[1] = ' ';
					if(incomingData[6]=='0')
					{ frequent_num[2] = ' ';}
					else
					{ frequent_num[2] = incomingData[6];}
                }
               	else
				{ frequent_num[2] = incomingData[6];
    			  frequent_num[1] = incomingData[5];
				}
			}
			else
			{ frequent_num[0] = incomingData[4];
			  frequent_num[1] = incomingData[5];
		      frequent_num[2] = incomingData[6];
			}
			
			frequent_num[3] = incomingData[7];
  
			if(incomingData[8]=='0')
			{ period_num[0] = ' ';
				if(incomingData[9]=='0')
				{ period_num[1] = ' ';
					if(incomingData[10]=='0')
					{ period_num[2] = ' ';}
					else
					{ period_num[2]=incomingData[10];}
				}
				else
				{ period_num[1]=incomingData[9];
				  period_num[2]=incomingData[10];
				}
			}
			else
			{ period_num[0]=incomingData[8];
			  period_num[1]=incomingData[9];
			  period_num[2]=incomingData[10];
			}	

			period_num[3]=incomingData[11];
			
			frequent_num[4] = ' ';
			period_num[4] = ' ';
			frequent_num[5] = ' ';
			period_num[5] = ' ';
			frequent_num[6] = ' ';
			frequent_num[7] = '\0';
			period_num[6] = '\0';
		}
		
		void clear_old_data()
		{
			for(int row=0;row<350;row++)   //clear old_data(except back_ground)
 			{
 				if(print_back_ground.back_ground_data[count+1][row]!=1)
 				{
 					putpixel(count+1,row,0);
				}
				else if(print_back_ground.back_ground_data[count+1][row]==1)
				{
					putpixel(count+1,row,15);
				}
			}
		}
		
		void new_wave_data()//drow waveform
		{
			if(abs(data-old_data)<10)
 			{ putpixel(count,data,13); }
 			else if(abs(data-old_data)<100 || abs(data-old_data)>=380)
 			{ putpixel(count,old_data,13); }
 			else//drow line from low to high or high to low
 			{
 				setcolor(13);
 				line(old_x, old_data, count, data);
			}
		}

		void new_string_data()//show the data
		{
			outtextxy(145, 700,frequent_num);
        	outtextxy(140, 725,period_num); 
       		outtextxy(680, 700,"1");
        	outtextxy(650, 725,"100");
		}	 
};

Function_value Value;
display Display;

int main()
{
	SerialPort arduino(port_name); //open your port for arduino
   
	if (arduino.isConnected())  // check is connected
	{
  		cout << "Connection Established" << endl;
	}
  	else 
	{
		cout << "ERROR, check port name";
	}
  
	initwindow(display_width,display_height+50);
	print_back_ground.Grid(display_width,display_height);
  	print_back_ground.Word();
	print_back_ground.Word();
	delay(5000);//open wait
	while (arduino.isConnected()) //every loop will chect is connected
	{
  		if(Display.delay_count==10) //get adc_data every about 100ms
  		{
    		int read_result = arduino.readSerialPort(Display.incomingData, MAX_DATA_LENGTH); //get data from arduino
    		Display.delay_count = 0;
		}
    	else
    	{
    		Display.delay_count++;
		}
    	if(Display.count<1200)
    	{
    		Display.get_data();  //collect data
 			Display.new_wave_data(); //print new data
 			Display.clear_old_data();//clear old_data
			Display.old_data = Display.data; //update old_data
			Display.old_x = Display.count; //update old_x 	
 			if(Display.num_delay==200) //delay for print string
 			{
	 			Display.get_string(); //collect string
 				Display.new_string_data(); //print new string
       			Display.num_delay = 0; 
    		}
    		else
    		{ Display.num_delay++;}
			if(Display.count==1199) //count 
 			{ 
				Display.count=0;
				delay(3400);//wiat for next
			}
 			else
 			{ Display.count++;}
    		Sleep(Display.delay_time);
  		}
 	}
} 















