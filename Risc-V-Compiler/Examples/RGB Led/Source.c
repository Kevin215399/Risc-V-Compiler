void main()
{
    int RLED = 16;
    int GLED = 17;
    int BLED = 18;

    pinMode(16,OUTPUT);
    pinMode(17,OUTPUT);
    pinMode(18,OUTPUT);
    pinMode(2,INPUT_PULLUP);
    pinMode(3,INPUT_PULLUP);
    pinMode(4,INPUT_PULLUP);
    pinMode(5,INPUT_PULLUP);

    int blink = 0;
    bool mode = 0;

    while(true){
        if(!pinRead(2)){
            if(mode == 0){
                pinWrite(RLED,HIGH);
            }
            if(mode == 1 && blink == 0){
                pinToggle(RLED);
            }
        }
        if(pinRead(2)){
            pinWrite(RLED,LOW);
        }
            
        if(!pinRead(3)){
            if(mode == 0){
                pinWrite(GLED,HIGH);
            }
            if(mode == 1 && blink == 0){
                pinToggle(GLED);
            }
        }
        if(pinRead(3)){
            pinWrite(GLED,LOW);
        }
            
        if(!pinRead(4)){
            if(mode == 0){
                pinWrite(BLED,HIGH);
            }
            if(mode == 1 && blink == 0){
                pinToggle(BLED);
            }
        }
        if(pinRead(4)){
            pinWrite(BLED,LOW);
        }

        if(!pinRead(5)){
            mode ^= 1;
            int wait = 0;
            while(wait < 1000000){
                wait+=1;
            }
        }
        blink += 1;
        if(blink > 10000){
            blink = 0;
        }
        
    }
    return 0;
}