int RLED = 16;
int GLED = 17;
int RBUTTON = 2;


void delay(int cycles){
    int x = 0;
    while(x<cycles){
        x+=1;
    }
    return;
}
void Alarm(){
    while(true){
        pinToggle(RLED);

        delay(1000000);

        if(!pinRead(RBUTTON)){
            break;
        }
        
    }
    pinWrite(RLED,LOW);
    pinWrite(GLED,HIGH);
    delay(5000000);
    pinWrite(GLED,LOW);
    delay(10000000);
    
    Alarm();
    return;
}
void main()
{

    pinMode(RLED,OUTPUT);
    pinMode(GLED,OUTPUT);
    pinMode(RBUTTON,INPUT_PULLUP);

    Alarm();

    return 0;
}