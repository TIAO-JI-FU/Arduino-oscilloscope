#define BAUD 115200
#define avg_num 10
int data,max_data=50,min_data=1000,mid_data=525; 
unsigned delay_count = 0;
unsigned int transfer_data[416],transfer_period[416],count_dataNumber;
int send_dataNumber,perSix;
int count_H = 0,count_L = 0; //ms
int period_H = 0,period_L = 0; //ms
unsigned int period = 0,frequent = 0; 
bool count_H_flag = false;
bool count_L_flag = false;
bool stopTransfer = false;
ISR(TIMER1_OVF_vect) //every 1ms(16MHz/64/250)
{
  TCNT1H = (65536-100)/256; //Count 250 times 
  TCNT1L = (65536-100)%256;
  
  data = analogRead(A0);
  if(count_dataNumber<7496)
  {
    if(count_dataNumber==7495)
    {stopTransfer=0;}
    if(data>max_data)
    { max_data = data; }
    if(data<min_data)
    { min_data = data; }
    mid_data = (max_data+min_data)/2;
    if(count_dataNumber>=5000)
    { perSix++;
      if(perSix==6)
      {transfer_data[(count_dataNumber-5000)/6]=data;}
    }
   if(data>mid_data)
   {
      count_H++; //count period when signal was '1'
      if(count_L_flag == true) //count finsh
      {
       period_L = count_L;
       count_L = 0;
       count_L_flag = false;
      }
      count_H_flag = true;
    }
    else if(data<=mid_data)
    {
     count_L++; //count period when signal was '0'
     if(count_H_flag == true) //count finsh
     {
       period_H = count_H;
       count_H = 0;
       count_H_flag = false; 
     }
     count_L_flag = true;
    }
     period = period_H + period_L; //ms
     if(count_dataNumber>=5000 && perSix==6)
     {
      perSix=0;
      transfer_period[(count_dataNumber-5000)/6]=period;}
     count_dataNumber++;
  }
  else if(count_dataNumber>=7496 && stopTransfer==0)
  {
      if(send_dataNumber==416)
      {
        stopTransfer=1;
        count_dataNumber=0;
        send_dataNumber=0;
      }
      if(delay_count==57) //every 100ms
      {
        delay_count = 0;
        Serial.print(transfer_data[send_dataNumber]/1000%10);
        Serial.print(transfer_data[send_dataNumber]/100%10);
        Serial.print(transfer_data[send_dataNumber]/10%10);
        Serial.print(transfer_data[send_dataNumber]%10);

        Serial.print(2500/transfer_period[send_dataNumber]/1000%10);
        Serial.print(2500/transfer_period[send_dataNumber]/100%10);
        Serial.print(2500/transfer_period[send_dataNumber]/10%10);
        Serial.print(2500/transfer_period[send_dataNumber]%10);
        
        Serial.print(transfer_period[send_dataNumber]*20/49/1000%10);
        Serial.print(transfer_period[send_dataNumber]*20/49/100%10);
        Serial.print(transfer_period[send_dataNumber]*20/49/10%10);
        Serial.println(transfer_period[send_dataNumber]*201/500%10);
        send_dataNumber++;
      }
      else
       {
        delay_count++;
       }
  }
}
void setup()
{
  Serial.begin(BAUD);
  pinMode(A0,INPUT);
  //pinMode(2,INPUT);
  TIMSK1 = 0x01; //open timer1
  TCCR1A = 0x00; //disable compare
  TCCR1B = 0x03; //set prescaler for clk/64 (last 3bits)
}

void loop()
{
  
}

